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
                std::shared_ptr<Grid> layout;
                size_t labelID = 0;
                std::map<size_t, std::shared_ptr<Label>> labelIDToLabel;
            };

            void Form::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Form");

                DJV_PRIVATE_PTR();
                p.layout = Grid::create(context);
                p.layout->setParent(shared_from_this());
            }

            Form::Form() :
                _p(new Private)
            {}

            Form::~Form()
            {}

            std::shared_ptr<Form> Form::create(Context * context)
            {
                auto out = std::shared_ptr<Form>(new Form);
                out->_init(context);
                return out;
            }

            void Form::addWidget(const std::shared_ptr<Widget>& value)
            {
                DJV_PRIVATE_PTR();
                const glm::ivec2 gridSize = p.layout->getGridSize();
                p.layout->addWidget(value, glm::ivec2(0, gridSize.y));
            }

            size_t Form::addWidget(const std::string & text, const std::shared_ptr<Widget>& value)
            {
                auto label = Label::create(text, getContext());
                label->setTextHAlign(TextHAlign::Left);
                DJV_PRIVATE_PTR();
                const glm::ivec2 gridSize = p.layout->getGridSize();
                p.layout->addWidget(label, glm::ivec2(0, gridSize.y));
                p.layout->addWidget(value, glm::ivec2(1, gridSize.y));
                ++p.labelID;
                p.labelIDToLabel[p.labelID] = label;
                return p.labelID;
            }

            void Form::removeWidget(const std::shared_ptr<Widget>& value)
            {
                _p->layout->removeWidget(value);
            }

            void Form::clearWidgets()
            {
                _p->layout->clearWidgets();
            }

            void Form::setText(size_t id, const std::string & text)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.labelIDToLabel.find(id);
                if (i != p.labelIDToLabel.end())
                {
                    i->second->setText(text);
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

            float Form::getHeightForWidth(float value) const
            {
                float out = 0.f;
                if (auto style = _getStyle().lock())
                {
                    out = _p->layout->getHeightForWidth(value - getMargin().getWidth(style)) + getMargin().getWidth(style);
                }
                return out;
            }

            void Form::_preLayoutEvent(Event::PreLayout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
                }
            }

            void Form::_layoutEvent(Event::Layout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    DJV_PRIVATE_PTR();
                    const BBox2f & g = getGeometry();
                    _p->layout->setGeometry(getMargin().bbox(g, style));
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
