//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUI/NumericEdit.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct NumericEditButtons::Private
        {
            std::shared_ptr<ToolButton> incButtons[2];
            std::shared_ptr<VerticalLayout> layout;
            std::function<void(void)> incrementCallback;
            std::function<void(void)> decrementCallback;
        };

        void NumericEditButtons::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::NumericEditButtons");
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
                p.incButtons[i]->setInsideMargin(Layout::Margin(MetricsRole::Border));
                p.incButtons[i]->setAutoRepeat(true);
                p.incButtons[i]->setVAlign(VAlign::Fill);
            }

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(Layout::Spacing(MetricsRole::None));
            for (size_t i = 0; i < 2; ++i)
            {
                p.layout->addChild(p.incButtons[i]);
                p.layout->setStretch(p.incButtons[i], RowStretch::Expand);
            }
            addChild(p.layout);

            auto weak = std::weak_ptr<NumericEditButtons>(std::dynamic_pointer_cast<NumericEditButtons>(shared_from_this()));
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

        NumericEditButtons::NumericEditButtons() :
            _p(new Private)
        {}

        NumericEditButtons::~NumericEditButtons()
        {}

        std::shared_ptr<NumericEditButtons> NumericEditButtons::create(const std::shared_ptr<Context>& value)
        {
            auto out = std::shared_ptr<NumericEditButtons>(new NumericEditButtons);
            out->_init(value);
            return out;
        }

        void NumericEditButtons::setIncrementEnabled(bool value)
        {
            _p->incButtons[0]->setEnabled(value);
        }

        void NumericEditButtons::setDecrementEnabled(bool value)
        {
            _p->incButtons[1]->setEnabled(value);
        }

        void NumericEditButtons::setIncrementCallback(const std::function<void(void)>& callback)
        {
            _p->incrementCallback = callback;
        }

        void NumericEditButtons::setDecrementCallback(const std::function<void(void)>& callback)
        {
            _p->decrementCallback = callback;
        }

        void NumericEditButtons::_preLayoutEvent(Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            _setMinimumSize(p.layout->getMinimumSize());
        }

        void NumericEditButtons::_layoutEvent(Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            p.layout->setGeometry(getGeometry());
        }

        struct NumericEdit::Private
        {
            std::shared_ptr<LineEditBase> lineEditBase;
            std::shared_ptr<NumericEditButtons> buttons;
            std::shared_ptr<HorizontalLayout> layout;
        };

        void NumericEdit::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::NumericEdit");
            setVAlign(VAlign::Center);

            p.lineEditBase = LineEditBase::create(context);
            p.lineEditBase->setFont(AV::Font::familyMono);

            p.buttons = NumericEditButtons::create(context);

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(Layout::Spacing(MetricsRole::None));
            p.layout->addChild(p.lineEditBase);
            p.layout->setStretch(p.lineEditBase, RowStretch::Expand);
            p.layout->addChild(p.buttons);
            addChild(p.layout);

            auto weak = std::weak_ptr<NumericEdit>(std::dynamic_pointer_cast<NumericEdit>(shared_from_this()));
            p.lineEditBase->setTextEditCallback(
                [weak](const std::string & value, TextEdit textEdit)
            {
                if (auto widget = weak.lock())
                {
                    widget->_textEdit(value, textEdit);
                }
            });
            p.lineEditBase->setFocusCallback(
                [weak](bool value)
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

        NumericEdit::NumericEdit() :
            _p(new Private)
        {}

        NumericEdit::~NumericEdit()
        {}

        void NumericEdit::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            glm::vec2 size = p.layout->getMinimumSize();
            const auto& style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border);
            _setMinimumSize(size + b * 6.F + getMargin().getSize(style));
        }

        void NumericEdit::_layoutEvent(Event::Layout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            const float b = style->getMetric(MetricsRole::Border);
            p.layout->setGeometry(g.margin(-b * 3.F));
        }

        void NumericEdit::_textUpdate(const std::string& text, const std::string& sizeString)
        {
            DJV_PRIVATE_PTR();
            p.lineEditBase->setText(text);
            p.lineEditBase->setSizeString(sizeString);
        }

        void NumericEdit::_setIsMin(bool value)
        {
            DJV_PRIVATE_PTR();
            p.buttons->setDecrementEnabled(!value);
        }

        void NumericEdit::_setIsMax(bool value)
        {
            DJV_PRIVATE_PTR();
            p.buttons->setIncrementEnabled(!value);
        }

        void NumericEdit::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getGeometry();
            const float b = style->getMetric(UI::MetricsRole::Border);
            auto render = _getRender();
            if (p.lineEditBase->hasTextFocus())
            {
                render->setFillColor(style->getColor(UI::ColorRole::TextFocus));
                drawBorder(render, g, b * 2.F);
            }
            render->setFillColor(style->getColor(UI::ColorRole::Border));
            const BBox2f g2 = g.margin(-b * 2.F);
            drawBorder(render, g2, b);
        }

        void NumericEdit::_keyPressEvent(Event::KeyPress& event)
        {
            Widget::_keyPressEvent(event);
            DJV_PRIVATE_PTR();
            if (!event.isAccepted() && p.lineEditBase->hasTextFocus())
            {
                if (_keyPress(event.getKey()))
                {
                    event.accept();
                }
            }
        }

    } // namespace UI
} // namespace djv
