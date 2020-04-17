// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/FormLayout.h>

#include <djvUI/GridLayout.h>
#include <djvUI/Label.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Form::Private
            {
                std::vector<ColorRole> alternateRowsRoles = { ColorRole::None, ColorRole::None };
                std::shared_ptr<Grid> layout;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<Label>> widgetToLabel;
                std::weak_ptr<LabelSizeGroup> sizeGroup;
            };

            void Form::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Form");

                DJV_PRIVATE_PTR();
                p.layout = Grid::create(context);
                Widget::addChild(p.layout);
            }

            Form::Form() :
                _p(new Private)
            {}

            Form::~Form()
            {}

            std::shared_ptr<Form> Form::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Form>(new Form);
                out->_init(context);
                return out;
            }

            void Form::setText(const std::shared_ptr<Widget> & value, const std::string & text)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.widgetToLabel.find(value);
                if (i != p.widgetToLabel.end())
                {
                    i->second->setText(text);
                }
                else if (auto context = getContext().lock())
                {
                    auto label = Label::create(context);
                    label->setText(text);
                    label->setTextHAlign(TextHAlign::Left);
                    label->setMargin(Margin(MetricsRole::MarginSmall));
                    label->setSizeGroup(p.sizeGroup);
                    glm::ivec2 gridPos = p.layout->getGridPos(value);
                    p.layout->addChild(label);
                    p.layout->setGridPos(label, glm::ivec2(0, gridPos.y));
                    p.layout->setGridPos(value, glm::ivec2(1, gridPos.y), GridStretch::Horizontal);
                    p.widgetToLabel[value] = label;
                }
            }

            const Spacing & Form::getSpacing() const
            {
                return _p->layout->getSpacing();
            }

            void Form::setSpacing(const Spacing & value)
            {
                _p->layout->setSpacing(value);
            }

            void Form::setAlternateRowsRoles(ColorRole value0, ColorRole value1)
            {
                DJV_PRIVATE_PTR();
                if (value0 == p.alternateRowsRoles[0] && value1 == p.alternateRowsRoles[1])
                    return;
                p.alternateRowsRoles = { value0, value1 };
                _widgetUpdate();
            }

            void Form::setSizeGroup(const std::weak_ptr<LabelSizeGroup>& value)
            {
                DJV_PRIVATE_PTR();
                for (auto i : p.widgetToLabel)
                {
                    i.second->setSizeGroup(std::weak_ptr<LabelSizeGroup>());
                }
                p.sizeGroup = value;
                for (auto i : p.widgetToLabel)
                {
                    i.second->setSizeGroup(p.sizeGroup);
                }
            }

            float Form::getHeightForWidth(float value) const
            {
                const auto& style = _getStyle();
                float out = _p->layout->getHeightForWidth(value - getMargin().getWidth(style)) + getMargin().getWidth(style);
                return out;
            }

            void Form::addChild(const std::shared_ptr<IObject> & value)
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

            void Form::removeChild(const std::shared_ptr<IObject> & value)
            {
                DJV_PRIVATE_PTR();
                p.layout->removeChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    const auto i = p.widgetToLabel.find(widget);
                    if (i != p.widgetToLabel.end())
                    {
                        p.widgetToLabel.erase(i);
                    }
                }
                _widgetUpdate();
            }

            void Form::clearChildren()
            {
                DJV_PRIVATE_PTR();
                p.layout->clearChildren();
                p.widgetToLabel.clear();
                _widgetUpdate();
            }

            void Form::_preLayoutEvent(Event::PreLayout & event)
            {
                const auto& style = _getStyle();
                _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
            }

            void Form::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();
                const BBox2f & g = getGeometry();
                const auto& style = _getStyle();
                p.layout->setGeometry(getMargin().bbox(g, style));
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
