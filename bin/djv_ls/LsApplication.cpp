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

#include <djv_ls/LsApplication.h>

#include <djv_ls/LsContext.h>

#include <djvCore/DebugLog.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/Time.h>

#include <QDir>
#include <QTimer>

namespace djv
{
    namespace ls
    {
        Application::Application(int & argc, char ** argv) :
            QGuiApplication(argc, argv),
            _context(0)
        {
            //DJV_DEBUG("Application::Application");

            setOrganizationName("djv.sourceforge.net");
            setApplicationName("djv_ls");

            // Create the context.
            _context = new Context(argc, argv);

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
            //DJV_DEBUG("djvLsApplication::work");

            int r = 0;

            // Convert the command line inputs:
            //
            // * Match wildcards
            // * Expand sequences
            Core::FileInfoList list;
            Q_FOREACH(const QString & input, _context->input())
            {
                // Parse the input.
                Core::FileInfo fileInfo = Core::FileInfoUtil::parse(input, _context->sequence());
                //DJV_DEBUG_PRINT("input = " << fileInfo);
                DJV_LOG(_context->debugLog(), "djv_ls",
                    QString("Input = \"%1\"").arg(fileInfo));

                // Expand the sequence.
                Q_FOREACH(const QString & fileName, Core::FileInfoUtil::expandSequence(fileInfo))
                {
                    fileInfo = Core::FileInfo(fileName, false);
                    if (!fileInfo.stat())
                    {
                        _context->printError(
                            qApp->translate("djv::ls::Application", "Cannot open: \"%1\"").
                            arg(QDir::toNativeSeparators(fileInfo)));
                        r = 1;
                        continue;
                    }
                    if (_context->sequence() && fileInfo.isSequenceValid())
                    {
                        fileInfo.setType(Core::FileInfo::SEQUENCE);
                    }
                    list += fileInfo;
                }
            }
            //DJV_DEBUG_PRINT("list = " << list);
            DJV_LOG(_context->debugLog(), "djv_ls",
                QString("Input count = %1").arg(list.count()));

            // Process the inputs.
            process(list);
            //DJV_DEBUG_PRINT("processed = " << list);
            DJV_LOG(_context->debugLog(), "djv_ls",
                QString("Processed count = %1").arg(list.count()));

            // If there are no inputs list the current directory.
            if (!list.count() && 0 == r)
            {
                list += Core::FileInfo(".");
            }

            // Print the items.
            for (int i = 0; i < list.count(); ++i)
            {
                if (Core::FileInfo::DIRECTORY == list[i].type())
                {
                    if (!printDirectory(
                        list[i],
                        ((list.count() > 1) || _context->hasRecurse()) &&
                        !_context->hasFilePath()))
                    {
                        _context->printError(
                            qApp->translate("djv::ls::Application", "Cannot open: \"%1\"").
                            arg(QDir::toNativeSeparators(list[i])));
                        r = 1;
                    }
                }
                else
                {
                    printItem(list[i], true, _context->hasFileInfo());
                }
            }

            exit(r);
        }

        void Application::process(Core::FileInfoList & in)
        {
            //DJV_DEBUG("Application::process");
            //DJV_DEBUG_PRINT("in = " << in);

            // Compress files into sequences.
            Core::FileInfoUtil::compressSequence(in, _context->sequence());
            //DJV_DEBUG_PRINT("compress = " << in);

            // Remove hidden files.
            if (!_context->hasHidden())
            {
                Core::FileInfoUtil::filter(in, Core::FileInfoUtil::FILTER_HIDDEN);
                //DJV_DEBUG_PRINT("hidden = " << in);
            }

            // Sort.
            Core::FileInfoUtil::sort(in, _context->sort(), _context->hasReverseSort());
            //DJV_DEBUG_PRINT("sort = " << in);

            if (_context->hasSortDirsFirst())
            {
                Core::FileInfoUtil::sortDirsFirst(in);
                //DJV_DEBUG_PRINT("sort directories = " << in);
            }
        }

        void Application::printItem(const Core::FileInfo & in, bool path, bool info)
        {
            //DJV_DEBUG("Application::printItem");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("path = " << path);
            //DJV_DEBUG_PRINT("info = " << info);
            //DJV_DEBUG_PRINT("columns = " << _columns);

            QString name = in.fileName(-1, path);

            if (info)
            {
                const QString infoString =
#if defined(DJV_WINDOWS)
                    qApp->translate("djv::ls::Application", "%1 %2 %3 %4").
#else
                    qApp->translate("djv::ls::Application", "%1 %2 %3 %4 %5").
#endif
                    arg(Core::FileInfo::typeLabels()[in.type()], 4).
                    arg(Core::Memory::sizeLabel(in.size())).
#if ! defined(DJV_WINDOWS)
                    arg(Core::User::uidToString(in.user())).
#endif // DJV_WINDOWS
                    arg(Core::FileInfo::permissionsLabel(in.permissions()), 3).
                    arg(Core::Time::timeToString(in.time()));

                // Elide the name if there isn't enough space.
                if (_context->columns())
                {
                    const int length = Core::Math::max(
                        _context->columns() - infoString.length() - 2,
                        0);
                    if (name.length() > length)
                    {
                        name = name.mid(0, length);

                        int i = name.length();
                        int j = 3;

                        while (i-- > 0 && j-- > 0)
                        {
                            name[i] = '.';
                        }
                    }
                }
                _context->print(
                    qApp->translate("djv::ls::Application", "%1 %2").
                    arg(QDir::toNativeSeparators(name)).
                    arg(infoString, _context->columns() - name.length() - 2));
            }
            else
            {
                _context->print(name);
            }
        }

        bool Application::printDirectory(const Core::FileInfo & in, bool label)
        {
            //DJV_DEBUG("Application::printDirectory");
            //DJV_DEBUG_PRINT("in = " << in);

            // Read the directory contents.
            if (!QDir(in.path()).exists())
                return false;
            Core::FileInfoList items = Core::FileInfoUtil::list(in, _context->sequence());

            // Process the items.
            process(items);

            // Print the items.
            if (label)
            {
                _context->print(qApp->translate("djv::ls::Application", "%1:").
                    arg(QDir::toNativeSeparators(in)));
            }
            for (int i = 0; i < items.count(); ++i)
            {
                printItem(items[i], _context->hasFilePath(), _context->hasFileInfo());
            }
            if (label)
            {
                _context->printSeparator();
            }

            // Recurse.
            bool r = true;
            if (_context->hasRecurse())
            {
                Core::FileInfoList items = Core::FileInfoUtil::list(in, _context->sequence());
                Core::FileInfoUtil::filter(
                    items,
                    Core::FileInfoUtil::FILTER_FILES |
                    (!_context->hasHidden() ? Core::FileInfoUtil::FILTER_HIDDEN : 0));
                for (int i = 0; i < items.count(); ++i)
                {
                    r &= printDirectory(items[i], label);
                }
            }

            return r;
        }

    } // namespace ls
} // namespace djv
