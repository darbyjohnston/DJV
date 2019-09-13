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

#include <djvUI/SoloLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Solo::Private
            {
                int currentIndex = -1;
                SoloMinimumSize soloMinimumSize = SoloMinimumSize::Both;
            };

            void Solo::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Solo");
            }

            Solo::Solo() :
                _p(new Private)
            {}

            Solo::~Solo()
            {}

            std::shared_ptr<Solo> Solo::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Solo>(new Solo);
                out->_init(context);
                return out;
            }

            int Solo::getCurrentIndex() const
            {
                return _p->currentIndex;
            }

            void Solo::setCurrentIndex(int value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.currentIndex)
                    return;
                p.currentIndex = value;
                _resize();
            }

            void Solo::setCurrentIndex(int value, Side side)
            {
                DJV_PRIVATE_PTR();
                if (value == p.currentIndex)
                    return;
                p.currentIndex = value;
                _resize();
            }

            std::shared_ptr<Widget> Solo::getCurrentWidget() const
            {
                const auto& children = getChildWidgets();
                DJV_PRIVATE_PTR();
                if (p.currentIndex >= 0 && p.currentIndex < static_cast<int>(children.size()))
                {
                    return children[p.currentIndex];
                }
                return nullptr;
            }

            void Solo::setCurrentWidget(const std::shared_ptr<Widget>& value)
            {
                int i = 0;
                for (const auto& child : getChildWidgets())
                {
                    if (value == child)
                    {
                        setCurrentIndex(i);
                        break;
                    }
                    ++i;
                }
            }

            void Solo::setCurrentWidget(const std::shared_ptr<Widget>& value, Side)
            {
                int i = 0;
                for (const auto& child : getChildWidgets())
                {
                    if (value == child)
                    {
                        setCurrentIndex(i);
                        break;
                    }
                    ++i;
                }
            }

            SoloMinimumSize Solo::getSoloMinimumSize() const
            {
                return _p->soloMinimumSize;
            }

            void Solo::setSoloMinimumSize(SoloMinimumSize value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.soloMinimumSize)
                    return;
                p.soloMinimumSize = value;
                _resize();
            }

            float Solo::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.f;
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                for (const auto & child : getChildWidgets())
                {
                    if (child->isVisible() ||
                        SoloMinimumSize::Vertical == p.soloMinimumSize ||
                        SoloMinimumSize::Both == p.soloMinimumSize)
                    {
                        out = std::max(out, child->getHeightForWidth(value - m.x));
                    }
                }
                out += m.y;
                return out;
            }

            void Solo::addChild(const std::shared_ptr<IObject> & value)
            {
                Widget::addChild(value);
                DJV_PRIVATE_PTR();
                if (-1 == p.currentIndex)
                {
                    p.currentIndex = 0;
                }
            }

            void Solo::removeChild(const std::shared_ptr<IObject> & value)
            {
                Widget::removeChild(value);
                if (0 == getChildren().size())
                {
                    _p->currentIndex = -1;
                }
            }

            void Solo::clearChildren()
            {
                Widget::clearChildren();
            }

            void Solo::_preLayoutEvent(Event::PreLayout &)
            {
                DJV_PRIVATE_PTR();
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                for (const auto & child : getChildWidgets())
                {
                    if (child->isVisible() || p.soloMinimumSize != SoloMinimumSize::None)
                    {
                        const glm::vec2& childMinimumSize = child->getMinimumSize();
                        switch (p.soloMinimumSize)
                        {
                        case Horizontal: minimumSize.x = glm::max(minimumSize.x, childMinimumSize.x); break;
                        case Vertical:   minimumSize.y = glm::max(minimumSize.y, childMinimumSize.y); break;
                        case Both:       minimumSize   = glm::max(minimumSize, child->getMinimumSize()); break;
                        default: break;
                        }
                    }
                }
                const auto& style = _getStyle();
                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }

            void Solo::_layoutEvent(Event::Layout &)
            {
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                for (const auto & child : getChildWidgets())
                {
                    child->setGeometry(Widget::getAlign(g, child->getMinimumSize(), child->getHAlign(), child->getVAlign()));
                }
            }

            void Solo::_updateEvent(Event::Update & event)
            {
                int index = 0;
                const auto& children = getChildWidgets();
                for (const auto & child : children)
                {
                    child->setVisible(_p->currentIndex == index);
                    ++index;
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
