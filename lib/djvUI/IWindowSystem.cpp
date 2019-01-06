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

#include <djvUI/IWindowSystem.h>

#include <djvUI/Window.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        class RootObject : public IObject
        {};

        struct IWindowSystem::Private
        {
        };

        void IWindowSystem::_init(const std::string & name, Core::Context * context)
        {
            ISystem::_init(name, context);
        }

        IWindowSystem::IWindowSystem() :
            _p(new Private)
        {}

        IWindowSystem::~IWindowSystem()
        {}

        void IWindowSystem::_pushClipRect(const Core::BBox2f &)
        {}

        void IWindowSystem::_popClipRect()
        {}

        void IWindowSystem::_hasResizeRequest(const std::shared_ptr<UI::Widget>& widget, bool& out) const
        {
            if (widget->_resizeRequest)
            {
                widget->_resizeRequest = false;
                out = true;
            }
        }

        void IWindowSystem::_hasRedrawRequest(const std::shared_ptr<UI::Widget>& widget, bool& out) const
        {
            if (widget->_redrawRequest)
            {
                widget->_redrawRequest = false;
                out = true;
            }
        }

        void IWindowSystem::_preLayoutRecursive(const std::shared_ptr<UI::Widget>& widget, Event::PreLayout& event)
        {
            for (const auto& child : widget->getChildrenT<UI::Widget>())
            {
                _preLayoutRecursive(child, event);
            }
            widget->event(event);
        }

        void IWindowSystem::_layoutRecursive(const std::shared_ptr<UI::Widget>& widget, Event::Layout& event)
        {
            if (widget->isVisible())
            {
                widget->event(event);
                for (const auto& child : widget->getChildrenT<UI::Widget>())
                {
                    _layoutRecursive(child, event);
                }
            }
        }

        void IWindowSystem::_clipRecursive(const std::shared_ptr<UI::Widget>& widget, Event::Clip& event)
        {
            widget->event(event);
            const BBox2f clipRect = event.getClipRect();
            for (const auto& child : widget->getChildrenT<UI::Widget>())
            {
                event.setClipRect(clipRect.intersect(child->getGeometry()));
                _clipRecursive(child, event);
            }
            event.setClipRect(clipRect);
        }

        void IWindowSystem::_paintRecursive(const std::shared_ptr<UI::Widget>& widget, Event::Paint& event)
        {
            if (widget->isVisible() && !widget->isClipped())
            {
                const BBox2f clipRect = event.getClipRect();
                _pushClipRect(clipRect);
                widget->event(event);
                for (const auto& child : widget->getChildrenT<UI::Widget>())
                {
                    event.setClipRect(clipRect.intersect(child->getGeometry()));
                    _paintRecursive(child, event);
                }
                _popClipRect();
                event.setClipRect(clipRect);
            }
        }

    } // namespace UI
} // namespace djv
