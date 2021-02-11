// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotateSystem.h>

#include <djvViewApp/AnnotateFunc.h>
#include <djvViewApp/AnnotateSettings.h>
#include <djvViewApp/AnnotatePrivate.h>
#include <djvViewApp/EditSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ToolTitleBar.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutDataFunc.h>
#include <djvUI/Style.h>
#include <djvUI/UISystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

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
            
            AnnotateTool tool = AnnotateTool::First;
            AnnotateLineSize lineSize = AnnotateLineSize::First;
            Image::Color color;
            glm::vec2 imagePos = glm::vec2(0.F, 0.F);
            float imageZoom = 1.F;
            glm::vec2 hoverPos = glm::vec2(0.F, 0.F);
            glm::vec2 dragStart = glm::vec2(0.F, 0.F);
            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<AnnotateCommand> currentCommand;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> toolActionGroup;
            std::shared_ptr<UI::ActionGroup> lineSizeActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<AnnotateWidget> widget;
            
            std::map<std::string, std::shared_ptr<Observer::Value<bool> > > actionObservers;
            std::shared_ptr<Observer::Value<AnnotateTool> > toolObserver;
            std::shared_ptr<Observer::Value<AnnotateLineSize> > lineSizeObserver;
            std::shared_ptr<Observer::Value<Image::Color> > colorObserver;
            std::shared_ptr<Observer::Value<bool> > undoObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<Observer::Value<glm::vec2> > imagePosObserver;
            std::shared_ptr<Observer::Value<float> > imageZoomObserver;
            std::shared_ptr<Observer::Value<PointerData> > hoverObserver;
            std::shared_ptr<Observer::Value<PointerData> > dragObserver;
            std::shared_ptr<Observer::Map<std::string, std::vector<UI::ShortcutData> > > shortcutsObserver;
        };

        void AnnotateSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::AnnotateSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = AnnotateSettings::create(context);

            p.actions["Annotate"] = UI::Action::create();
            p.actions["Annotate"]->setIcon("djvIconAnnotate");

            p.actions["Freehand"] = UI::Action::create();
            p.actions["Freehand"]->setIcon("djvIconAnnotateFreehand");
            p.actions["Arrow"] = UI::Action::create();
            p.actions["Arrow"]->setIcon("djvIconAnnotateArrow");
            p.actions["Line"] = UI::Action::create();
            p.actions["Line"]->setIcon("djvIconAnnotateLine");
            p.actions["Rectangle"] = UI::Action::create();
            p.actions["Rectangle"]->setIcon("djvIconAnnotateRectangle");
            p.actions["Ellipse"] = UI::Action::create();
            p.actions["Ellipse"]->setIcon("djvIconAnnotateEllipse");
            p.toolActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.toolActionGroup->setActions({
                p.actions["Freehand"],
                p.actions["Arrow"],
                p.actions["Line"],
                p.actions["Rectangle"],
                p.actions["Ellipse"] });

            p.actions["LineSizeSmall"] = UI::Action::create();
            p.actions["LineSizeSmall"]->setIcon("djvIconAnnotateLineSizeSmall");
            p.actions["LineSizeMedium"] = UI::Action::create();
            p.actions["LineSizeMedium"]->setIcon("djvIconAnnotateLineSizeMedium");
            p.actions["LineSizeLarge"] = UI::Action::create();
            p.actions["LineSizeLarge"]->setIcon("djvIconAnnotateLineSizeLarge");
            p.lineSizeActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.lineSizeActionGroup->setActions({
                p.actions["LineSizeSmall"],
                p.actions["LineSizeMedium"],
                p.actions["LineSizeLarge"] });

            p.actions["Clear"] = UI::Action::create();
            p.actions["Clear"]->setIcon("djvIconClear");

            p.actions["Add"] = UI::Action::create();
            p.actions["Add"]->setIcon("djvIconAdd");
            p.actions["Next"] = UI::Action::create();
            p.actions["Next"]->setIcon("djvIconArrowRight");
            p.actions["Prev"] = UI::Action::create();
            p.actions["Prev"]->setIcon("djvIconArrowLeft");

            _addShortcut(DJV_TEXT("shortcut_annotate"), GLFW_KEY_A, UI::getSystemModifier());
            _addShortcut(DJV_TEXT("shortcut_annotate_freehand"), 0);
            _addShortcut(DJV_TEXT("shortcut_annotate_arrow"), 0);
            _addShortcut(DJV_TEXT("shortcut_annotate_line"), 0);
            _addShortcut(DJV_TEXT("shortcut_annotate_rectangle"), 0);
            _addShortcut(DJV_TEXT("shortcut_annotate_ellipse"), 0);
            _addShortcut(DJV_TEXT("shortcut_annotate_clear"), 0);
            _addShortcut(DJV_TEXT("shortcut_annotate_add"), 0);
            _addShortcut(DJV_TEXT("shortcut_annotate_next"), 0);
            _addShortcut(DJV_TEXT("shortcut_annotate_prev"), 0);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Freehand"]);
            p.menu->addAction(p.actions["Arrow"]);
            p.menu->addAction(p.actions["Line"]);
            p.menu->addAction(p.actions["Rectangle"]);
            p.menu->addAction(p.actions["Ellipse"]);
            p.menu->addAction(p.actions["Clear"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Add"]);
            p.menu->addAction(p.actions["Next"]);
            p.menu->addAction(p.actions["Prev"]);

            _textUpdate();
            _shortcutsUpdate();

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.toolActionGroup->setRadioCallback(
                [contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                    auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
                    annotateSettings->setTool(static_cast<AnnotateTool>(value));
                }
            });

            p.lineSizeActionGroup->setRadioCallback(
                [contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                    auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
                    annotateSettings->setLineSize(static_cast<AnnotateLineSize>(value));
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
            auto weak = std::weak_ptr<AnnotateSystem>(std::dynamic_pointer_cast<AnnotateSystem>(shared_from_this()));
            p.toolObserver = Observer::Value<AnnotateTool>::create(
                annotateSettings->observeTool(),
                [weak](AnnotateTool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->tool = value;
                    system->_p->toolActionGroup->setChecked(static_cast<size_t>(system->_p->tool));
                }
            });

            p.lineSizeObserver = Observer::Value<AnnotateLineSize>::create(
                annotateSettings->observeLineSize(),
                [weak](AnnotateLineSize value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->lineSize = value;
                    system->_p->lineSizeActionGroup->setChecked(static_cast<size_t>(system->_p->lineSize));
                }
            });

            p.colorObserver = Observer::Value<Image::Color>::create(
                annotateSettings->observeColor(),
                [weak](const Image::Color& value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->color = value;
                }
            });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = Observer::Value<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak, contextWeak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->imagePosObserver = Observer::Value<glm::vec2>::create(
                                    system->_p->activeWidget->getViewWidget()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imagePos = value;
                                        }
                                    });

                                system->_p->imageZoomObserver = Observer::Value<float>::create(
                                    system->_p->activeWidget->getViewWidget()->observeImageZoom(),
                                    [weak](float value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->imageZoom = value;
                                        }
                                    });

                                system->_p->hoverObserver = Observer::Value<PointerData>::create(
                                    system->_p->activeWidget->observeHover(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->hoverPos = value.pos;
                                        }
                                    });
                                system->_p->dragObserver = Observer::Value<PointerData>::create(
                                    system->_p->activeWidget->observeDrag(),
                                    [weak, contextWeak](const PointerData& value)
                                    {
                                        if (auto context = contextWeak.lock())
                                        {
                                            if (auto system = weak.lock())
                                            {
                                                if (system->_p->actions["Annotate"]->observeChecked()->get() &&
                                                    system->_p->activeWidget)
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

            _logInitTime();
        }

        AnnotateSystem::AnnotateSystem() :
            _p(new Private)
        {}

        AnnotateSystem::~AnnotateSystem()
        {}

        std::shared_ptr<AnnotateSystem> AnnotateSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<AnnotateSystem>();
            if (!out)
            {
                out = std::shared_ptr<AnnotateSystem>(new AnnotateSystem);
                out->_init(context);
            }
            return out;
        }

        int AnnotateSystem::getSortKey() const
        {
            return 10;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > AnnotateSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<std::shared_ptr<UI::Menu> > AnnotateSystem::getMenus() const
        {
            return { _p->menu };
        }

        std::vector<std::shared_ptr<UI::Action> > AnnotateSystem::getToolWidgetActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["Annotate"]
            };
        }

        std::vector<std::shared_ptr<UI::Action> > AnnotateSystem::getToolWidgetToolBarActions() const
        {
            DJV_PRIVATE_PTR();
            return
            {
                p.actions["Annotate"]
            };
        }

        ToolWidgetData AnnotateSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            if (auto context = getContext().lock())
            {
                if (value == p.actions["Annotate"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("widget_annotate"), context);

                    auto toolBar = AnnotateToolBar::create(p.actions, context);

                    auto widget = AnnotateWidget::create(context);
                    p.widget = widget;
                    
                    out.titleBar = titleBar;
                    out.toolBar = toolBar;
                    out.widget = widget;
                }
            }
            return out;
        }
        
        void AnnotateSystem::deleteToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.actions["Annotate"])
            {
                if (p.widget)
                {
                    p.widget.reset();
                }
            }
        }

        void AnnotateSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Annotate"]->setText(_getText(DJV_TEXT("menu_annotate")));
                p.actions["Annotate"]->setTooltip(_getText(DJV_TEXT("menu_annotate_tooltip")));
                p.actions["Freehand"]->setText(_getText(DJV_TEXT("menu_annotate_freehand")));
                p.actions["Freehand"]->setTooltip(_getText(DJV_TEXT("menu_annotate_freehand_tooltip")));
                p.actions["Arrow"]->setText(_getText(DJV_TEXT("menu_annotate_arrow")));
                p.actions["Arrow"]->setTooltip(_getText(DJV_TEXT("menu_annotate_arrow_tooltip")));
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
                p.actions["Next"]->setText(_getText(DJV_TEXT("menu_annotate_next")));
                p.actions["Next"]->setTooltip(_getText(DJV_TEXT("menu_annotate_next_tooltip")));
                p.actions["Prev"]->setText(_getText(DJV_TEXT("menu_annotate_prev")));
                p.actions["Prev"]->setTooltip(_getText(DJV_TEXT("menu_annotate_prev_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_annotate")));
            }
        }

        void AnnotateSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Annotate"]->setShortcuts(_getShortcuts("shortcut_annotate"));
                p.actions["Freehand"]->setShortcuts(_getShortcuts("shortcut_annotate_freehand"));
                p.actions["Arrow"]->setShortcuts(_getShortcuts("shortcut_annotate_arrow"));
                p.actions["Line"]->setShortcuts(_getShortcuts("shortcut_annotate_line"));
                p.actions["Rectangle"]->setShortcuts(_getShortcuts("shortcut_annotate_rectangle"));
                p.actions["Ellipse"]->setShortcuts(_getShortcuts("shortcut_annotate_ellipse"));
                p.actions["Clear"]->setShortcuts(_getShortcuts("shortcut_annotate_clear"));
                p.actions["Add"]->setShortcuts(_getShortcuts("shortcut_annotate_add"));
                p.actions["Next"]->setShortcuts(_getShortcuts("shortcut_annotate_next"));
                p.actions["Prev"]->setShortcuts(_getShortcuts("shortcut_annotate_prev"));
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
                    auto uiSystem = context->getSystemT<UI::UISystem>();
                    const auto& style = uiSystem->getStyle();
                    options.lineSize = getAnnotateLineSize(p.lineSize) * style->getScale();
                    std::shared_ptr<AnnotatePrimitive> primitive;
                    switch (p.tool)
                    {
                    case AnnotateTool::Freehand:
                        primitive = AnnotateFreehand::create(options, context);
                        break;
                    case AnnotateTool::Arrow:
                        primitive = AnnotateArrow::create(options, context);
                        break;
                    case AnnotateTool::Line:
                        primitive = AnnotateLine::create(options, context);
                        break;
                    case AnnotateTool::Rectangle:
                        primitive = AnnotateRectangle::create(options, context);
                        break;
                    case AnnotateTool::Ellipse:
                        primitive = AnnotateEllipse::create(options, context);
                        break;
                    default: break;
                    }
                    if (primitive)
                    {
                        primitive->addPoint(_xformDrag(value));
                        auto media = p.activeWidget->getMedia();
                        p.currentCommand = AnnotateCommand::create(primitive, media);

                        auto weak = std::weak_ptr<AnnotateSystem>(std::dynamic_pointer_cast<AnnotateSystem>(shared_from_this()));
                        p.undoObserver = Observer::Value<bool>::create(
                            p.currentCommand->observeUndo(),
                            [weak](bool value)
                            {
                                if (value)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->currentCommand.reset();
                                    }
                                }
                            });

                        media->pushCommand(p.currentCommand);
                    }
                }
            }
        }
        
        void AnnotateSystem::_dragMove(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (auto command = p.currentCommand)
            {
                auto primitive = command->getPrimitive();
                primitive->addPoint(_xformDrag(value));
            }
        }
        
        void AnnotateSystem::_dragEnd(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            p.currentCommand.reset();
        }

    } // namespace ViewApp
} // namespace djv

