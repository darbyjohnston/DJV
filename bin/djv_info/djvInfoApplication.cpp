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

#include <djvInfoApplication.h>

#include <djvInfoContext.h>

#include <djvDebugLog.h>
#include <djvFileInfoUtil.h>
#include <djvImageIO.h>
#include <djvTime.h>
#include <djvVectorUtil.h>

#include <QDir>
#include <QTimer>

//------------------------------------------------------------------------------
// djvInfoApplication
//------------------------------------------------------------------------------

djvInfoApplication::djvInfoApplication(int argc, char ** argv) :
    QGuiApplication(argc, argv),
    _context(0)
{
    //DJV_DEBUG("djvInfoApplication::djvInfoApplication");
    
    setOrganizationName("djv.sourceforge.net");
    setApplicationName("djv_info");
    
    // Create the context.
    _context = new djvInfoContext;

    // Parse the command line.
    if (! _context->commandLine(argc, argv))
    {
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

djvInfoApplication::~djvInfoApplication()
{
    //DJV_DEBUG("djvInfoApplication::~djvInfoApplication");
    delete _context;
}

void djvInfoApplication::commandLineExit()
{
    exit(1);
}

void djvInfoApplication::work()
{
    //DJV_DEBUG("djvInfoApplication::work");
    
    int r = 0;

    // Convert the command line inputs:
    //
    // * Match wildcards
    // * Expand sequences
    djvFileInfoList list;
    const QStringList & input = _context->input();
    for (int i = 0; i < input.count(); ++i)
    {
        // Parse the input.
        djvFileInfo fileInfo = djvFileInfoUtil::parse(input[i], _context->sequence());
        //DJV_DEBUG_PRINT("input = " << fileInfo);
        DJV_LOG(_context->debugLog(), "djv_info", QString("Input = \"%1\"").arg(fileInfo));

        // Expand the sequence.
        QStringList tmp = djvFileInfoUtil::expandSequence(fileInfo);
        for (int j = 0; j < tmp.count(); ++j)
        {
            fileInfo = djvFileInfo(tmp[j]);
            if (_context->sequence() && fileInfo.isSequenceValid())
            {
                fileInfo.setType(djvFileInfo::SEQUENCE);
            }
            list += fileInfo;
        }
    }
    //DJV_DEBUG_PRINT("list = " << list);
    DJV_LOG(_context->debugLog(), "djv_info",
        QString("Input count = %1").arg(list.count()));

    // Compress files into sequences.
    djvFileInfoUtil::compressSequence(list, _context->sequence());
    //DJV_DEBUG_PRINT("list = " << list);
    DJV_LOG(_context->debugLog(), "djv_info",
        QString("Processed count = %1").arg(list.count()));

    // If there are no inputs use the current directory.
    if (! list.count() && 0 == r)
    {
        list += djvFileInfo(".");
    }

    // Print the files.
    for (int i = 0; i < list.count(); ++i)
    {
        if (djvFileInfo::DIRECTORY == list[i].type())
        {
            printDirectory(
                list[i],
                ((list.count() > 1) || _context->hasRecurse()) && ! _context->hasFilePath());
        }
        else
        {
            try
            {
                printItem(list[i], _context->hasFilePath(), _context->hasInfo());
            }
            catch (const djvError & error)
            {
                _context->printError(error);
                
                r = 1;
            }
        }
    }
    
    exit(r);
}

void djvInfoApplication::printItem(const djvFileInfo & in, bool path, bool info)
    throw (djvError)
{
    //DJV_DEBUG("djvInfoApplication::printItem");
    //DJV_DEBUG_PRINT("in = " << in);

    // Open the file.
    djvImageIOInfo _info;
    QScopedPointer<djvImageLoad> load;
    try
    {
        load.reset(_context->imageIOFactory()->load(in, _info));
    }
    catch (djvError error)
    {
        error.add(
            qApp->translate("djvInfoApplication", "Cannot open image: \"%1\"").
            arg(QDir::toNativeSeparators(in)));
        throw error;    
    }
    
    // Print the file.
    const QString name = in.fileName(-1, path);
    const bool verbose = _context->hasVerbose();
    const int columns = _context->columns();
    if (info && ! verbose)
    {
        if (1 == _info.layerCount())
        {
            // Print single layer information.
            const QString str = qApp->translate("djvInfoApplication",
                "%1x%2:%3 %4 %5@%6").
                arg(_info[0].size.x).
                arg(_info[0].size.y).
                arg(djvVectorUtil::aspect(_info[0].size), 0, 'f', 2).
                arg(djvStringUtil::label(_info[0].pixel).join(", ")).
                arg(djvTime::frameToString(
                    _info.sequence.frames.count(),
                    _info.sequence.speed)).
                arg(djvSpeed::speedToFloat(_info.sequence.speed), 0, 'f', 2);
            _context->print(qApp->translate("djvInfoApplication", "%1 %2").
                arg(QDir::toNativeSeparators(name)).
                arg(str, columns - name.length() - 2));
        }
        else
        {
            // Print time information.
            const QString str = qApp->translate("djvInfoApplication", "%1@%2").
                arg(djvTime::frameToString(
                    _info.sequence.frames.count(),
                    _info.sequence.speed)).
                arg(djvSpeed::speedToFloat(_info.sequence.speed), 0, 'f', 2);
            _context->print(qApp->translate("djvInfoApplication", "%1 %2").
                arg(QDir::toNativeSeparators(name)).
                arg(str, columns - name.length() - 2));

            // Print each layer's information.
            for (int i = 0; i < _info.layerCount(); ++i)
            {
                const QString nameString = qApp->translate("djvInfoApplication",
                    "    %1. %2").
                    arg(i).
                    arg(_info[i].layerName);
                const QString infoString = qApp->translate("djvInfoApplication",
                    "%1x%2:%3 %4").
                    arg(_info[i].size.x).
                    arg(_info[i].size.y).
                    arg(djvVectorUtil::aspect(_info[i].size), 0, 'f', 2).
                    arg(djvStringUtil::label(_info[i].pixel).join(", "));
                _context->print(qApp->translate("djvInfoApplication", "%1 %2").
                    arg(nameString).
                    arg(infoString, columns - nameString.length() - 2));
            }
        }
    }
    else if (info && verbose)
    {
        // Print verbose informaton.
        _context->print(QDir::toNativeSeparators(name));
        for (int i = 0; i < _info.layerCount(); ++i)
        {
            _context->print(qApp->translate("djvInfoApplication", "Layer = %1").
                arg(_info[i].layerName));
            _context->print(qApp->translate("djvInfoApplication", "  Width = %1").
                arg(_info[i].size.x));
            _context->print(qApp->translate("djvInfoApplication", "  Height = %1").
                arg(_info[i].size.y));
            _context->print(qApp->translate("djvInfoApplication", "  Aspect = %1").
                arg(djvVectorUtil::aspect(_info[i].size), 0, 'f', 2));
            _context->print(qApp->translate("djvInfoApplication", "  Pixel = %1").
                arg(djvStringUtil::label(_info[i].pixel).join(", ")));
        }
        _context->print(qApp->translate("djvInfoApplication", "Start = %1").
            arg(djvTime::frameToString(
                _info.sequence.start(),
                _info.sequence.speed)));
        _context->print(qApp->translate("djvInfoApplication", "End = %1").
            arg(djvTime::frameToString(
                _info.sequence.end(),
                _info.sequence.speed)));
        _context->print(qApp->translate("djvInfoApplication", "Duration = %1").
            arg(djvTime::frameToString(
                _info.sequence.frames.count(),
                _info.sequence.speed)));
        _context->print(qApp->translate("djvInfoApplication", "Speed = %1").
            arg(djvSpeed::speedToFloat(_info.sequence.speed)));
        const QStringList keys = _info.tags.keys();
        for (int i = 0; i < keys.count(); ++i)
        {
            _context->print(qApp->translate("djvInfoApplication", "Tag %1 = %2").
                arg(keys[i]).
                arg(_info.tags[keys[i]]));
        }
        _context->printSeparator();
    }
    else
    {
        _context->print(name);
    }
}

void djvInfoApplication::printDirectory(const djvFileInfo & in, bool label)
{
    //DJV_DEBUG("djvInfoApplication::printDirectory");
    //DJV_DEBUG_PRINT("in = " << in);

    // Read the directory contents.
    djvFileInfoList items;
    items = djvFileInfoUtil::list(in, _context->sequence());

    // Process the items.
    djvFileInfoUtil::filter(items, djvFileInfoUtil::FILTER_DIRECTORIES);
    //djvFileInfoUtil::compressSequence(items, _sequence);

    // Print the items.
    if (label)
    {
        _context->print(qApp->translate("djvInfoApplication", "%1:").
            arg(QDir::toNativeSeparators(in)));
        if (_context->hasVerbose())
        {
            _context->printSeparator();
        }
    }
    for (int i = 0; i < items.count(); ++i)
    {
        try
        {
            printItem(items[i], _context->hasFilePath(), _context->hasInfo());
        }
        catch (const djvError &)
        {}
    }
    if (label)
    {
        _context->printSeparator();
    }

    // Recurse.
    if (_context->hasRecurse())
    {
        djvFileInfoList list = djvFileInfoUtil::list(in, _context->sequence());
        djvFileInfoUtil::filter(
            list,
            djvFileInfoUtil::FILTER_FILES | djvFileInfoUtil::FILTER_HIDDEN);
        for (int i = 0; i < list.count(); ++i)
        {
            printDirectory(list[i], label);
        }
    }
}
