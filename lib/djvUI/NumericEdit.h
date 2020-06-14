// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides buttons for incrementing/decrementing a value.
        class NumericEditButtons : public Widget
        {
            DJV_NON_COPYABLE(NumericEditButtons);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            NumericEditButtons();

        public:
            virtual ~NumericEditButtons();

            static std::shared_ptr<NumericEditButtons> create(const std::shared_ptr<Core::Context>&);

            void setIncrementEnabled(bool);
            void setDecrementEnabled(bool);

            void setIncrementCallback(const std::function<void(void)>&);
            void setDecrementCallback(const std::function<void(void)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

        //! This class provides the base functionality for numeric editor widgets.
        class NumericEdit : public Widget
        {
            DJV_NON_COPYABLE(NumericEdit);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            NumericEdit();

        public:
            virtual ~NumericEdit() = 0;

        protected:
            virtual void _textEdit(const std::string&, TextEditReason) = 0;
            virtual bool _keyPress(int) = 0;
            virtual void _scroll(float) = 0;
            virtual void _incrementValue() = 0;
            virtual void _decrementValue() = 0;
            void _textUpdate(const std::string&, const std::string&);
            void _setIsMin(bool);
            void _setIsMax(bool);

            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

            bool _eventFilter(const std::shared_ptr<IObject>&, Core::Event::Event&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
