// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotateSystem.h>

#include <djvViewApp/Annotate.h>
#include <djvViewApp/AnnotateSettings.h>
#include <djvViewApp/AnnotateWidget.h>
#include <djvViewApp/ImageView.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateSystem::Private
        {
            std::shared_ptr<AnnotateSettings> settings;
            
            AV::Image::Color color = AV::Image::Color(1.F, 0.F, 0.F);
            float lineWidth = 10.F;
            glm::vec2 imagePos = glm::vec2(0.F, 0.F);
            float imageZoom = 1.F;
            bool currentTool = false;
            glm::vec2 hoverPos = glm::vec2(0.F, 0.F);
            glm::vec2 dragStart = glm::vec2(0.F, 0.F);
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<AnnotatePrimitive> currentPrimitive;
            
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<AnnotateWidget> widget;
            
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<glm::vec2> > imagePosObserver;
            std::shared_ptr<ValueObserver<float> > imageZoomObserver;
            std::shared_ptr<ValueObserver<PointerData> > hoverObserver;
            std::shared_ptr<ValueObserver<PointerData> > dragObserver;
        };

        void AnnotateSystem::_init(const std::shared_ptr<Context>& context)
        {
            IToolSystem::_init("djv::ViewApp::AnnotateSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = AnnotateSettings::create(context);
            _setWidgetGeom(p.settings->getWidgetGeom());

            p.actions["Annotate"] = UI::Action::create();
            p.actions["Annotate"]->setIcon("djvIconAnnotate");
            p.actions["Annotate"]->setShortcut(GLFW_KEY_A, UI::Shortcut::getSystemModifier());

            p.menu = UI::Menu::create(context);

            _textUpdate();

            auto weak = std::weak_ptr<AnnotateSystem>(std::dynamic_pointer_cast<AnnotateSystem>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak, contextWeak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->imagePosObserver = ValueObserver<glm::vec2>::create(
                                    system->_p->activeWidget->getImageView()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imagePos = value;
                                        }
                                    });

                                system->_p->imageZoomObserver = ValueObserver<float>::create(
                                    system->_p->activeWidget->getImageView()->observeImageZoom(),
                                    [weak](float value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imageZoom = value;
                                        }
                                    });

                                system->_p->hoverObserver = ValueObserver<PointerData>::create(
                                    system->_p->activeWidget->observeHover(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->hoverPos = value.pos;
                                        }
                                    });
                                system->_p->dragObserver = ValueObserver<PointerData>::create(
                                    system->_p->activeWidget->observeDrag(),
                                    [weak, contextWeak](const PointerData& value)
                                    {
                                        if (auto context = contextWeak.lock())
                                        {
                                            if (auto system = weak.lock())
                                            {
                                                if (system->_p->currentTool && system->_p->activeWidget)
                                                {
                                                    switch (value.state)
                                                    {
                                                    case PointerState::Start:
                                                        system->_dragStart(value.pos);
                                                        break;
                                                    case PointerState::Move:
                                                        system->_dragMove(value.pos);
                                                        break;
                                                    case PointerState::End:
                                                        system->_dragEnd(value.pos);
                                                        break;
                                                    default: break;
                                                    }
                                                }
                                            }
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->imagePosObserver.reset();
                                system->_p->imageZoomObserver.reset();
                                system->_p->hoverObserver.reset();
                                system->_p->dragObserver.reset();
                            }
                        }
                    });
            }
        }

        AnnotateSystem::AnnotateSystem() :
            _p(new Private)
        {}

        AnnotateSystem::~AnnotateSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("Annotate");
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

        std::shared_ptr<AnnotateSystem> AnnotateSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<AnnotateSystem>(new AnnotateSystem);
            out->_init(context);
            return out;
        }

        ToolActionData AnnotateSystem::getToolAction() const
        {
            return
            {
                _p->actions["Annotate"],
                "G"
            };
        }

        void AnnotateSystem::setCurrentTool(bool value)
        {
            DJV_PRIVATE_PTR();
            p.currentTool = value;
            if (value)
            {
                if (auto context = getContext().lock())
                {
                    auto widget = AnnotateWidget::create(context);
                    widget->setColor(p.color);
                    widget->setLineWidth(p.lineWidth);
                    auto weak = std::weak_ptr<AnnotateSystem>(std::dynamic_pointer_cast<AnnotateSystem>(shared_from_this()));
                    widget->setColorCallback(
                        [weak](const AV::Image::Color& value)
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->color = value;
                            }
                        });
                    widget->setLineWidthCallback(
                        [weak](float value)
                        {
                            if (auto system = weak.lock())
                            {
                                system->_p->lineWidth = value;
                            }
                        });
                    p.widget = widget;
                    _openWidget("Annotate", widget);
                }
            }
            else
            {
                _closeWidget("Annotate");
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > AnnotateSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData AnnotateSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "G"
            };
        }

        void AnnotateSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            p.widget.reset();
            IToolSystem::_closeWidget(value);
        }

        void AnnotateSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Annotate"]->setText(_getText(DJV_TEXT("menu_annotate")));
                p.actions["Annotate"]->setTooltip(_getText(DJV_TEXT("menu_annotate_tooltip")));
            
                p.menu->setText(_getText(DJV_TEXT("menu_annotate")));
            }
        }
        
        glm::vec2 AnnotateSystem::_xformDrag(const glm::vec2& value) const
        {
            DJV_PRIVATE_PTR();
            return (value - p.imagePos) / p.imageZoom;
        }
        
        void AnnotateSystem::_dragStart(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (p.activeWidget)
                {
                    p.dragStart = value;
                    AnnotateOptions options;
                    options.color = p.color;
                    options.lineWidth = p.lineWidth;
                    p.currentPrimitive = AnnotateLine::create(options, context);
                    p.currentPrimitive->addPoint(_xformDrag(value));
                    p.activeWidget->getMedia()->addAnnotation(p.currentPrimitive);
                }
            }
        }
        
        void AnnotateSystem::_dragMove(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (p.currentPrimitive)
            {
                p.currentPrimitive->addPoint(_xformDrag(value));
            }
        }
        
        void AnnotateSystem::_dragEnd(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            p.currentPrimitive.reset();
        }

    } // namespace ViewApp
} // namespace djv

