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

#pragma once

#include <djvCore/Error.h>
#include <djvCore/StringUtil.h>

#include <QObject>
#include <QPointer>
#include <QStringList>

#include <memory>

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

namespace djv
{
    namespace Core
    {
        class DebugLog;

        //! This class provides global functionality for the library.
        class CoreContext : public QObject
        {
            Q_OBJECT

        public:
            explicit CoreContext(int & argc, char ** argv, QObject * parent = nullptr);
            virtual ~CoreContext();
            
            // Initialize the library paths. This needs to be called before the
            // application is created.
            static void initLibPaths(int & argc, char ** argv);

            //! Parse the command line.
            virtual bool commandLine(int & argc, char ** argv);

            //! Get the documentation path.
            QString documentationPath() const;

            //! Get the application information.
            virtual QString info() const;

            //! Get the application about information.
            virtual QString about() const;

            //! Print a string.
            virtual void print(const QString &, bool newLine = true, int indent = 0);

            //! Print a separator.
            virtual void printSeparator();

            //! Get the debugging log.
            QPointer<DebugLog> debugLog() const;

        public Q_SLOTS:
            //! Print a message.
            void printMessage(const QString &);

            //! Print an error.
            void printError(const djv::Core::Error &);

        protected:
            void loadTranslator(const QString & baseName);

            //! Throws:
            //! - QString
            virtual bool commandLineParse(QStringList &);

            virtual QString commandLineHelp() const;

            void consolePrint(const QString &, bool newLine = true, int indent = 0);

        private Q_SLOTS:
            void debugLogCallback(const QString &);

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace Core
} // namespace djv
