// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/FormLayout.h>

#include <djvUI/GridLayout.h>
#include <djvUI/TextBlock.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Form::Private
            {
                std::string fontFamily;
                std::string fontFace;
                std::vector<ColorRole> alternateRowsRoles = { ColorRole::None, ColorRole::None };
                VAlign labelVAlign = VAlign::Center;
                std::shared_ptr<Grid> layout;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<Text::Block>> widgetToText;
            };

            void Form::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::Form");

                p.layout = Grid::create(context);
                p.layout->setSpacing(MetricsRole::None);
                Widget::addChild(p.layout);
            }

            Form::Form() :
                _p(new Private)
            {}

            Form::~Form()
            {}

            std::shared_ptr<Form> Form::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Form>(new Form);
                out->_init(context);
                return out;
            }

            void Form::setText(const std::shared_ptr<Widget>& value, const std::string& text)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.widgetToText.find(value);
                if (i != p.widgetToText.end())
                {
                    i->second->setText(text);
                }
                else if (auto context = getContext().lock())
                {
                    auto textBlock = Text::Block::create(context);
                    textBlock->setText(text);
                    textBlock->setFontFamily(p.fontFamily);
                    textBlock->setFontFace(p.fontFace);
                    textBlock->setTextSizeRole(MetricsRole::ScrollAreaSmall);
                    textBlock->setMargin(Margin(MetricsRole::MarginSmall));
                    textBlock->setVAlign(p.labelVAlign);
                    glm::ivec2 gridPos = p.layout->getGridPos(value);
                    p.layout->addChild(textBlock);
                    p.layout->setGridPos(textBlock, glm::ivec2(0, gridPos.y));
                    p.layout->setGridPos(value, glm::ivec2(1, gridPos.y), GridStretch::Horizontal);
                    p.widgetToText[value] = textBlock;
                }
            }

            const std::string& Form::getFontFamily() const
            {
                return _p->fontFamily;
            }

            const std::string& Form::getFontFace() const
            {
                return _p->fontFace;
            }

            void Form::setFontFamily(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontFamily)
                    return;
                p.fontFamily = value;
                for (const auto& i : p.widgetToText)
                {
                    i.second->setFontFamily(value);
                }
            }

            void Form::setFontFace(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontFace)
                    return;
                p.fontFace = value;
                for (const auto& i : p.widgetToText)
                {
                    i.second->setFontFace(value);
                }
            }

            const Spacing& Form::getSpacing() const
            {
                return _p->layout->getSpacing();
            }

            void Form::setSpacing(const Spacing& value)
            {
                _p->layout->setSpacing(value);
            }

            void Form::setLabelVAlign(VAlign value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.labelVAlign)
                    return;
                p.labelVAlign = value;
                for (auto i : p.widgetToText)
                {
                    i.second->setVAlign(value);
                }
            }

            void Form::setAlternateRowsRoles(ColorRole value0, ColorRole value1)
            {
                DJV_PRIVATE_PTR();
                if (value0 == p.alternateRowsRoles[0] && value1 == p.alternateRowsRoles[1])
                    return;
                p.alternateRowsRoles = { value0, value1 };
                _widgetUpdate();
            }

            float Form::getHeightForWidth(float value) const
            {
                const auto& style = _getStyle();
                float out = _p->layout->getHeightForWidth(value - getMargin().getWidth(style)) + getMargin().getHeight(style);
                return out;
            }

            void Form::addChild(const std::shared_ptr<IObject>& value)
            {
                DJV_PRIVATE_PTR();
                const glm::ivec2 gridSize = p.layout->getGridSize();
                p.layout->addChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    p.layout->setGridPos(widget, glm::ivec2(1, gridSize.y));
                    p.layout->setStretch(widget, GridStretch::Horizontal);
                }
                _widgetUpdate();
            }

            void Form::removeChild(const std::shared_ptr<IObject>& value)
            {
                DJV_PRIVATE_PTR();
                p.layout->removeChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    const auto i = p.widgetToText.find(widget);
                    if (i != p.widgetToText.end())
                    {
                        p.widgetToText.erase(i);
                    }
                }
                _widgetUpdate();
            }

            void Form::clearChildren()
            {
                DJV_PRIVATE_PTR();
                p.layout->clearChildren();
                p.widgetToText.clear();
                _widgetUpdate();
            }

            void Form::_preLayoutEvent(System::Event::PreLayout& event)
            {
                const auto& style = _getStyle();
                _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
            }

            void Form::_layoutEvent(System::Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
                p.layout->setGeometry(g);
            }

            void Form::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                const glm::ivec2 gridSize = p.layout->getGridSize();
                for (int y = 0; y < gridSize.y; ++y)
                {
                    p.layout->setRowBackgroundRole(y, (y % 2 == 0) ? p.alternateRowsRoles[0] : p.alternateRowsRoles[1]);
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
