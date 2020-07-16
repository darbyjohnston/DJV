// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/INumericWidget.h>
#include <djvUI/NumericEdit.h>

namespace djv
{
    namespace UI
    {
        //! This class provides an editor widget for integer values.
        class IntEdit : public INumericEdit<int>, public NumericEdit
        {
            DJV_NON_COPYABLE(IntEdit);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            IntEdit();

        public:
            ~IntEdit() override;

            static std::shared_ptr<IntEdit> create(const std::shared_ptr<Core::Context>&);

            void setModel(const std::shared_ptr<Core::INumericValueModel<int> >&) override;

        protected:
            void _setIsMin(bool) override;
            void _setIsMax(bool) override;

            void _textEdit(const std::string&, TextEditReason) override;
            bool _keyPress(int) override;
            void _scroll(float) override;
            void _incrementValue() override;
            void _decrementValue() override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
