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
            std::shared_ptr<Time::Timer> statsTimer;
        };

        namespace
        {
            /*void getClassNames(const std::shared_ptr<IObject>& object, std::map<std::string, size_t>& out)
            {
                const std::string& className = object->getClassName();
                const auto i = out.find(className);
                if (i == out.end())
                {
                    out[className] = 0;
                }
                out[className]++;
                for (const auto& j : object->getChildren())
                {
                    getClassNames(j, out);
                }
            }*/
        
        } // namespace
        
        void EventSystem::_init(const std::string & name, const std::shared_ptr<Core::Context>& context)
        {
            IEventSystem::_init(name, context);

            DJV_PRIVATE_PTR();
            addDependency(context->getSystemT<UI::UISystem>());

            p.statsTimer = Time::Timer::create(context);
            p.statsTimer->setRepeating(true);
            auto weak = std::weak_ptr<EventSystem>(std::dynamic_pointer_cast<EventSystem>(shared_from_this()));
            p.statsTimer->start(
                Time::getTime(Time::TimerValue::VerySlow),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Unit&)
            {
                if (auto system = weak.lock())
                {
                    std::stringstream ss;
                    ss << "Global widget count: " << Widget::getGlobalWidgetCount();
                    system->_log(ss.str());
                    
                    /*std::map<std::string, size_t> classNames;
                    getClassNames(system->getRootObject(), classNames);
                    for (const auto& i : classNames)
                    {
                        std::stringstream ss;
                        ss << i.first << " count: " << i.second;
                        system->_log(ss.str());
                    }*/
                }
            });
        }

        EventSystem::EventSystem() :
            _p(new Private)
        {}

        EventSystem::~EventSystem()
        {}

        void EventSystem::tick(const std::chrono::steady_clock::time_point& t, const Time::Unit& dt)
        {
            IEventSystem::tick(t, dt);
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto uiSystem = context->getSystemT<UISystem>();
                auto style = uiSystem->getStyle();
                if (style->isDirty())
                {
                    Event::Init initEvent;
                    for (auto i : p.windows)
                    {
                        if (auto window = i.lock())
                        {
                            _initRecursive(window, initEvent);
                        }
                    }
                    style->setClean();
                }
                auto i = p.windows.begin();
                while (i != p.windows.end())
                {
                    bool erase = false;
                    if (auto window = i->lock())
                    {
                        if (window->isClosed())
                        {
                            getRootObject()->removeChild(window);
                            erase = true;
                        }
                    }
                    else
                    {
                        erase = true;
                    }
                    if (erase)
                    {
                        i = p.windows.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
            }
        }

        void EventSystem::_pushClipRect(const Core::BBox2f &)
        {
            // Default implementation does nothing.
        }

        void EventSystem::_popClipRect()
        {
            // Default implementation does nothing.
        }

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

        void EventSystem::_initLayoutRecursive(const std::shared_ptr<Widget>& widget, Event::InitLayout& event)
        {
            for (const auto& child : widget->getChildWidgets())
            {
                _initLayoutRecursive(child, event);
            }
            widget->event(event);
        }

        void EventSystem::_preLayoutRecursive(const std::shared_ptr<Widget>& widget, Event::PreLayout& event)
        {
            for (const auto& child : widget->getChildWidgets())
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
                setTextFocus(nullptr);
                getRootObject()->addChild(window);
                p.windows.push_back(window);
            }
        }

    } // namespace UI
} // namespace djv
