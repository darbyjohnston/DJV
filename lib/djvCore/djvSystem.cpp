//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvSystem.cpp

#include <djvSystem.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvMemoryBuffer.h>
#include <djvStringUtil.h>

#include <QCoreApplication>
#include <QDir>
#include <QRegExp>
//#include <QRegularExpression>
#include <QSet>

#if defined(DJV_WINDOWS)

#include <windows.h>

#else // DJV_WINDOWS

#include <sys/ioctl.h>
#include <sys/termios.h>
#include <sys/utsname.h>
#include <unistd.h>

#endif // DJV_WINDOWS

#include <stdlib.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// djvSystem
//------------------------------------------------------------------------------

djvSystem::~djvSystem()
{}

#if defined(DJV_WINDOWS)

namespace
{

enum WINDOWS
{
    WINDOWS_UNKNOWN,
    WINDOWS_2000,
    WINDOWS_XP,
    WINDOWS_XP_PRO_64,
    WINDOWS_SERVER_2003,
    WINDOWS_SERVER_2003_R2,
    WINDOWS_VISTA,
    WINDOWS_SERVER_2008,
    WINDOWS_7,
    WINDOWS_SERVER_2008_R2,

    _WINDOWS_SIZE
};

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

WINDOWS windowsVersion()
{
    //DJV_DEBUG("windowsVersion");

    WINDOWS out = WINDOWS_UNKNOWN;

    // Get OS version information.

    ::OSVERSIONINFOEX info;
    djvMemory::zero(&info, sizeof(::OSVERSIONINFOEX));
    info.dwOSVersionInfoSize = sizeof(::OSVERSIONINFOEX);

    if (! ::GetVersionEx((::OSVERSIONINFO *)&info))
        return out;

    //DJV_DEBUG_PRINT("version major = " <<
    //    static_cast<int>(info.dwMajorVersion));
    //DJV_DEBUG_PRINT("version minor = " <<
    //    static_cast<int>(info.dwMinorVersion));
    //DJV_DEBUG_PRINT("NT Workstation = " <<
    //    (info.wProductType == VER_NT_WORKSTATION ? 1 : 0));

    // Get system informaion.

    ::SYSTEM_INFO systemInfo;
    djvMemory::zero(&systemInfo, sizeof(::SYSTEM_INFO));

    if (PGNSI pGNSI = (PGNSI)::GetProcAddress(
        ::GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo"))
    {
        pGNSI(&systemInfo);
    }
    else
    {
        ::GetSystemInfo(&systemInfo);
    }

    // Use OS version and system information to determnine the version name.

    switch (info.dwMajorVersion)
    {
        case 5:

            switch (info.dwMinorVersion)
            {
                case 0: out = WINDOWS_2000; break;
                case 1: out = WINDOWS_XP;   break;

                case 2:

                    if (
                        VER_NT_WORKSTATION == info.wProductType &&
                        PROCESSOR_ARCHITECTURE_AMD64 ==
                            systemInfo.wProcessorArchitecture)
                    {
                        out = WINDOWS_XP_PRO_64;
                        
                        break;
                    }

                    if (GetSystemMetrics(SM_SERVERR2) == 0)
                    {
                        out = WINDOWS_SERVER_2003;
                    }
                    else
                    {
                        out = WINDOWS_SERVER_2003_R2;
                    }

                    break;
            }

            break;

        case 6:

            switch (info.dwMinorVersion)
            {
                case 0:
                
                    if (info.wProductType == VER_NT_WORKSTATION)
                    {
                        out = WINDOWS_VISTA;
                    }
                    else
                    {
                        out = WINDOWS_SERVER_2008;
                    }

                    break;
                
                case 1:
                
                    if (info.wProductType == VER_NT_WORKSTATION)
                    {
                        out = WINDOWS_7;
                    }
                    else
                    {
                        out = WINDOWS_SERVER_2008_R2;
                    }

                    break;
            }

            break;
    }

    //DJV_DEBUG_PRINT("out = " << out);

    return out;
}

const QStringList & windowsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvSystem", "Unknown") <<
        qApp->translate("djvSystem", "Windows 2000") <<
        qApp->translate("djvSystem", "Windows XP") <<
        qApp->translate("djvSystem", "Windows XPProfessional x64 Edition") <<
        qApp->translate("djvSystem", "Windows Server2003") <<
        qApp->translate("djvSystem", "Windows Server 2003 R2") <<
        qApp->translate("djvSystem", "Windows Vista") <<
        qApp->translate("djvSystem", "Windows Server 2008") <<
        qApp->translate("djvSystem", "Windows 7") <<
        qApp->translate("djvSystem", "Windows Server 2008 R2");

    DJV_ASSERT(data.count() == _WINDOWS_SIZE);

    return data;
}

} // namespace

#endif // DJV_WINDOWS

QString djvSystem::info()
{
    QString out;

#if defined(DJV_WINDOWS)

    out = windowsLabels()[windowsVersion()];

#else // DJV_WINDOWS

    ::utsname info;
    uname(&info);

    out = QString("%1 %2 %3").
        arg(info.sysname).
        arg(info.release).
        arg(info.machine);

#endif // DJV_WINDOWS

    return out;
}
    
const QString & djvSystem::djvPathEnv()
{
    static const QString var = "DJV_PATH";
    
    return var;
}

const QString & djvSystem::ldLibPathEnv()
{
    static const QString var = "LD_LIBRARY_PATH";
    
    return var;
}

QStringList djvSystem::searchPath()
{
    //DJV_DEBUG("djvSystem::searchPath");

    QStringList out;

    // Add paths from environment variables.

    QString path = env(djvPathEnv());

    if (! path.isEmpty())
    {
        //DJV_DEBUG_PRINT("DJV_PATH = " << path);

        QString match;
        Q_FOREACH(QChar c, djvFileInfoUtil::listSeparators)
            match += c;
        
        out += path.split(
            QRegExp(QString("[%1]+").arg(match)),
            QString::SkipEmptyParts);
    }

    path = env(ldLibPathEnv());

    if (! path.isEmpty())
    {
        //DJV_DEBUG_PRINT("LD_LIBRARY_PATH = " << path);

        QString match;
        Q_FOREACH(QChar c, djvFileInfoUtil::listSeparators)
            match += c;
        
        //DJV_DEBUG_PRINT("match = " << match);

        out += path.split(
            QRegExp(QString("[%1]+").arg(match)),
            QString::SkipEmptyParts);
    }

    // Add the application path.
    
    //DJV_DEBUG_PRINT("qApp = " << qApp);
        
    const QString applicationPath = qApp->applicationDirPath();

    //DJV_DEBUG_PRINT("application path = " << applicationPath);

    out += applicationPath;
    
    // Add library and translation paths.

    out += QDir(applicationPath + "/../lib").absolutePath();
    out += QDir(applicationPath + "/../translations").absolutePath();

#if defined(DJV_WINDOWS) || defined(DJV_OSX)

    const QString dirName = QDir(qApp->applicationDirPath()).dirName();

    out += QDir(applicationPath + "/../../lib/" + dirName).absolutePath();
    out += QDir(applicationPath + "/../../translations/" + dirName).absolutePath();

#endif

    // Add the build directories.
    
    out += QDir("lib/djvCore").absolutePath();
    out += QDir("lib/djvGui").absolutePath();
    out += QDir("lib/djvViewLib").absolutePath();
    out += QDir("bin/djv_ls").absolutePath();
    out += QDir("bin/djv_info").absolutePath();
    out += QDir("bin/djv_convert").absolutePath();

    // Add the current directory.

    out += ".";

    //DJV_DEBUG_PRINT("out = " << out);

    QSet<QString> set;

    for (int i = 0; i < out.count(); ++i)
        set.insert(out[i]);

    QList<QString> list = set.toList();

    out.clear();
    Q_FOREACH(const QString & s, list)
        out += s;

    return out;
}

QString djvSystem::findFile(const QString & fileName)
{
    Q_FOREACH(const QString & path, searchPath())
    {
        QFileInfo fileInfo(path + "/" + fileName);
        
        if (fileInfo.exists())
        {
            return fileInfo.absoluteFilePath();
        }
    }
    
    return QString();
}

int djvSystem::terminalWidth()
{
    int out = 80;

#if defined(DJV_WINDOWS)

    HANDLE h = ::GetStdHandle(STD_OUTPUT_HANDLE);

    if (h != INVALID_HANDLE_VALUE)
    {
        CONSOLE_SCREEN_BUFFER_INFO info;

        if (::GetConsoleScreenBufferInfo(h, &info))
        {
            out = info.dwSize.X;
        }
    }

#else // DJV_WINDOWS

    struct winsize ws;
    ws.ws_col = 0;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        ws.ws_col = 80;
    }

    out = ws.ws_col;

#endif // DJV_WINDOWS

    return out;
}

void djvSystem::print(const QString & in, bool newline, int indent)
{
    //DJV_DEBUG("djvSystem::print");
    //DJV_DEBUG_PRINT("newline = " << newline);
    
    const QStringList lines = in.split('\n');
    
    const int width = terminalWidth() - 1 - indent;

    //DJV_DEBUG_PRINT("lines = " << lines.count());
    //DJV_DEBUG_PRINT("width = " << width);

    for (int i = 0; i < lines.count(); ++i)
    {
        QString line = lines[i];
        
        QStringList chunks;
        
        if (line.length() > width)
        {
            int indent = 0;
            for (
                ;
                indent < line.length() &&
                indent < width - 1 &&
                ' ' == line[indent];
                ++indent)
                ;
            
            line.remove(0, indent);
            
            while (line.length() > 0)
            {
                //DJV_DEBUG_PRINT("line = \"" << line << "\"");
                
                int l = djvMath::min(width - indent, line.length());
                if (l < line.length())
                    for (; l > 0 && ' ' != line[l - 1]; --l)
                        ;
                
                const int r = l > 0 ? l : (width - indent);
                const QString chunk = line.mid(0, r);
                
                //DJV_DEBUG_PRINT("chunk = \"" << chunk << "\"");
                
                chunks += QString(indent, ' ') + chunk;
                line.remove(0, r);
            }
        }
        else
        {
            chunks += line;
        }
        
        /*::printf("%s", chunks.join("\n").toLatin1().data());
        
        if (i < lines.count() - 1)
        {
            ::printf("\n");
        }*/

        for (int j = 0; j < chunks.count(); ++j)
        {
            ::printf("%s", QString("%1%2").
                arg("", -indent).
                arg(chunks[j]).toLatin1().data());

            if (j < chunks.count() - 1)
            {
                ::printf("\n");
            }
        }

        if (i < lines.count() - 1)
        {
            ::printf("\n");
        }
    }
    
    if (newline)
    {
        ::printf("\n");
    }
    else
    {
        ::fflush(stdout);
    }
}

int djvSystem::exec(const QString & in)
{
    return ::system(in.toLatin1().data());
}

QString djvSystem::env(const QString & in)
{
    QString out;

#   if defined(DJV_WINDOWS)

    size_t size = 0;

    char * p = 0;

    if (0 == _dupenv_s(&p, &size, in.toLatin1().data()))
    {
        out = p;
    }

    if (p)
    {
        free(p);
    }

#   else // DJV_WINDOWS

    if (const char * p = ::getenv(in.toLatin1().data()))
    {
        out = p;
    }

#   endif // DJV_WINDOWS

    return out;
}

bool djvSystem::setEnv(const QString & var, const QString & value)
{
#if defined(DJV_WINDOWS)

    return ::_putenv_s(var.toLatin1().data(), value.toLatin1().data()) == 0;

#else // DJV_WINDOWS

    return ::setenv(var.toLatin1().data(), value.toLatin1().data(), 1) == 0;

#endif // DJV_WINDOWS
}

QStringList djvSystem::drives()
{
    QStringList out;
    
    Q_FOREACH(const QFileInfo & fileInfo, QDir::drives())
    {
        out += fileInfo.absoluteFilePath();
    }
    
#if defined(DJV_OSX)

    Q_FOREACH(const QFileInfo & fileInfo, QDir("/Volumes").entryInfoList())
    {
        const QString & path = fileInfo.absoluteFilePath();
        
        if (! out.contains(path))
        {
            out += path;
        }
    }

#endif // DJV_OSX
    
    return out;
}

