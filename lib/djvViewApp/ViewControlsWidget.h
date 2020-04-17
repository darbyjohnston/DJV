// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;
    
        } // namespace Image
    } // namespace AV

    namespace ViewApp
    {
        class GridOptions;

        //! This class provides the view controls widget.
        class ViewControlsWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(ViewControlsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ViewControlsWidget();

        public:
            ~ViewControlsWidget() override;

            static std::shared_ptr<ViewControlsWidget> create(const std::shared_ptr<Core::Context>&);

            int getCurrentTab() const;
            void setCurrentTab(int);            

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            void _setPos(const glm::vec2&);
            void _setZoom(float);
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

