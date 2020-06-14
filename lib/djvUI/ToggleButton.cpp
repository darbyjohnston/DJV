// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ToggleButton.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Style.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>

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
            namespace
            {
                //! \todo Should this be configurable?
                const size_t animationDuration = 50;
            
            } // namespace

            struct Toggle::Private
            {
                float animationValue = 0.F;
                std::shared_ptr<Animation::Animation> animation;
            };

            void Toggle::_init(const std::shared_ptr<Context>& context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Button::Toggle");
                setButtonType(ButtonType::Toggle);
                setHAlign(HAlign::Left);
                setVAlign(VAlign::Center);
                p.animation = Animation::Animation::create(context);
            }

            Toggle::Toggle() :
                _p(new Private)
            {}

            Toggle::~Toggle()
            {}

            std::shared_ptr<Toggle> Toggle::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Toggle>(new Toggle);
                out->_init(context);
                return out;
            }

            void Toggle::setChecked(bool value)
            {
                IButton::setChecked(value);
                DJV_PRIVATE_PTR();
                if (isVisible(true))
                {
                    auto weak = std::weak_ptr<Toggle>(std::dynamic_pointer_cast<Toggle>(shared_from_this()));
                    p.animation->start(
                        p.animationValue,
                        value ? 1.F : 0.F,
                        std::chrono::milliseconds(animationDuration),
                        [weak](float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->animationValue = value;
                            widget->_redraw();
                        }
                    });
                }
                else
                {
                    p.animationValue = value ? 1.F : 0.F;
                }
            }

            bool Toggle::acceptFocus(TextFocusDirection)
            {
                bool out = false;
                if (isEnabled(true) && isVisible(true) && !isClipped())
                {
                    takeTextFocus();
                    out = true;
                }
                return out;
            }

            void Toggle::_preLayoutEvent(Event::PreLayout& event)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const float is = style->getMetric(MetricsRole::IconSmall);
                _setMinimumSize(glm::vec2(is * 2.F, is) + b * 4.F + getMargin().getSize(style));
            }

            void Toggle::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);

                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const BBox2f& g = getMargin().bbox(getGeometry(), style);

                const auto& render = _getRender();
                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(ColorRole::TextFocus));
                    drawBorder(render, g, b * 2.F);
                }

                const BBox2f& g2 = g.margin(-b * 2.F);
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g2, b);
                render->setFillColor(style->getColor(isChecked() ? ColorRole::Checked : ColorRole::Trough));
                render->drawRect(g2.margin(-b));

                if (_isHovered())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g2);
                }

                const float r = g2.h() / 2.F;
                const float x = Math::lerp(p.animationValue, g2.min.x + r, g2.max.x - r);
                const BBox2f handleBBox = BBox2f(x - r, g2.min.y, r * 2.F, r * 2.F);
                auto color = style->getColor(ColorRole::Border);
                render->setFillColor(color);
                render->drawRect(handleBBox);
                color = style->getColor(ColorRole::Button);
                render->setFillColor(color);
                render->drawRect(handleBBox.margin(-b));

                if (_isPressed())
                {
                    render->setFillColor(style->getColor(ColorRole::Pressed));
                    render->drawRect(handleBBox);
                }
            }

            void Toggle::_buttonPressEvent(Event::ButtonPress& event)
            {
                IButton::_buttonPressEvent(event);
                if (event.isAccepted())
                {
                    takeTextFocus();
                }
            }

            void Toggle::_keyPressEvent(Event::KeyPress& event)
            {
                IButton::_keyPressEvent(event);
                if (!event.isAccepted() && hasTextFocus())
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_ENTER:
                    case GLFW_KEY_SPACE:
                        event.accept();
                        setChecked(!isChecked());
                        _doCheckedCallback(isChecked());
                        break;
                    case GLFW_KEY_ESCAPE:
                        event.accept();
                        releaseTextFocus();
                        break;
                    default: break;
                    }
                }
            }

            void Toggle::_textFocusEvent(Event::TextFocus&)
            {
                _redraw();
            }

            void Toggle::_textFocusLostEvent(Event::TextFocusLost&)
            {
                _redraw();
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
