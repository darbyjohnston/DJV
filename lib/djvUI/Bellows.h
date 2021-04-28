// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! Bellows widget.
            class Bellows : public Widget
            {
                DJV_NON_COPYABLE(Bellows);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Bellows();

            public:
                ~Bellows() override;

                static std::shared_ptr<Bellows> create(const std::shared_ptr<System::Context>&);

                //! \name Text
                ///@{
                
                const std::string& getText() const;

                void setText(const std::string&);

                ///@}

                //! \name Open and Close
                ///@{

                bool isOpen() const;

                void setOpen(bool, bool animate = true);
                void open(bool animate = true);
                void close(bool animate = true);
                void setOpenCallback(const std::function<void(bool)>&);

                ///@}

                //! \name Button Widgets
                ///@{

                void addButtonWidget(const std::shared_ptr<IObject>&);
                void removeButtonWidget(const std::shared_ptr<IObject>&);
                void clearButtonWidgets();

                ///@}

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;
                void clearChildren() override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Bellows Bellows;

    } // namespace UI
} // namespace djv

