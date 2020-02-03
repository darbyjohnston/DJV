//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUI/DrawUtil.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct LineEdit::Private
        {
            std::shared_ptr<LineEditBase> lineEditBase;
        };

        void LineEdit::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::LineEdit");
            setVAlign(VAlign::Center);

            p.lineEditBase = LineEditBase::create(context);
            p.lineEditBase->setBackgroundRole(ColorRole::Trough);
            addChild(p.lineEditBase);

            auto weak = std::weak_ptr<LineEdit>(std::dynamic_pointer_cast<LineEdit>(shared_from_this()));
            p.lineEditBase->setFocusCallback(
                [weak](bool)
            {
                if (auto widget = weak.lock())
                {
                    widget->_redraw();
                }
            });
        }

        LineEdit::LineEdit() :
            _p(new Private)
        {}

        LineEdit::~LineEdit()
        {}

        std::shared_ptr<LineEdit> LineEdit::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<LineEdit>(new LineEdit);
            out->_init(context);
            return out;
        }

        const std::string & LineEdit::getText() const
        {
            return _p->lineEditBase->getText();
        }

        void LineEdit::setText(const std::string & value)
        {
            _p->lineEditBase->setText(value);
        }

        ColorRole LineEdit::getTextColorRole() const
        {
            return _p->lineEditBase->getTextColorRole();
        }

        MetricsRole LineEdit::getTextSizeRole() const
        {
            return _p->lineEditBase->getTextSizeRole();
        }

        void LineEdit::setTextColorRole(ColorRole value)
        {
            _p->lineEditBase->setTextColorRole(value);
        }

        void LineEdit::setTextSizeRole(MetricsRole value)
        {
            _p->lineEditBase->setTextSizeRole(value);
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

        const std::string & LineEdit::getSizeString() const
        {
            return _p->lineEditBase->getSizeString();
        }

        void LineEdit::setSizeString(const std::string & value)
        {
            _p->lineEditBase->setSizeString(value);
        }

        void LineEdit::setTextChangedCallback(const std::function<void(const std::string &)> & callback)
        {
            _p->lineEditBase->setTextChangedCallback(callback);
        }

        void LineEdit::setTextEditCallback(const std::function<void(const std::string&, TextEditReason)> & callback)
        {
            _p->lineEditBase->setTextEditCallback(callback);
        }

        void LineEdit::setFocusCallback(const std::function<void(bool)> & callback)
        {
            _p->lineEditBase->setFocusCallback(callback);
        }

        void LineEdit::setBackgroundRole(ColorRole value)
        {
            DJV_PRIVATE_PTR();
            p.lineEditBase->setBackgroundRole(value);
        }

        std::shared_ptr<Widget> LineEdit::getFocusWidget()
        {
            return _p->lineEditBase->getFocusWidget();
        }
        
        void LineEdit::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            glm::vec2 size = p.lineEditBase->getMinimumSize();
            const auto& style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border);
            _setMinimumSize(size + b * 6.F + getMargin().getSize(style));
        }

        void LineEdit::_layoutEvent(Event::Layout & event)
        {
            DJV_PRIVATE_PTR();
            const BBox2f g = getGeometry();
            const auto& style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border);
            p.lineEditBase->setGeometry(g.margin(-b * 3.F));
        }

        void LineEdit::_paintEvent(Event::Paint& event)
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

    } // namespace UI
} // namespace djv
