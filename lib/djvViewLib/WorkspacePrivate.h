//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/Enum.h>

#include <QMdiSubWindow>
#include <QPointer>
#include <QWidget>

namespace djv
{
    namespace ViewLib
    {
        class Context;
        class Media;
        class Workspace;

        class WorkspaceMDISubWindow : public QMdiSubWindow
        {
            Q_OBJECT

        public:
            WorkspaceMDISubWindow(const QPointer<Workspace> &, const std::shared_ptr<Media> &, const std::shared_ptr<Context> &);
            ~WorkspaceMDISubWindow() override;
            
            virtual QSize sizeHint() const override;

        private:
            DJV_PRIVATE();
        };

        class WorkspaceMDI : public QWidget
        {
            Q_OBJECT

        public:
            WorkspaceMDI(const QPointer<Workspace> &, const std::shared_ptr<Context> &, QWidget * parent = nullptr);
            ~WorkspaceMDI() override;

        protected:
            bool eventFilter(QObject *, QEvent *) override;

        private Q_SLOTS:
            void _addMedia(const std::shared_ptr<Media> &);
            void _removeMedia(const std::shared_ptr<Media> &);

        private:
            DJV_PRIVATE();
        };

        class WorkspaceTabs : public QWidget
        {
            Q_OBJECT

        public:
            WorkspaceTabs(const std::shared_ptr<Context> &, QWidget * parent = nullptr);
            ~WorkspaceTabs() override;

        private Q_SLOTS:
            void _addWorkspace(const QPointer<Workspace> &);
            void _removeWorkspace(const QPointer<Workspace> &);

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv

