// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the magnify widget.
        class MagnifyWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(MagnifyWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MagnifyWidget();

        public:
            ~MagnifyWidget() override;

            static std::shared_ptr<MagnifyWidget> create(const std::shared_ptr<System::Context>&);

            void setCurrentTool(bool);

            size_t getMagnify() const;

            void setMagnify(size_t);

            const glm::vec2& getMagnifyPos() const;
            
            void setMagnifyPos(const glm::vec2&);

        protected:
            void _initEvent(System::Event::Init &) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

