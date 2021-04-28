// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! Layers widget.
        class LayersWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(LayersWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            LayersWidget();

        public:
            ~LayersWidget() override;

            static std::shared_ptr<LayersWidget> create(const std::shared_ptr<System::Context>&);

            void setFilter(const std::string&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            void _layersUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

