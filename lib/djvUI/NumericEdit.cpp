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

#include <djvUI/LineEdit.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

#include <djvAV/FontSystem.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct NumericEdit::Private
        {
            std::shared_ptr<LineEdit> lineEdit;
            std::shared_ptr<ToolButton> incButtons[2];
            std::shared_ptr<HorizontalLayout> layout;
        };

        void NumericEdit::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::NumericEdit");
            setVAlign(VAlign::Center);

            p.lineEdit = LineEdit::create(context);
            p.lineEdit->setFont(AV::Font::familyMono);

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
                p.incButtons[i]->setBackgroundRole(ColorRole::Button);
            }

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(Layout::Spacing(MetricsRole::None));
            p.layout->addChild(p.lineEdit);
            p.layout->setStretch(p.lineEdit, RowStretch::Expand);
            auto vLayout = VerticalLayout::create(context);
            vLayout->setMargin(Layout::Margin(MetricsRole::Border));
            vLayout->setSpacing(Layout::Spacing(MetricsRole::Border));
            for (size_t i = 0; i < 2; ++i)
            {
                vLayout->addChild(p.incButtons[i]);
                vLayout->setStretch(p.incButtons[i], RowStretch::Expand);
            }
            p.layout->addChild(vLayout);
            addChild(p.layout);

            auto weak = std::weak_ptr<NumericEdit>(std::dynamic_pointer_cast<NumericEdit>(shared_from_this()));
            p.lineEdit->setTextEditCallback(
                [weak](const std::string & value, TextEdit textEdit)
            {
                if (auto widget = weak.lock())
                {
                    widget->_textEdit(value, textEdit);
                }
            });

            p.incButtons[0]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_incrementValue();
                    }
                });

            p.incButtons[1]->setClickedCallback(
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
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void NumericEdit::_layoutEvent(Event::Layout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void NumericEdit::_textUpdate(const std::string& text, const std::string& sizeString)
        {
            DJV_PRIVATE_PTR();
            p.lineEdit->setText(text);
            p.lineEdit->setSizeString(sizeString);
        }

        void NumericEdit::_keyPressEvent(Event::KeyPress& event)
        {
            Widget::_keyPressEvent(event);
            DJV_PRIVATE_PTR();
            if (!event.isAccepted())
            {
                if (_keyPress(event.getKey()))
                {
                    event.accept();
                }
            }
        }

    } // namespace UI
} // namespace djv
