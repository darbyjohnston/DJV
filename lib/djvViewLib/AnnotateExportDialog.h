//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewLib/ViewLib.h>

#include <djvCore/Util.h>

#include <QDialog>
#include <QProcess>

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        //! This class provides a spinner widget.
        class Spinner : public QWidget
        {
            Q_OBJECT

        public:
            explicit Spinner(QWidget* parent = nullptr);
            ~Spinner() override;
            
            QSize sizeHint() const override;

        public Q_SLOTS:
            void start();
            void stop();

        protected:
            void paintEvent(QPaintEvent*) override;
            void timerEvent(QTimerEvent*) override;

        private:
            DJV_PRIVATE_COPY(Spinner);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides the annotations export dialog.
        class AnnotateExportDialog : public QDialog
        {
            Q_OBJECT

        public:
            explicit AnnotateExportDialog(QProcess*, QWidget * parent = nullptr);
            ~AnnotateExportDialog() override;

        private Q_SLOTS:
            void stateCallback(QProcess::ProcessState);
            void standardOutputCallback();
            void standardErrorCallback();
            void scriptErrorCallback(QProcess::ProcessError);

        private:
            void widgetUpdate();

            DJV_PRIVATE_COPY(AnnotateExportDialog);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
