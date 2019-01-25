//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct LineEdit::Private
        {
            std::shared_ptr<Layout::Border> border;
        };

        void LineEdit::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::LineEdit");
            setBackgroundRole(Style::ColorRole::Trough);
            setVAlign(VAlign::Center);

            DJV_PRIVATE_PTR();
            p.border = Layout::Border::create(context);
            p.border->setParent(shared_from_this());
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

        void LineEdit::setBorder(bool value)
        {
            _p->border->setBorderSize(value ? Style::MetricsRole::Border : Style::MetricsRole::None);
        }
        
        float LineEdit::getHeightForWidth(float value) const
        {
            float out = 0.f;
            if (auto style = _getStyle().lock())
            {
                out = _p->border->getHeightForWidth(value) +
                    getMargin().getHeight(style);
            }
            return out;
        }
        
        void LineEdit::_preLayoutEvent(Event::PreLayout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const float tc = style->getMetric(Style::MetricsRole::TextColumn);
                const float i = style->getMetric(Style::MetricsRole::Icon);
                glm::vec2 size = _p->border->getMinimumSize();
                size.x = std::max(size.x, tc);
                size.y = std::max(size.y, i);
                _setMinimumSize(size + getMargin().getSize(style));
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
