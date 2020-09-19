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
        class IntValueModel;

    } // namespace Core

    namespace UI
    {
        //! This class provides a label for integer values.
        class IntLabel : public Widget
        {
            DJV_NON_COPYABLE(IntLabel);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            IntLabel();

        public:
            ~IntLabel() override;

            static std::shared_ptr<IntLabel> create(const std::shared_ptr<System::Context>&);

            const std::shared_ptr<Math::IntValueModel>& getModel() const;
            void setModel(const std::shared_ptr<Math::IntValueModel>&);

            static std::string getSizeString(const Math::IntRange&);
            
        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
