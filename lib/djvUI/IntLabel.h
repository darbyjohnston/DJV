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
        class IntValueModel;

    } // namespace Core

    namespace UI
    {
        //! This class provides a label for integer values.
        class IntLabel : public Widget
        {
            DJV_NON_COPYABLE(IntLabel);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            IntLabel();

        public:
            ~IntLabel() override;

            static std::shared_ptr<IntLabel> create(const std::shared_ptr<Core::Context>&);

            const std::shared_ptr<Core::IntValueModel>& getModel() const;
            void setModel(const std::shared_ptr<Core::IntValueModel>&);

            static std::string getSizeString(const Core::IntRange&);
            
        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
