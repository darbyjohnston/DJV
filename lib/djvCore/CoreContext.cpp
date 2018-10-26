//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvCore/CoreContext.h>

#include <djvCore/Assert.h>
#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/ErrorUtil.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/Sequence.h>
#include <djvCore/System.h>
#include <djvCore/Time.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLocale>
#include <QScopedPointer>
#include <QTranslator>

namespace djv
{
    namespace Core
    {
        struct CoreContext::Private
        {
            Private() :
                debugLog(new DebugLog)
            {}

            bool endline = false;
            bool separator = false;
            QScopedPointer<DebugLog> debugLog;
        };
        
        CoreContext::CoreContext(int & argc, char ** argv, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("CoreContext::CoreContext");
            
            qRegisterMetaType<FileInfo>("djv::Core::FileInfo");
            qRegisterMetaType<FileInfoList>("djv::Core::FileInfoList");
            qRegisterMetaType<Sequence>("djv::Core::Sequence");
            qRegisterMetaType<Sequence::FORMAT>("djv::Core::Sequence::FORMAT");

            DJV_LOG(debugLog(), "djv::Core::CoreContext",
                QString("Search paths: %1").arg(StringUtil::addQuotes(System::searchPath()).join(", ")));                
            QTranslator * qtTranslator = new QTranslator(this);
            qtTranslator->load("qt_" + QLocale::system().name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath));
            qApp->installTranslator(qtTranslator);
            loadTranslator("djvCore");
        }

        CoreContext::~CoreContext()
        {
            //DJV_DEBUG("CoreContext::~CoreContext");    
        }

        namespace
        {
            QString applicationPath(int & argc, char ** argv)
            {
                QFileInfo applicationPath(argv[0]);
                QDir applicationDir = applicationPath.absoluteDir();
                while (applicationDir.exists() && !applicationDir.isRoot())
                {
                    if (applicationDir.exists("lib"))
                        break;
                    applicationDir.cdUp();
                }
                return applicationDir.absolutePath();
            }
        
        } // namespace

        void CoreContext::initLibPaths(int & argc, char ** argv)
        {
            //DJV_DEBUG("CoreContexT::initLibPaths");
            QDir dir(applicationPath(argc, argv));
            //DJV_DEBUG_PRINT("dir = " << dir.absolutePath());
            if (dir.cd("plugins"))
            {
                //DJV_DEBUG_PRINT("dir = " << dir.absolutePath());
                QCoreApplication::addLibraryPath(dir.absolutePath());
            }
        }

        bool CoreContext::commandLine(int & argc, char ** argv)
        {
            QStringList args;
            for (int i = 1; i < argc; ++i)
            {
                args += argv[i];
            }
            DJV_LOG(debugLog(), "djv::Core::CoreContext",
                QString("Command line: %1").arg(StringUtil::addQuotes(args).join(", ")));
            DJV_LOG(debugLog(), "djv::Core::CoreContext", "");

            try
            {
                if (!commandLineParse(args))
                {
                    return false;
                }
            }
            catch (const QString & error)
            {
                printError(
                    qApp->translate("djv::Core::CoreContext",
                        "Cannot parse the command line argument: %1").arg(error));

                return false;
            }

            DJV_LOG(debugLog(), "djv::Core::CoreContext", "Information:");
            DJV_LOG(debugLog(), "djv::Core::CoreContext", "");
            DJV_LOG(debugLog(), "djv::Core::CoreContext", info());
            return true;
        }

        QString CoreContext::documentationPath() const
        {
            return FileInfoUtil::fixPath(qApp->applicationDirPath() + "/../doc/");
        }

        QString CoreContext::info() const
        {
            QStringList seqFormatLabel;
            seqFormatLabel << Sequence::format();
            QStringList seqAutoEnabledLabel;
            seqAutoEnabledLabel << Sequence::isAutoEnabled();
            QStringList seqMaxSizeLabel;
            seqMaxSizeLabel << Sequence::maxSize();
            QStringList seqNegativeEnabledLabel;
            seqNegativeEnabledLabel << Sequence::isNegativeEnabled();
            QStringList timeLabel;
            timeLabel << Time::units();
            QStringList speedLabel;
            speedLabel << Speed::speed();
            QStringList endianLabel;
            endianLabel << Memory::endian();
            static const QString label = qApp->translate(
                "djv::Core::CoreContext",
                "General\n"
                "\n"
                "    Version: %1\n"
                "\n"
                "File Sequences\n"
                "\n"
                "    Formatting: %2\n"
                "    Auto sequencing: %3\n"
                "    Maximum size: %4\n"
                "    Negative numbers: %5\n"
                "\n"
                "Time\n"
                "\n"
                "    Units: %6\n"
                "    Default speed: %7\n"
                "\n"
                "System\n"
                "\n"
                "    %8\n"
                "    Endian: %9\n"
                "    Locale: %10\n"
                "    Search path: %11\n"
                "    Documentation path: %12\n"
                "    Qt version: %13\n");
            return QString(label).
                arg(DJV_VERSION).
                arg(seqFormatLabel.join(", ")).
                arg(seqAutoEnabledLabel.join(", ")).
                arg(seqMaxSizeLabel.join(", ")).
                arg(seqNegativeEnabledLabel.join(", ")).
                arg(timeLabel.join(", ")).
                arg(speedLabel.join(", ")).
                arg(System::info()).
                arg(endianLabel.join(", ")).
                arg(QLocale::system().name()).
                arg(StringUtil::addQuotes(System::searchPath()).join(", ")).
                arg(documentationPath()).
                arg(qVersion());
        }

        QString CoreContext::about() const
        {
            static const QString label = qApp->translate(
                "djv::Core::CoreContext",
                "DJV Imaging, Version: %1\n"
                "http://djv.sourceforge.net\n"
                "Made in Berkeley, CA, USA\n"
                "\n"
                "Copyright (c) 2004-2018 Darby Johnston\n"
                "All rights reserved.\n"
                "\n"
                "Redistribution and use in source and binary forms, with or without "
                "modification, are permitted provided that the following conditions are "
                "met:\n"
                "\n"
                "* Redistributions of source code must retain the above copyright notice, "
                "this list of conditions, and the following disclaimer.\n"
                "* Redistributions in binary form must reproduce the above copyright "
                "notice, this list of conditions, and the following disclaimer in the "
                "documentation and/or other materials provided with the distribution.\n"
                "* Neither the names of the copyright holders nor the names of any "
                "contributors may be used to endorse or promote products derived from this "
                "software without specific prior written permission.\n"
                "\n"
                "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
                "\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
                "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
                "PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER "
                "OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
                "EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
                "PROCUREMENT OF SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
                "PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
                "LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
                "NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
                "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
                "\n"
                "Portions of this software are copyright (in alphabetical order):\n"
                "\n"
                "    Copyright (c) 2004-2018 Darby Johnston\n"
                "    All rights reserved.\n"
                "\n"
                "    Copyright (c) 2008 Alan Jones\n"
                "    All rights reserved.\n"
                "\n"
                "    Copyright (c) 2008-2009 Mikael Sundell\n"
                "    All rights reserved.\n"
                "\n"
                "This software is based in part on the works of (in alphabetical order):\n"
                "\n"
                "    CMake\n"
                "    http://www.cmake.org\n"
                "    Copyright 2000-2011 Kitware, Inc., Insight Software Consortium\n"
                "    All rights reserved.\n"
                "\n"
                "    FFmpeg\n"
                "    http://www.ffmpeg.org\n"
                "    Licensed under the LGPLv2.1\n"
                "    http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html\n"
                "\n"
                "    GLM\n"
                "    https://glm.g-truc.net/0.9.9/index.html\n"
                "    Copyright (c) 2005-2017 G-Truc Creation\n"
                "\n"
                "    libjpeg\n"
                "    http://www.ijg.org\n"
                "    Copyright (c) 1991-2012, Thomas G. Lane, Guido Vollbeding\n"
                "\n"
                "    libpng\n"
                "    http://www.libpng.org\n"
                "    Copyright (c) 2004, 2006-2012 Glenn Randers-Pehrson\n"
                "\n"
                "    libtiff\n"
                "    http://www.libtiff.org\n"
                "    Copyright (c) 1988-1997 Sam Leffler\n"
                "    Copyright (c) 1991-1997 Silicon Graphics, Inc.\n"
                "\n"
                "    OpenAL\n"
                "    https://www.openal.org\n"
                "    Copyright (C) 1999-2000 by Loki Software\n"
                "\n"
                "    OpenEXR\n"
                "    http://www.openexr.com\n"
                "    Copyright (c) 2006, Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.\n"
                "\n"
                "    Qt\n"
                "    http://qt-project.org\n"
                "    Copyright (c) 2013 Digia Plc and/or its subsidiary(-ies)\n"
                "\n"
                "    zlib\n"
                "    http://www.zlib.net/\n"
                "    Copyright (c) 1995-2013 Jean-loup Gailly and Mark Adler\n");
            return QString(label).arg(DJV_VERSION);
        }

        void CoreContext::print(const QString & string, bool newLine, int indent)
        {
            consolePrint(string, newLine, indent);
        }

        void CoreContext::printSeparator()
        {
            const_cast<CoreContext *>(this)->_p->separator = true;
        }

        QPointer<DebugLog> CoreContext::debugLog() const
        {
            return _p->debugLog.data();
        }

        void CoreContext::printMessage(const QString & string)
        {
            print(string);
        }

        void CoreContext::printError(const Error & error)
        {
            const QStringList list = ErrorUtil::format(error);
            for (int i = list.count() - 1; i >= 0; --i)
            {
                print(list[i]);
            }
        }

        void CoreContext::loadTranslator(const QString & baseName)
        {
            const QString qmName = QString("%1_%2.qm").arg(baseName).arg(QLocale::system().name());
            DJV_LOG(debugLog(), "djv::Core::CoreContext", QString("Searching for translator: \"%1\"").arg(qmName));
            const QString fileName = System::findFile(qmName);
            if (!fileName.isEmpty())
            {
                DJV_LOG(debugLog(), "djv::Core::CoreContext", QString("Found translator: \"%1\"").arg(fileName));
                QTranslator * qTranslator = new QTranslator(qApp);
                qTranslator->load(fileName);
                qApp->installTranslator(qTranslator);
            }
        }

        bool CoreContext::commandLineParse(QStringList & in)
        {
            //DJV_DEBUG("CoreContext::commandLineParse");
            //DJV_DEBUG_PRINT("in = " << in);
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;

                    // General options.
                    if (qApp->translate("djv::Core::CoreContext", "-seq_format") == arg)
                    {
                        Sequence::FORMAT value = static_cast<Sequence::FORMAT>(0);
                        in >> value;
                        Sequence::setFormat(value);
                    }
                    else if (qApp->translate("djv::Core::CoreContext", "-seq_auto") == arg)
                    {
                        bool value = false;
                        in >> value;
                        Sequence::setAutoEnabled(value);
                    }
                    else if (qApp->translate("djv::Core::CoreContext", "-seq_max") == arg)
                    {
                        qint64 value = static_cast<qint64>(0);
                        in >> value;
                        Sequence::setMaxSize(value);
                    }
                    else if (qApp->translate("djv::Core::CoreContext", "-seq_negative") == arg)
                    {
                        bool value = false;
                        in >> value;
                        Sequence::setNegativeEnabled(value);
                    }
                    else if (qApp->translate("djv::Core::CoreContext", "-time_units") == arg)
                    {
                        Time::UNITS value = static_cast<Time::UNITS>(0);
                        in >> value;
                        Time::setUnits(value);
                    }
                    else if (qApp->translate("djv::Core::CoreContext", "-default_speed") == arg)
                    {
                        Speed::FPS value = static_cast<Speed::FPS>(0);
                        in >> value;
                        Speed::setSpeed(value);
                    }
                    else if (qApp->translate("djv::Core::CoreContext", "-debug_log") == arg)
                    {
                        Q_FOREACH(const QString & message, debugLog()->messages())
                        {
                            printMessage(message);
                        }
                        connect(
                            debugLog(),
                            SIGNAL(message(const QString &)),
                            SLOT(debugLogCallback(const QString &)));
                    }
                    else if (
                        qApp->translate("djv::Core::CoreContext", "-help") == arg ||
                        qApp->translate("djv::Core::CoreContext", "-h") == arg)
                    {
                        print("\n" + commandLineHelp());
                        return false;
                    }
                    else if (qApp->translate("djv::Core::CoreContext", "-info") == arg)
                    {
                        print("\n" + info());
                        return false;
                    }
                    else if (qApp->translate("djv::Core::CoreContext", "-about") == arg)
                    {
                        print("\n" + about());
                        return false;
                    }
#if defined(DJV_OSX)
                    else if (qApp->translate("djv::Core::CoreContext", "-psn_") == arg.mid(0, 5))
                    {
                        //! \todo Ignore the Mac OS process id command line argument.
                        //! Is this still necessary?
                    }
#endif
                    // Leftovers.
                    else
                    {
                        tmp << arg;
                    }
                }
            }
            catch (const QString &)
            {
                in = tmp;
                throw QString(arg);
            }
            in = tmp;
            return true;
        }

        QString djv::Core::CoreContext::commandLineHelp() const
        {
            QStringList seqFormatLabel;
            seqFormatLabel << Sequence::format();
            QStringList seqAutoEnabledLabel;
            seqAutoEnabledLabel << Sequence::isAutoEnabled();
            QStringList seqMaxSizeLabel;
            seqMaxSizeLabel << Sequence::maxSize();
            QStringList seqNegativeEnabledLabel;
            seqNegativeEnabledLabel << Sequence::isNegativeEnabled();
            QStringList timeUnitsLabel;
            timeUnitsLabel << Time::units();
            QStringList speedLabel;
            speedLabel << Speed::speed();
            static const QString label = qApp->translate(
                "djv::Core::CoreContext",
                "\n"
                "File Sequence Options\n"
                "\n"
                "    -seq_format (value)\n"
                "        Set the file sequence formatting: %1. Default = %2.\n"
                "    -seq_auto (value)\n"
                "        Set whether auto file sequencing is enabled: %3. Default = %4.\n"
                "    -seq_max (value)\n"
                "        Set the maximum allowed size of file sequences. Default = %5.\n"
                "    -seq_negative (value)\n"
                "        Set whether negative numbers are enabled: %6. Default = %7.\n"
                "\n"
                "Time Options\n"
                "\n"
                "    -time_units (value)\n"
                "        Set the time units: %8. Default = %9.\n"
                "    -default_speed (value)\n"
                "        Set the default speed: %10. Default = %11.\n"
                "\n"
                "Miscellaneous Options\n"
                "\n"
                "    -debug_log\n"
                "        Print debug log messages.\n"
                "    -help, -h\n"
                "        Show the command line documentation.\n"
                "    -info\n"
                "        Show information about the application.\n"
                "    -about\n"
                "        Show legal infomration.\n");
            return QString(label).
                arg(Sequence::formatLabels().join(", ")).
                arg(seqFormatLabel.join(", ")).
                arg(StringUtil::boolLabels().join(", ")).
                arg(seqAutoEnabledLabel.join(", ")).
                arg(seqMaxSizeLabel.join(", ")).
                arg(StringUtil::boolLabels().join(", ")).
                arg(seqNegativeEnabledLabel.join(", ")).
                arg(Time::unitsLabels().join(", ")).
                arg(timeUnitsLabel.join(", ")).
                arg(Speed::fpsLabels().join(", ")).
                arg(speedLabel.join(", "));
        }

        void CoreContext::consolePrint(const QString & string, bool newline, int indent)
        {
            if (_p->separator)
            {
                System::print("");
                const_cast<CoreContext *>(this)->_p->separator = false;
            }
            System::print(string, newline, indent);
            const_cast<CoreContext *>(this)->_p->endline = !newline;
        }

        void CoreContext::debugLogCallback(const QString & string)
        {
            consolePrint(string);
        }

    } // namespace Core
} // namespace djv
