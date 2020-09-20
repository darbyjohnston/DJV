// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides a MDI widget.
        class MDIWidget : public UI::MDI::IWidget
        {
            DJV_NON_COPYABLE(MDIWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MDIWidget();

        public:
            ~MDIWidget() override = 0;

            const std::string & getTitle() const;

            void setTitle(const std::string &);

            void close();
            void setCloseCallback(const std::function<void(void)> &);

            float getHeightForWidth(float) const override;

            void addChild(const std::shared_ptr<IObject> &) override;
            void removeChild(const std::shared_ptr<IObject> &) override;
            void clearChildren() override;

        protected:
            std::map<UI::MDI::Handle, std::vector<Math::BBox2f> > _getHandles() const override;
            void _setActiveWidget(bool) override;

            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

