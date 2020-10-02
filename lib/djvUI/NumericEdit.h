// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Numeric
        {
            //! This class provides buttons for incrementing/decrementing a value.
            class IncrementButtons : public Widget
            {
                DJV_NON_COPYABLE(IncrementButtons);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                IncrementButtons();

            public:
                ~IncrementButtons() override;

                static std::shared_ptr<IncrementButtons> create(const std::shared_ptr<System::Context>&);

                void setIncrementEnabled(bool);
                void setDecrementEnabled(bool);

                void setIncrementCallback(const std::function<void(void)>&);
                void setDecrementCallback(const std::function<void(void)>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

            //! This class provides the base functionality for numeric editor widgets.
            class Edit : public Widget
            {
                DJV_NON_COPYABLE(Edit);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Edit();

            public:
                virtual ~Edit() = 0;

            protected:
                virtual void _textEdit(const std::string&, TextEditReason) = 0;
                virtual bool _keyPress(int) = 0;
                virtual void _scroll(float) = 0;
                virtual void _incrementValue() = 0;
                virtual void _decrementValue() = 0;
                void _textUpdate(const std::string&, const std::string&);
                void _setIsMin(bool);
                void _setIsMax(bool);

                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

                bool _eventFilter(const std::shared_ptr<IObject>&, System::Event::Event&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Numeric
    } // namespace UI
} // namespace djv
