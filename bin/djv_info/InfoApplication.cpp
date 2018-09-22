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

#include <djv_info/InfoApplication.h>

#include <djv_info/InfoContext.h>

#include <djvGraphics/ImageIO.h>

#include <djvCore/DebugLog.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/Time.h>
#include <djvCore/VectorUtil.h>

#include <QDir>
#include <QTimer>

namespace djv
{
    namespace info
    {
        Application::Application(int argc, char ** argv) :
            QGuiApplication(argc, argv),
            _context(0)
        {
            //DJV_DEBUG("Application::Application");

            setOrganizationName("djv.sourceforge.net");
            setApplicationName("djv_info");

            // Create the context.
            _context = new Context;

            // Parse the command line.
            if (!_context->commandLine(argc, argv))
            {
                QTimer::singleShot(0, this, SLOT(commandLineExit()));
            }
            else
            {
                QTimer::singleShot(0, this, SLOT(work()));
            }
        }

        Application::~Application()
        {
            //DJV_DEBUG("Application::~Application");
            delete _context;
        }

        void Application::commandLineExit()
        {
            exit(1);
        }

        void Application::work()
        {
            //DJV_DEBUG("Application::work");

            int r = 0;

            // Convert the command line inputs:
            //
            // * Match wildcards
            // * Expand sequences
            Core::FileInfoList list;
            const QStringList & input = _context->input();
            for (int i = 0; i < input.count(); ++i)
            {
                // Parse the input.
                Core::FileInfo fileInfo = Core::FileInfoUtil::parse(input[i], _context->sequence());
                //DJV_DEBUG_PRINT("input = " << fileInfo);
                DJV_LOG(_context->debugLog(), "djv_info", QString("Input = \"%1\"").arg(fileInfo));

                // Expand the sequence.
                QStringList tmp = Core::FileInfoUtil::expandSequence(fileInfo);
                for (int j = 0; j < tmp.count(); ++j)
                {
                    fileInfo = Core::FileInfo(tmp[j]);
                    if (_context->sequence() && fileInfo.isSequenceValid())
                    {
                        fileInfo.setType(Core::FileInfo::SEQUENCE);
                    }
                    list += fileInfo;
                }
            }
            //DJV_DEBUG_PRINT("list = " << list);
            DJV_LOG(_context->debugLog(), "djv_info",
                QString("Input count = %1").arg(list.count()));

            // Compress files into sequences.
            Core::FileInfoUtil::compressSequence(list, _context->sequence());
            //DJV_DEBUG_PRINT("list = " << list);
            DJV_LOG(_context->debugLog(), "djv_info",
                QString("Processed count = %1").arg(list.count()));

            // If there are no inputs use the current directory.
            if (!list.count() && 0 == r)
            {
                list += Core::FileInfo(".");
            }

            // Print the files.
            for (int i = 0; i < list.count(); ++i)
            {
                if (Core::FileInfo::DIRECTORY == list[i].type())
                {
                    printDirectory(
                        list[i],
                        ((list.count() > 1) || _context->hasRecurse()) && !_context->hasFilePath());
                }
                else
                {
                    try
                    {
                        printItem(list[i], _context->hasFilePath(), _context->hasInfo());
                    }
                    catch (const Core::Error & error)
                    {
                        _context->printError(error);
                        r = 1;
                    }
                }
            }

            exit(r);
        }

        void Application::printItem(const Core::FileInfo & in, bool path, bool info)
        {
            //DJV_DEBUG("Application::printItem");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            Graphics::ImageIOInfo _info;
            QScopedPointer<Graphics::ImageLoad> load;
            try
            {
                load.reset(_context->imageIOFactory()->load(in, _info));
            }
            catch (Core::Error error)
            {
                error.add(
                    qApp->translate("djv::info::Application", "Cannot open image: \"%1\"").
                    arg(QDir::toNativeSeparators(in)));
                throw error;
            }

            // Print the file.
            const QString name = in.fileName(-1, path);
            const bool verbose = _context->hasVerbose();
            const int columns = _context->columns();
            if (info && !verbose)
            {
                if (1 == _info.layerCount())
                {
                    // Print single layer information.
                    QStringList pixelLabel;
                    pixelLabel << _info[0].pixel;
                    const QString str = qApp->translate("djv::info::Application",
                        "%1x%2:%3 %4 %5@%6").
                        arg(_info[0].size.x).
                        arg(_info[0].size.y).
                        arg(Core::VectorUtil::aspect(_info[0].size), 0, 'f', 2).
                        arg(pixelLabel.join(", ")).
                        arg(Core::Time::frameToString(
                            _info.sequence.frames.count(),
                            _info.sequence.speed)).
                        arg(Core::Speed::speedToFloat(_info.sequence.speed), 0, 'f', 2);
                    _context->print(qApp->translate("djv::info::Application", "%1 %2").
                        arg(QDir::toNativeSeparators(name)).
                        arg(str, columns - name.length() - 2));
                }
                else
                {
                    // Print time information.
                    const QString str = qApp->translate("djv::info::Application", "%1@%2").
                        arg(Core::Time::frameToString(
                            _info.sequence.frames.count(),
                            _info.sequence.speed)).
                        arg(Core::Speed::speedToFloat(_info.sequence.speed), 0, 'f', 2);
                    _context->print(qApp->translate("djv::info::Application", "%1 %2").
                        arg(QDir::toNativeSeparators(name)).
                        arg(str, columns - name.length() - 2));

                    // Print each layer's information.
                    for (int i = 0; i < _info.layerCount(); ++i)
                    {
                        const QString nameString = qApp->translate("djv::info::Application",
                            "    %1. %2").
                            arg(i).
                            arg(_info[i].layerName);
                        QStringList pixelLabel;
                        pixelLabel << _info[i].pixel;
                        const QString infoString = qApp->translate("djv::info::Application",
                            "%1x%2:%3 %4").
                            arg(_info[i].size.x).
                            arg(_info[i].size.y).
                            arg(Core::VectorUtil::aspect(_info[i].size), 0, 'f', 2).
                            arg(pixelLabel.join(", "));
                        _context->print(qApp->translate("djv::info::Application", "%1 %2").
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
                    _context->print(qApp->translate("djv::info::Application", "Layer = %1").
                        arg(_info[i].layerName));
                    _context->print(qApp->translate("djv::info::Application", "  Width = %1").
                        arg(_info[i].size.x));
                    _context->print(qApp->translate("djv::info::Application", "  Height = %1").
                        arg(_info[i].size.y));
                    _context->print(qApp->translate("djv::info::Application", "  Aspect = %1").
                        arg(Core::VectorUtil::aspect(_info[i].size), 0, 'f', 2));
                    QStringList pixelLabel;
                    pixelLabel << _info[i].pixel;
                    _context->print(qApp->translate("djv::info::Application", "  Pixel = %1").
                        arg(pixelLabel.join(", ")));
                }
                _context->print(qApp->translate("djv::info::Application", "Start = %1").
                    arg(Core::Time::frameToString(
                        _info.sequence.start(),
                        _info.sequence.speed)));
                _context->print(qApp->translate("djv::info::Application", "End = %1").
                    arg(Core::Time::frameToString(
                        _info.sequence.end(),
                        _info.sequence.speed)));
                _context->print(qApp->translate("djv::info::Application", "Duration = %1").
                    arg(Core::Time::frameToString(
                        _info.sequence.frames.count(),
                        _info.sequence.speed)));
                _context->print(qApp->translate("djv::info::Application", "Speed = %1").
                    arg(Core::Speed::speedToFloat(_info.sequence.speed)));
                const QStringList keys = _info.tags.keys();
                for (int i = 0; i < keys.count(); ++i)
                {
                    _context->print(qApp->translate("djv::info::Application", "Tag %1 = %2").
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

        void Application::printDirectory(const Core::FileInfo & in, bool label)
        {
            //DJV_DEBUG("Application::printDirectory");
            //DJV_DEBUG_PRINT("in = " << in);

            // Read the directory contents.
            Core::FileInfoList items;
            items = Core::FileInfoUtil::list(in, _context->sequence());

            // Process the items.
            Core::FileInfoUtil::filter(items, Core::FileInfoUtil::FILTER_DIRECTORIES);
            //Core::FileInfoUtil::compressSequence(items, _sequence);

            // Print the items.
            if (label)
            {
                _context->print(qApp->translate("djv::info::Application", "%1:").
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
                catch (const Core::Error &)
                {
                }
            }
            if (label)
            {
                _context->printSeparator();
            }

            // Recurse.
            if (_context->hasRecurse())
            {
                Core::FileInfoList list = Core::FileInfoUtil::list(in, _context->sequence());
                Core::FileInfoUtil::filter(
                    list,
                    Core::FileInfoUtil::FILTER_FILES | Core::FileInfoUtil::FILTER_HIDDEN);
                for (int i = 0; i < list.count(); ++i)
                {
                    printDirectory(list[i], label);
                }
            }
        }

    } // namespace info
} // namespace djv
