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

#include <djvViewLib/AbstractGroup.h>
#include <djvViewLib/Enum.h>

#include <djvCore/Vector.h>

#include <memory>

class QAction;

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the view group. The view group encapsulates all
        //! of the functionality relating to views such as panning and zooming the
        //! image, which image chaneels are displayed, etc.
        class ViewGroup : public AbstractGroup
        {
            Q_OBJECT

        public:
            ViewGroup(
                const QPointer<ViewGroup> & copy,
                const QPointer<Session> &,
                const QPointer<ViewContext> &);
            ~ViewGroup() override;

            djv::ViewLib::Enum::GRID grid() const;
            bool isHudEnabled() const;

            QPointer<QMenu> createMenu() const override;
            QPointer<QToolBar> createToolBar() const override;

        Q_SIGNALS:
            void gridChanged(djv::ViewLib::Enum::GRID);
            void hudEnabledChanged(bool);

        private Q_SLOTS:
            void leftCallback();
            void rightCallback();
            void upCallback();
            void downCallback();
            void centerCallback();
            void resetCallback();
            void zoomCallback(float);
            void zoomInCallback();
            void zoomOutCallback();
            void zoomResetCallback();
            void zoomIncCallback();
            void zoomDecCallback();
            void fitCallback();
            void gridCallback(djv::ViewLib::Enum::GRID);
            void gridCallback(QAction *);
            void hudEnabledCallback(bool);

            void update();

        private:
            DJV_PRIVATE_COPY(ViewGroup);

            void viewMove(const glm::ivec2 &);
            void viewZoom(float);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
