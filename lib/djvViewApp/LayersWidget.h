// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the layers widget.
        class LayersWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(LayersWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            LayersWidget();

        public:
            ~LayersWidget() override;

            static std::shared_ptr<LayersWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            void _layersUpdate();
            void _currentLayerUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

