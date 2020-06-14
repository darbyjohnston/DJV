// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Spacing.h>
#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        class LabelSizeGroup;

        namespace Layout
        {
            //! This class provides a form layout.
            class Form : public Widget
            {
                DJV_NON_COPYABLE(Form);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Form();

            public:
                virtual ~Form();

                static std::shared_ptr<Form> create(const std::shared_ptr<Core::Context>&);

                void setText(const std::shared_ptr<Widget>&, const std::string&);

                const Spacing& getSpacing() const;
                void setSpacing(const Spacing&);

                const std::string& getFontFamily() const;
                const std::string& getFontFace() const;
                void setFontFamily(const std::string&);
                void setFontFace(const std::string&);

                void setAlternateRowsRoles(ColorRole, ColorRole);

                void setLabelVAlign(VAlign);
                void setLabelSizeGroup(const std::weak_ptr<LabelSizeGroup>&);

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;
                void clearChildren() override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Form FormLayout;

    } // namespace UI
} // namespace djv

