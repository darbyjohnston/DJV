// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvCore/Range.h>

namespace djv
{
    namespace Core
    {
        class FloatValueModel;

    } // namespace Core

    namespace UI
    {
        //! This class provides a label for floating-point values.
        class FloatLabel : public Widget
        {
            DJV_NON_COPYABLE(FloatLabel);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            FloatLabel();

        public:
            virtual ~FloatLabel();

            static std::shared_ptr<FloatLabel> create(const std::shared_ptr<Core::Context>&);

            const std::shared_ptr<Core::FloatValueModel> & getModel() const;
            void setModel(const std::shared_ptr<Core::FloatValueModel> &);

            size_t getPrecision();
            void setPrecision(size_t);
            
            static std::string getSizeString(const Core::FloatRange&, size_t precision);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
