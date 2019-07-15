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

#include <djvUI/EventSystem.h>

#include <djvUI/Style.h>
#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>
#include <djvCore/Timer.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct EventSystem::Private
        {
            std::vector<std::weak_ptr<Window> > windows;
            std::weak_ptr<Window> currentWindow;
            std::shared_ptr<Time::Timer> statsTimer;
        };

        void EventSystem::_init(const std::string & name, Core::Context * context)
        {
            IEventSystem::_init(name, context);

            DJV_PRIVATE_PTR();
            addDependency(context->getSystemT<UI::UISystem>());

            p.statsTimer = Time::Timer::create(context);
            p.statsTimer->setRepeating(true);
            auto weak = std::weak_ptr<EventSystem>(std::dynamic_pointer_cast<EventSystem>(shared_from_this()));
            p.statsTimer->start(
                Time::getMilliseconds(Time::TimerValue::VerySlow),
                [weak](float)
            {
                if (auto system = weak.lock())
                {
                    std::stringstream s;
                    s << "Global widget count: " << Widget::getGlobalWidgetCount();
                    system->_log(s.str());
                }
            });
        }

        EventSystem::EventSystem() :
            _p(new Private)
        {}

        EventSystem::~EventSystem()
        {}

        const std::vector<std::weak_ptr<Window> > & EventSystem::getWindows() const
        {
            return _p->windows;
        }

        const std::weak_ptr<Window> EventSystem::getCurrentWindow() const
        {
            return _p->currentWindow;
        }

        void EventSystem::tick(float dt)
        {
            IEventSystem::tick(dt);
            DJV_PRIVATE_PTR();
            if (auto uiSystem = getContext()->getSystemT<UISystem>())
            {
                auto style = uiSystem->getStyle();
                if (style->isDirty())
                {
                    Event::Style styleEvent;
                    for (auto i : p.windows)
                    {
                        if (auto window = i.lock())
                        {
                            _styleRecursive(window, styleEvent);
                        }
                    }
                    style->setClean();
                }
            }
        }

        void EventSystem::_pushClipRect(const Core::BBox2f &)
        {}

        void EventSystem::_popClipRect()
        {}

        bool EventSystem::_resizeRequest(const std::shared_ptr<Widget> & widget) const
        {
            bool out = widget->_resizeRequest;
            widget->_resizeRequest = false;
            return out;
        }

        bool EventSystem::_redrawRequest(const std::shared_ptr<Widget> & widget) const
        {
            bool out = widget->_redrawRequest;
            widget->_redrawRequest = false;
            return out;
        }

        void EventSystem::_styleRecursive(const std::shared_ptr<Widget> & widget, Event::Style & event)
        {
            widget->event(event);
            for (const auto & child : widget->getChildWidgets())
            {
                _styleRecursive(child, event);
            }
        }

        void EventSystem::_preLayoutRecursive(const std::shared_ptr<Widget> & widget, Event::PreLayout & event)
        {
            for (const auto & child : widget->getChildWidgets())
            {
                _preLayoutRecursive(child, event);
            }
            widget->event(event);
        }

        void EventSystem::_layoutRecursive(const std::shared_ptr<Widget> & widget, Event::Layout & event)
        {
            if (widget->isVisible())
            {
                widget->event(event);
                for (const auto & child : widget->getChildWidgets())
                {
                    _layoutRecursive(child, event);
                }
            }
        }

        void EventSystem::_clipRecursive(const std::shared_ptr<Widget> & widget, Event::Clip & event)
        {
            widget->event(event);
            const BBox2f clipRect = event.getClipRect();
            for (const auto & child : widget->getChildWidgets())
            {
                event.setClipRect(clipRect.intersect(child->getGeometry()));
                _clipRecursive(child, event);
            }
            event.setClipRect(clipRect);
        }

        void EventSystem::_paintRecursive(
            const std::shared_ptr<Widget> & widget,
            Event::Paint & event,
            Event::PaintOverlay& overlayEvent)
        {
            if (widget->isVisible() && !widget->isClipped())
            {
                const BBox2f clipRect = event.getClipRect();
                _pushClipRect(clipRect);
                widget->event(event);
                for (const auto & child : widget->getChildWidgets())
                {
                    const BBox2f childClipRect = clipRect.intersect(child->getGeometry());
                    event.setClipRect(childClipRect);
                    overlayEvent.setClipRect(childClipRect);
                    _paintRecursive(child, event, overlayEvent);
                }
                widget->event(overlayEvent);
                _popClipRect();
                event.setClipRect(clipRect);
                overlayEvent.setClipRect(clipRect);
            }
        }

        void EventSystem::_initObject(const std::shared_ptr<IObject> & object)
        {
            IEventSystem::_initObject(object);
            DJV_PRIVATE_PTR();
            if (auto window = std::dynamic_pointer_cast<Window>(object))
            {
                p.windows.push_back(window);
                p.currentWindow = window;
            }
            if (auto widget = std::dynamic_pointer_cast<Widget>(object))
            {
                Event::Style styleEvent;
                object->event(styleEvent);
            }
        }

    } // namespace UI
} // namespace djv
