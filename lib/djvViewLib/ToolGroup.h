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

#include <djvViewLib/AbstractGroup.h>
#include <djvViewLib/Enum.h>

#include <QMap>

#include <memory>

class QAction;
class QDockWidget;

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the tool group. The tool group encapsulates all of the
        //! functionality relating to tools such as the color picker, magnifier, and
        //! histogram display.
        class ToolGroup : public AbstractGroup
        {
            Q_OBJECT

        public:
            ToolGroup(
                const QPointer<ToolGroup> & copy,
                const QPointer<Session> &,
                const QPointer<ViewContext> &);
            ~ToolGroup();

            //! Get the tool visibility.
            const QMap<Enum::TOOL, bool> & toolsVisible() const;

            //! Get the dock widgets.
            QVector<QPointer<QDockWidget> > dockWidgets() const;

            QPointer<QMenu> createMenu() const override;
            QPointer<QToolBar> createToolBar() const override;

        public Q_SLOTS:
            //! Set the tool visibility.
            void setToolsVisible(const QMap<djv::ViewLib::Enum::TOOL, bool> &);

        Q_SIGNALS:
            //! This signal is emitted when the tool visibility is changed.
            void toolsVisibleChanged(const QMap<djv::ViewLib::Enum::TOOL, bool> &);

        private Q_SLOTS:
            void toolsVisibleCallback();

            void update();

        private:
            DJV_PRIVATE_COPY(ToolGroup);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
