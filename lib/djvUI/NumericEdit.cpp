// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/NumericEdit.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvMath/NumericValueModels.h>

#include <djvCore/ValueObserver.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Numeric
        {
            struct IncrementButtons::Private
            {
                std::shared_ptr<ToolButton> incButtons[2];
                std::function<void(void)> incrementCallback;
                std::function<void(void)> decrementCallback;
            };

            void IncrementButtons::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Numeric::IncrementButtons");
                setBackgroundRole(ColorRole::Button);

                const std::vector<std::string> icons =
                {
                    "djvIconIncrement",
                    "djvIconDecrement"
                };
                for (size_t i = 0; i < 2; ++i)
                {
                    p.incButtons[i] = ToolButton::create(context);
                    p.incButtons[i]->setIcon(icons[i]);
                    p.incButtons[i]->setIconSizeRole(MetricsRole::IconMini);
                    p.incButtons[i]->setInsideMargin(MetricsRole::Border);
                    p.incButtons[i]->setAutoRepeat(true);
                    addChild(p.incButtons[i]);
                }

                auto weak = std::weak_ptr<IncrementButtons>(std::dynamic_pointer_cast<IncrementButtons>(shared_from_this()));
                p.incButtons[0]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->incrementCallback)
                            {
                                widget->_p->incrementCallback();
                            }
                        }
                    });
                p.incButtons[1]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->decrementCallback)
                            {
                                widget->_p->decrementCallback();
                            }
                        }
                    });
            }

            IncrementButtons::IncrementButtons() :
                _p(new Private)
            {}

            IncrementButtons::~IncrementButtons()
            {}

            std::shared_ptr<IncrementButtons> IncrementButtons::create(const std::shared_ptr<System::Context>& value)
            {
                auto out = std::shared_ptr<IncrementButtons>(new IncrementButtons);
                out->_init(value);
                return out;
            }

            void IncrementButtons::setIncrementEnabled(bool value)
            {
                _p->incButtons[0]->setEnabled(value);
            }

            void IncrementButtons::setDecrementEnabled(bool value)
            {
                _p->incButtons[1]->setEnabled(value);
            }

            void IncrementButtons::setIncrementCallback(const std::function<void(void)>& callback)
            {
                _p->incrementCallback = callback;
            }

            void IncrementButtons::setDecrementCallback(const std::function<void(void)>& callback)
            {
                _p->decrementCallback = callback;
            }

            void IncrementButtons::_preLayoutEvent(System::Event::PreLayout&)
            {
                DJV_PRIVATE_PTR();
                glm::vec2 size = glm::vec2(0.F, 0.F);
                for (size_t i = 0; i < 2; ++i)
                {
                    const auto& tmp = p.incButtons[i]->getMinimumSize();
                    size.x = std::max(size.x, tmp.x);
                    size.y += tmp.y;
                }
                _setMinimumSize(size);
            }

            void IncrementButtons::_layoutEvent(System::Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const Math::BBox2f& g = getGeometry();
                float x = g.min.x;
                float y = g.min.y;
                float w = g.w();
                float h = ceilf(g.h() / 2.F);
                p.incButtons[0]->setGeometry(Math::BBox2f(x, y, w, h));
                y = g.max.y - h;
                p.incButtons[1]->setGeometry(Math::BBox2f(x, y, w, h));
            }

            struct Edit::Private
            {
                std::shared_ptr<Text::LineEditBase> lineEditBase;
                std::shared_ptr<IncrementButtons> buttons;
            };

            void Edit::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Numeric::Edit");
                setVAlign(VAlign::Center);

                p.lineEditBase = Text::LineEditBase::create(context);
                p.lineEditBase->setFont(Render2D::Font::familyMono);
                p.lineEditBase->installEventFilter(shared_from_this());
                addChild(p.lineEditBase);

                p.buttons = IncrementButtons::create(context);
                addChild(p.buttons);

                auto weak = std::weak_ptr<Edit>(std::dynamic_pointer_cast<Edit>(shared_from_this()));
                p.lineEditBase->setTextEditCallback(
                    [weak](const std::string& value, TextEditReason reason)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_textEdit(value, reason);
                        }
                    });
                p.lineEditBase->setFocusCallback(
                    [weak](bool)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_redraw();
                        }
                    });

                p.buttons->setIncrementCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_incrementValue();
                        }
                    });
                p.buttons->setDecrementCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_decrementValue();
                        }
                    });
            }

            Edit::Edit() :
                _p(new Private)
            {}

            Edit::~Edit()
            {}

            void Edit::_preLayoutEvent(System::Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                glm::vec2 size = glm::vec2(0.F, 0.F);
                glm::vec2 tmp = p.lineEditBase->getMinimumSize();
                size.x += tmp.x;
                size.y = std::max(size.y, tmp.y);
                tmp = p.buttons->getMinimumSize();
                size.x += tmp.x;
                size.y = std::max(size.y, tmp.y);
                const auto& style = _getStyle();
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                _setMinimumSize(size + btf * 2.F + getMargin().getSize(style));
            }

            void Edit::_layoutEvent(System::Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                glm::vec2 tmp = p.buttons->getMinimumSize();
                Math::BBox2f g2 = g.margin(-btf);
                float x = g2.max.x - tmp.x;
                float y = g2.min.y;
                float w = tmp.x;
                float h = g2.h();
                p.buttons->setGeometry(Math::BBox2f(x, y, w, h));
                x = g2.min.x;
                w = g2.w() - tmp.x;
                p.lineEditBase->setGeometry(Math::BBox2f(x, y, w, h));
            }

            void Edit::_textUpdate(const std::string& text, const std::string& sizeString)
            {
                DJV_PRIVATE_PTR();
                p.lineEditBase->setText(text);
                p.lineEditBase->setSizeString(sizeString);
            }

            void Edit::_setIsMin(bool value)
            {
                DJV_PRIVATE_PTR();
                p.buttons->setDecrementEnabled(!value);
            }

            void Edit::_setIsMax(bool value)
            {
                DJV_PRIVATE_PTR();
                p.buttons->setIncrementEnabled(!value);
            }

            void Edit::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float b = style->getMetric(UI::MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getGeometry();
                const auto& render = _getRender();
                if (p.lineEditBase->hasTextFocus())
                {
                    render->setFillColor(style->getColor(UI::ColorRole::TextFocus));
                    drawBorder(render, g, btf);
                }
                else
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Border));
                    drawBorder(render, g.margin(-b), b);
                }
            }

            bool Edit::_eventFilter(const std::shared_ptr<IObject>& object, System::Event::Event& event)
            {
                DJV_PRIVATE_PTR();
                switch (event.getEventType())
                {
                case System::Event::Type::KeyPress:
                {
                    auto& keyPressEvent = static_cast<System::Event::KeyPress&>(event);
                    if (_keyPress(keyPressEvent.getKey()))
                    {
                        keyPressEvent.accept();
                        return true;
                    }
                    break;
                }
                case System::Event::Type::Scroll:
                {
                    auto& scrollEvent = static_cast<System::Event::Scroll&>(event);
                    scrollEvent.accept();
                    p.lineEditBase->takeTextFocus();
                    _scroll(scrollEvent.getScrollDelta().y);
                    return true;
                }
                default: break;
                }
                return false;
            }

        } // namespace Numeric
    } // namespace UI
} // namespace djv
