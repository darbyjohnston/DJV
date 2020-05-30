// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotateSystem.h>

#include <djvViewApp/Annotate.h>
#include <djvViewApp/AnnotateSettings.h>
#include <djvViewApp/AnnotateWidget.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Style.h>
#include <djvUI/UISystem.h>

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
            
            bool currentTool = false;
            AnnotateTool tool = AnnotateTool::First;
            AnnotateLineSize lineSize = AnnotateLineSize::First;
            std::vector<AV::Image::Color> colors;
            int currentColor = -1;
            glm::vec2 imagePos = glm::vec2(0.F, 0.F);
            float imageZoom = 1.F;
            glm::vec2 hoverPos = glm::vec2(0.F, 0.F);
            glm::vec2 dragStart = glm::vec2(0.F, 0.F);
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<AnnotatePrimitive> currentPrimitive;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> toolActionGroup;
            std::shared_ptr<UI::ActionGroup> lineSizeActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::weak_ptr<AnnotateWidget> widget;
            
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<AnnotateTool> > toolObserver;
            std::shared_ptr<ValueObserver<AnnotateLineSize> > lineSizeObserver;
            std::shared_ptr<ListObserver<AV::Image::Color> > colorsObserver;
            std::shared_ptr<ValueObserver<int> > currentColorObserver;
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

            p.actions["Polyline"] = UI::Action::create();
            p.actions["Polyline"]->setIcon("djvIconAnnotatePolyline");
            p.actions["Line"] = UI::Action::create();
            p.actions["Line"]->setIcon("djvIconAnnotateLine");
            p.actions["Rectangle"] = UI::Action::create();
            p.actions["Rectangle"]->setIcon("djvIconAnnotateRectangle");
            p.actions["Ellipse"] = UI::Action::create();
            p.actions["Ellipse"]->setIcon("djvIconAnnotateEllipse");
            p.toolActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.toolActionGroup->addAction(p.actions["Polyline"]);
            p.toolActionGroup->addAction(p.actions["Line"]);
            p.toolActionGroup->addAction(p.actions["Rectangle"]);
            p.toolActionGroup->addAction(p.actions["Ellipse"]);

            p.actions["LineSizeSmall"] = UI::Action::create();
            p.actions["LineSizeSmall"]->setIcon("djvIconAnnotateLineSizeSmall");
            p.actions["LineSizeMedium"] = UI::Action::create();
            p.actions["LineSizeMedium"]->setIcon("djvIconAnnotateLineSizeMedium");
            p.actions["LineSizeLarge"] = UI::Action::create();
            p.actions["LineSizeLarge"]->setIcon("djvIconAnnotateLineSizeLarge");
            p.lineSizeActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.lineSizeActionGroup->addAction(p.actions["LineSizeSmall"]);
            p.lineSizeActionGroup->addAction(p.actions["LineSizeMedium"]);
            p.lineSizeActionGroup->addAction(p.actions["LineSizeLarge"]);

            p.actions["Clear"] = UI::Action::create();
            p.actions["Clear"]->setIcon("djvIconClear");

            p.actions["Add"] = UI::Action::create();
            p.actions["Add"]->setIcon("djvIconAdd");
            p.actions["Delete"] = UI::Action::create();
            p.actions["Delete"]->setIcon("djvIconClose");
            p.actions["DeleteAll"] = UI::Action::create();
            p.actions["Next"] = UI::Action::create();
            p.actions["Next"]->setIcon("djvIconArrowRight");
            p.actions["Prev"] = UI::Action::create();
            p.actions["Prev"]->setIcon("djvIconArrowLeft");

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Polyline"]);
            p.menu->addAction(p.actions["Line"]);
            p.menu->addAction(p.actions["Rectangle"]);
            p.menu->addAction(p.actions["Ellipse"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Clear"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Add"]);
            p.menu->addAction(p.actions["Delete"]);
            p.menu->addAction(p.actions["DeleteAll"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Next"]);
            p.menu->addAction(p.actions["Prev"]);

            _textUpdate();

            auto weak = std::weak_ptr<AnnotateSystem>(std::dynamic_pointer_cast<AnnotateSystem>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.toolActionGroup->setRadioCallback(
                [contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
                    annotateSettings->setTool(static_cast<AnnotateTool>(value));
                }
            });

            p.lineSizeActionGroup->setRadioCallback(
                [contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
                    annotateSettings->setLineSize(static_cast<AnnotateLineSize>(value));
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
            p.toolObserver = ValueObserver<AnnotateTool>::create(
                annotateSettings->observeTool(),
                [weak](AnnotateTool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->tool = value;
                    system->_p->toolActionGroup->setChecked(static_cast<size_t>(system->_p->tool));
                }
            });

            p.lineSizeObserver = ValueObserver<AnnotateLineSize>::create(
                annotateSettings->observeLineSize(),
                [weak](AnnotateLineSize value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->lineSize = value;
                    system->_p->lineSizeActionGroup->setChecked(static_cast<size_t>(system->_p->lineSize));
                }
            });

            p.colorsObserver = ListObserver<AV::Image::Color>::create(
                annotateSettings->observeColors(),
                [weak](const std::vector<AV::Image::Color>& value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->colors = value;
                }
            });

            p.currentColorObserver = ValueObserver<int>::create(
                annotateSettings->observeCurrentColor(),
                [weak](int value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->currentColor = value;
                }
            });

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
                                    system->_p->activeWidget->getViewWidget()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imagePos = value;
                                        }
                                    });

                                system->_p->imageZoomObserver = ValueObserver<float>::create(
                                    system->_p->activeWidget->getViewWidget()->observeImageZoom(),
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

        void AnnotateSystem::setCurrentTool(bool value, int index)
        {
            DJV_PRIVATE_PTR();
            p.currentTool = value;
            if (value)
            {
                if (auto context = getContext().lock())
                {
                    if (p.widget.expired())
                    {
                        auto widget = AnnotateWidget::create(p.actions, context);
                        p.widget = widget;
                        _openWidget("Annotate", widget);
                    }
                }
            }
            else if (-1 == index)
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
                "H"
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
                p.actions["Polyline"]->setText(_getText(DJV_TEXT("menu_annotate_polyline")));
                p.actions["Polyline"]->setTooltip(_getText(DJV_TEXT("menu_annotate_polyline_tooltip")));
                p.actions["Line"]->setText(_getText(DJV_TEXT("menu_annotate_line")));
                p.actions["Line"]->setTooltip(_getText(DJV_TEXT("menu_annotate_line_tooltip")));
                p.actions["Rectangle"]->setText(_getText(DJV_TEXT("menu_annotate_rectangle")));
                p.actions["Rectangle"]->setTooltip(_getText(DJV_TEXT("menu_annotate_rectangle_tooltip")));
                p.actions["Ellipse"]->setText(_getText(DJV_TEXT("menu_annotate_ellipse")));
                p.actions["Ellipse"]->setTooltip(_getText(DJV_TEXT("menu_annotate_ellipse_tooltip")));
                p.actions["Clear"]->setText(_getText(DJV_TEXT("menu_annotate_clear")));
                p.actions["Clear"]->setTooltip(_getText(DJV_TEXT("menu_annotate_clear_tooltip")));
                p.actions["Add"]->setText(_getText(DJV_TEXT("menu_annotate_add")));
                p.actions["Add"]->setTooltip(_getText(DJV_TEXT("menu_annotate_add_tooltip")));
                p.actions["Delete"]->setText(_getText(DJV_TEXT("menu_annotate_delete")));
                p.actions["Delete"]->setTooltip(_getText(DJV_TEXT("menu_annotate_delete_tooltip")));
                p.actions["DeleteAll"]->setText(_getText(DJV_TEXT("menu_annotate_delete_all")));
                p.actions["DeleteAll"]->setTooltip(_getText(DJV_TEXT("menu_annotate_delete_all_tooltip")));
                p.actions["Next"]->setText(_getText(DJV_TEXT("menu_annotate_next")));
                p.actions["Next"]->setTooltip(_getText(DJV_TEXT("menu_annotate_next_tooltip")));
                p.actions["Prev"]->setText(_getText(DJV_TEXT("menu_annotate_prev")));
                p.actions["Prev"]->setTooltip(_getText(DJV_TEXT("menu_annotate_prev_tooltip")));

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
                    options.color = p.currentColor >= 0 && p.currentColor < p.colors.size() ? p.colors[p.currentColor] : AV::Image::Color();
                    auto uiSystem = context->getSystemT<UI::UISystem>();
                    const auto& style = uiSystem->getStyle();
                    options.lineSize = getAnnotateLineSize(p.lineSize) * style->getScale();
                    switch (p.tool)
                    {
                    case AnnotateTool::Polyline:
                        p.currentPrimitive = AnnotatePolyline::create(options, context);
                        break;
                    case AnnotateTool::Line:
                        p.currentPrimitive = AnnotateLine::create(options, context);
                        break;
                    case AnnotateTool::Rectangle:
                        p.currentPrimitive = AnnotateRectangle::create(options, context);
                        break;
                    case AnnotateTool::Ellipse:
                        p.currentPrimitive = AnnotateEllipse::create(options, context);
                        break;
                    default: break;
                    }
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

