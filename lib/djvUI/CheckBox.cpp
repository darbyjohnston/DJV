// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/CheckBox.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Label.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct CheckBox::Private
            {
                std::shared_ptr<Text::Label> label;
            };

            void CheckBox::_init(const std::shared_ptr<System::Context>& context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Button::CheckBox");
                setButtonType(UI::ButtonType::Toggle);

                p.label = Text::Label::create(context);
                p.label->setTextHAlign(TextHAlign::Left);
                p.label->setTextColorRole(getForegroundColorRole());
                p.label->setMargin(MetricsRole::MarginSmall);
                addChild(p.label);

                _widgetUpdate();
            }

            CheckBox::CheckBox() :
                _p(new Private)
            {}

            CheckBox::~CheckBox()
            {}

            std::shared_ptr<CheckBox> CheckBox::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<CheckBox>(new CheckBox);
                out->_init(context);
                return out;
            }

            std::string CheckBox::getText() const
            {
                DJV_PRIVATE_PTR();
                return p.label ? p.label->getText() : std::string();
            }

            void CheckBox::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.label->getText())
                    return;
                p.label->setText(value);
                _widgetUpdate();
            }

            const std::string& CheckBox::getFontFamily() const
            {
                return _p->label->getFontFamily();
            }

            const std::string& CheckBox::getFontFace() const
            {
                return _p->label->getFontFace();
            }

            MetricsRole CheckBox::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void CheckBox::setFontFamily(const std::string& value)
            {
                _p->label->setFontFamily(value);
            }

            void CheckBox::setFontFace(const std::string& value)
            {
                _p->label->setFontFace(value);
            }

            void CheckBox::setFontSizeRole(MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            bool CheckBox::acceptFocus(TextFocusDirection)
            {
                bool out = false;
                if (isEnabled(true) && isVisible(true) && !isClipped())
                {
                    takeTextFocus();
                    out = true;
                }
                return out;
            }

            void CheckBox::_preLayoutEvent(System::Event::PreLayout& event)
            {
                const auto& style = _getStyle();
                const float m = 0.F;// style->getMetric(MetricsRole::MarginInside);
                const float bt = style->getMetric(MetricsRole::BorderTextFocus);
                const glm::vec2 checkBoxSize = getCheckBoxSize(style);
                const glm::vec2 labelSize = _p->label->getMinimumSize();
                const glm::vec2 size(
                    checkBoxSize.x + labelSize.x + (m + bt) * 2.F,
                    std::max(checkBoxSize.y, labelSize.y) + (m + bt) * 2.F);
                _setMinimumSize(size);
            }

            void CheckBox::_layoutEvent(System::Event::Layout&)
            {
                _p->label->setGeometry(_getLabelGeometry());
            }

            void CheckBox::_paintEvent(System::Event::Paint& event)
            {
                IButton::_paintEvent(event);
                const auto& style = _getStyle();
                const float bt = style->getMetric(MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getGeometry();

                const auto& render = _getRender();
                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(ColorRole::TextFocus));
                    drawBorder(render, g, bt);
                }

                const Math::BBox2f g2 = g.margin(-bt);
                if (_isPressed())
                {
                    render->setFillColor(style->getColor(ColorRole::Pressed));
                    render->drawRect(g2);
                }
                else if (_isHovered())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g2);
                }

                drawCheckBox(render, style, _getCheckGeometry(), _isToggled());
            }

            void CheckBox::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                IButton::_buttonPressEvent(event);
                if (event.isAccepted())
                {
                    takeTextFocus();
                }
            }

            void CheckBox::_keyPressEvent(System::Event::KeyPress& event)
            {
                IButton::_keyPressEvent(event);
                if (!event.isAccepted() && hasTextFocus())
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_ENTER:
                    case GLFW_KEY_SPACE:
                        event.accept();
                        switch (getButtonType())
                        {
                        case ButtonType::Toggle:
                            _doCheck(!isChecked());
                            break;
                        case ButtonType::Radio:
                            if (!isChecked())
                            {
                                _doCheck(true);
                            }
                            break;
                        case ButtonType::Exclusive:
                            _doCheck(!isChecked());
                            break;
                        default: break;
                        }
                        break;
                    case GLFW_KEY_ESCAPE:
                        event.accept();
                        releaseTextFocus();
                        break;
                    default: break;
                    }
                }
            }

            void CheckBox::_textFocusEvent(System::Event::TextFocus&)
            {
                _redraw();
            }

            void CheckBox::_textFocusLostEvent(System::Event::TextFocusLost&)
            {
                _redraw();
            }

            Math::BBox2f CheckBox::_getCheckGeometry() const
            {
                const auto& style = _getStyle();
                const glm::vec2 size = getCheckBoxSize(style);
                const float m = style->getMetric(MetricsRole::MarginInside);
                const float bt = style->getMetric(MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getGeometry();
                const Math::BBox2f g2 = g.margin(-(m + bt));
                return Math::BBox2f(g2.min.x, floorf(g2.min.y + g2.h() / 2.F - size.y / 2.F), size.x, size.y);
            }

            Math::BBox2f CheckBox::_getLabelGeometry() const
            {
                const auto& style = _getStyle();
                const glm::vec2 size = getCheckBoxSize(style);
                const float m = style->getMetric(MetricsRole::MarginInside);
                const float bt = style->getMetric(MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getGeometry();
                const Math::BBox2f g2 = g.margin(-(m + bt));
                return Math::BBox2f(g2.min.x + size.x, g2.min.y, g2.w() - size.y, g2.h());
            }

            void CheckBox::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                p.label->setVisible(!p.label->getText().empty());
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
