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
        //! This class provides an editor widget for floating-point values.
        class FloatEdit : public INumericEdit<float>, public NumericEdit
        {
            DJV_NON_COPYABLE(FloatEdit);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            FloatEdit();

        public:
            ~FloatEdit() override;

            static std::shared_ptr<FloatEdit> create(const std::shared_ptr<Core::Context>&);

            int getPrecision();
            void setPrecision(int);

            void setModel(const std::shared_ptr<Core::INumericValueModel<float> >&) override;

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
