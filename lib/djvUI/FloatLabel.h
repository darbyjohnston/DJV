// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvMath/Range.h>

namespace djv
{
    namespace Math
    {
        class FloatValueModel;

    } // namespace Math

    namespace UI
    {
        //! This class provides a label for floating-point values.
        class FloatLabel : public Widget
        {
            DJV_NON_COPYABLE(FloatLabel);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            FloatLabel();

        public:
            ~FloatLabel() override;

            static std::shared_ptr<FloatLabel> create(const std::shared_ptr<System::Context>&);

            const std::shared_ptr<Math::FloatValueModel>& getModel() const;
            void setModel(const std::shared_ptr<Math::FloatValueModel>&);

            size_t getPrecision();
            void setPrecision(size_t);
            
            static std::string getSizeString(const Math::FloatRange&, size_t precision);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
