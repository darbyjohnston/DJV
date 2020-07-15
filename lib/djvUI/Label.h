// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        class LabelSizeGroup;

        //! This class provides a label.
        class Label : public Widget
        {
            DJV_NON_COPYABLE(Label);
            
        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            Label();

        public:
            virtual ~Label();

            //! \name Label Creation
            ///@{

            static std::shared_ptr<Label> create(const std::shared_ptr<Core::Context>&);

            ///@}

            //! \name Label Text
            ///@{

            const std::string& getText() const;
            void setText(const std::string&);

            TextHAlign getTextHAlign() const;
            TextVAlign getTextVAlign() const;
            void setTextHAlign(TextHAlign);
            void setTextVAlign(TextVAlign);

            ColorRole getTextColorRole() const;
            void setTextColorRole(ColorRole);

            const std::string& getFontFamily() const;
            const std::string& getFontFace() const;
            MetricsRole getFontSizeRole() const;
            void setFontFamily(const std::string&);
            void setFontFace(const std::string&);
            void setFontSizeRole(MetricsRole);

            int getElide() const;
            void setElide(int);

            const std::string& getSizeString() const;
            void setSizeString(const std::string&);

            ///@}

            void setSizeGroup(const std::weak_ptr<LabelSizeGroup>&);

        protected:
            void _initLayoutEvent(Core::Event::InitLayout&) override;
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

            void _initEvent(Core::Event::Init&) override;
            void _updateEvent(Core::Event::Update&) override;

        private:
            glm::vec2 _labelMinimumSize = glm::vec2(0.F, 0.F);

            void _textUpdate();
            void _sizeStringUpdate();
            void _fontUpdate();

            friend class LabelSizeGroup;

            DJV_PRIVATE();
        };

        //! This class provides functionality to constrain the minimum size
        //! of multiple Label widgets.
        class LabelSizeGroup : public std::enable_shared_from_this<LabelSizeGroup>
        {
            DJV_NON_COPYABLE(LabelSizeGroup);

        protected:
            LabelSizeGroup();

        public:
            virtual ~LabelSizeGroup();

            static std::shared_ptr<LabelSizeGroup> create();

            void addLabel(const std::weak_ptr<Label>&);
            void removeLabel(const std::weak_ptr<Label>&);
            void clearLabels();

            const glm::vec2& getMinimumSize() const;
            void calcMinimumSize();

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
