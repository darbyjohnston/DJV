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

#include <djvUI/LineEdit.h>

#include <djvUI/Border.h>
#include <djvUI/LineEditBase.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct LineEdit::Private
        {
            std::shared_ptr<LineEditBase> lineEditBase;
            std::shared_ptr<Border> border;
        };

        void LineEdit::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::LineEdit");
            setVAlign(VAlign::Center);

            DJV_PRIVATE_PTR();
            p.lineEditBase = LineEditBase::create(context);

            p.border = Border::create(context);
            p.border->addChild(p.lineEditBase);
            addChild(p.border);

            auto weak = std::weak_ptr<LineEdit>(std::dynamic_pointer_cast<LineEdit>(shared_from_this()));
            p.lineEditBase->setFocusCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->border->setBorderColorRole(value ? ColorRole::Checked : ColorRole::Border);
                }
            });
        }

        LineEdit::LineEdit() :
            _p(new Private)
        {}

        LineEdit::~LineEdit()
        {}

        std::shared_ptr<LineEdit> LineEdit::create(Context * context)
        {
            auto out = std::shared_ptr<LineEdit>(new LineEdit);
            out->_init(context);
            return out;
        }

        const std::string& LineEdit::getText() const
        {
            return _p->lineEditBase->getText();
        }

        void LineEdit::setText(const std::string& value)
        {
            _p->lineEditBase->setText(value);
        }

        ColorRole LineEdit::getTextColorRole() const
        {
            return _p->lineEditBase->getTextColorRole();
        }

        void LineEdit::setTextColorRole(ColorRole value)
        {
            _p->lineEditBase->setTextColorRole(value);
        }

        const std::string & LineEdit::getFont() const
        {
            return _p->lineEditBase->getFont();
        }

        const std::string & LineEdit::getFontFace() const
        {
            return _p->lineEditBase->getFontFace();
        }

        MetricsRole LineEdit::getFontSizeRole() const
        {
            return _p->lineEditBase->getFontSizeRole();
        }

        void LineEdit::setFont(const std::string & value)
        {
            _p->lineEditBase->setFont(value);
        }

        void LineEdit::setFontFace(const std::string & value)
        {
            _p->lineEditBase->setFontFace(value);
        }

        void LineEdit::setFontSizeRole(MetricsRole value)
        {
            _p->lineEditBase->setFontSizeRole(value);
        }
        
        void LineEdit::_preLayoutEvent(Event::PreLayout& event)
        {
            if (auto style = _getStyle().lock())
            {
                DJV_PRIVATE_PTR();
                _setMinimumSize(p.border->getMinimumSize() + getMargin().getSize(style));
            }
        }

        void LineEdit::_layoutEvent(Event::Layout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f g = getGeometry();
                _p->border->setGeometry(getMargin().bbox(g, style));
            }
        }

    } // namespace UI
} // namespace djv
