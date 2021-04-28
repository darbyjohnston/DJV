// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/EventSystem.h>

#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>
#include <djvUI/UISettings.h>
#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvRender2D/RenderSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/Timer.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct EventSystem::Private
        {
            std::shared_ptr<UI::UISystem> uiSystem;
            std::vector<std::weak_ptr<Window> > windows;
            std::vector<std::weak_ptr<Window> > newWindows;
            bool resizeRequest = false;
            bool redrawRequest = false;
            bool textLCDRenderingDirty = false;
            bool tooltips = false;
            std::shared_ptr<Observer::Value<bool> > textLCDRenderingObserver;
            std::shared_ptr<Observer::Value<bool> > tooltipsObserver;
            std::shared_ptr<System::Timer> statsTimer;
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
        
        void EventSystem::_init(const std::string& name, const std::shared_ptr<System::Context>& context)
        {
            IEventSystem::_init(name, context);

            DJV_PRIVATE_PTR();
            p.uiSystem = context->getSystemT<UI::UISystem>();
            addDependency(p.uiSystem);

            auto render2DSystem = context->getSystemT<Render2D::RenderSystem>();
            auto weak = std::weak_ptr<EventSystem>(std::dynamic_pointer_cast<EventSystem>(shared_from_this()));
            p.textLCDRenderingObserver = Observer::Value<bool>::create(
                render2DSystem->observeTextLCDRendering(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->textLCDRenderingDirty = true;
                    }
                });

            auto settingsSystem = context->getSystemT<Settings::SettingsSystem>();
            auto uiSettings = settingsSystem->getSettingsT<Settings::UI>();
            p.tooltipsObserver = Observer::Value<bool>::create(
                uiSettings->observeTooltips(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->tooltips = value;
                    }
                });

            p.statsTimer = System::Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                System::getTimerDuration(System::TimerValue::VerySlow),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
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

        void EventSystem::resizeRequest()
        {
            _p->resizeRequest = true;
        }

        void EventSystem::redrawRequest()
        {
            _p->redrawRequest = true;
        }

        bool EventSystem::areTooltipsEnabled() const
        {
            return _p->tooltips;
        }

        void EventSystem::tick()
        {
            IEventSystem::tick();
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                const auto& style = p.uiSystem->getStyle();
                bool redraw = style->isPaletteDirty();
                bool resize = style->isSizeDirty();
                bool font = p.textLCDRenderingDirty || style->isFontDirty();
                p.textLCDRenderingDirty = false;
                if (redraw || resize || font)
                {
                    System::Event::InitData data;
                    data.redraw = redraw;
                    data.resize = resize;
                    data.font = font;
                    System::Event::Init initEvent(data);
                    for (auto i : p.windows)
                    {
                        if (auto window = i.lock())
                        {
                            _initRecursive(window, initEvent);
                        }
                    }
                    style->setClean();
                }

                if (!p.newWindows.empty())
                {
                    auto newWindows = std::move(p.newWindows);
                    System::Event::InitData data;
                    data.redraw = true;
                    data.resize = true;
                    data.font = true;
                    data.text = true;
                    System::Event::Init initEvent(data);
                    for (const auto& i : newWindows)
                    {
                        if (auto window = i.lock())
                        {
                            _initRecursive(window, initEvent);
                            p.windows.push_back(window);
                        }
                    }
                }

                auto i = p.windows.begin();
                while (i != p.windows.end())
                {
                    bool erase = false;
                    if (auto window = i->lock())
                    {
                        if (window->isClosed())
                        {
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

        const std::vector<std::weak_ptr<Window> >& EventSystem::_getWindows() const
        {
            return _p->windows;
        }
        
        void EventSystem::_addWindow(const std::shared_ptr<Window>& value)
        {
            setTextFocus(nullptr);
            _p->newWindows.push_back(value);
            _p->resizeRequest = true;
            _p->redrawRequest = true;
        }

        bool EventSystem::_resizeRequestReset()
        {
            const bool out = _p->resizeRequest;
            _p->resizeRequest = false;
            return out;
        }

        bool EventSystem::_redrawRequestReset()
        {
            const bool out = _p->redrawRequest;
            _p->redrawRequest = false;
            return out;
        }

        void EventSystem::_pushClipRect(const Math::BBox2f&)
        {
            // Default implementation does nothing.
        }

        void EventSystem::_popClipRect()
        {
            // Default implementation does nothing.
        }

        void EventSystem::_initLayoutRecursive(const std::shared_ptr<Widget>& widget, System::Event::InitLayout& event)
        {
            for (const auto& child : widget->getChildWidgets())
            {
                _initLayoutRecursive(child, event);
            }
            widget->event(event);
        }

        void EventSystem::_preLayoutRecursive(const std::shared_ptr<Widget>& widget, System::Event::PreLayout& event)
        {
            for (const auto& child : widget->getChildWidgets())
            {
                _preLayoutRecursive(child, event);
            }
            widget->event(event);
        }

        void EventSystem::_layoutRecursive(const std::shared_ptr<Widget>& widget, System::Event::Layout& event)
        {
            if (widget->isVisible())
            {
                widget->event(event);
                for (const auto& child : widget->getChildWidgets())
                {
                    _layoutRecursive(child, event);
                }
            }
        }

        void EventSystem::_clipRecursive(const std::shared_ptr<Widget>& widget, System::Event::Clip& event)
        {
            widget->event(event);
            const Math::BBox2f clipRect = event.getClipRect();
            for (const auto& child : widget->getChildWidgets())
            {
                event.setClipRect(clipRect.intersect(child->getGeometry()));
                _clipRecursive(child, event);
            }
            event.setClipRect(clipRect);
        }

        void EventSystem::_paintRecursive(
            const std::shared_ptr<Widget>& widget,
            System::Event::Paint& event,
            System::Event::PaintOverlay& overlayEvent)
        {
            if (widget->isVisible() && !widget->isClipped())
            {
                const Math::BBox2f clipRect = event.getClipRect();
                _pushClipRect(clipRect);
                widget->event(event);
                for (const auto& child : widget->getChildWidgets())
                {
                    const Math::BBox2f childClipRect = clipRect.intersect(child->getGeometry());
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

        void EventSystem::_init(System::Event::Init& event)
        {
            for (const auto& i : _p->windows)
            {
                if (auto window = i.lock())
                {
                    _initRecursive(window, event);
                }
            }
        }

        void EventSystem::_update(System::Event::Update& event)
        {
            for (const auto& i : _p->windows)
            {
                if (auto window = i.lock())
                {
                    _updateRecursive(window, event);
                }
            }
        }

    } // namespace UI
} // namespace djv
