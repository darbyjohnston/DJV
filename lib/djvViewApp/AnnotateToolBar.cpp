// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotatePrivate.h>

#include <djvViewApp/AnnotateSettings.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/IconWidget.h>
#include <djvUI/PopupButton.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class AnnotateToolWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(AnnotateToolWidget);

            protected:
                void _init(std::map<std::string, std::shared_ptr<UI::Action> >, const std::shared_ptr<System::Context>&);
                AnnotateToolWidget();

            public:
                ~AnnotateToolWidget() override;

                static std::shared_ptr<AnnotateToolWidget> create(std::map<std::string, std::shared_ptr<UI::Action> >, const std::shared_ptr<System::Context>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                std::shared_ptr<UI::ToolBar> _toolBar;
            };

            void AnnotateToolWidget::_init(std::map<std::string, std::shared_ptr<UI::Action> > actions, const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                _toolBar = UI::ToolBar::create(context);
                _toolBar->addAction(actions["Freehand"]);
                _toolBar->addAction(actions["Arrow"]);
                _toolBar->addAction(actions["Line"]);
                _toolBar->addAction(actions["Rectangle"]);
                _toolBar->addAction(actions["Ellipse"]);
                addChild(_toolBar);
            }

            AnnotateToolWidget::AnnotateToolWidget()
            {}

            AnnotateToolWidget::~AnnotateToolWidget()
            {}

            std::shared_ptr<AnnotateToolWidget> AnnotateToolWidget::create(std::map<std::string, std::shared_ptr<UI::Action> > actions, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<AnnotateToolWidget>(new AnnotateToolWidget);
                out->_init(actions, context);
                return out;
            }

            void AnnotateToolWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_toolBar->getMinimumSize());
            }

            void AnnotateToolWidget::_layoutEvent(System::Event::Layout&)
            {
                _toolBar->setGeometry(getGeometry());
            }

            class AnnotateLineSizeWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(AnnotateLineSizeWidget);

            protected:
                void _init(std::map<std::string, std::shared_ptr<UI::Action> >, const std::shared_ptr<System::Context>&);
                AnnotateLineSizeWidget();

            public:
                ~AnnotateLineSizeWidget() override;

                static std::shared_ptr<AnnotateLineSizeWidget> create(std::map<std::string, std::shared_ptr<UI::Action> >, const std::shared_ptr<System::Context>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                std::shared_ptr<UI::ToolBar> _toolBar;
            };

            void AnnotateLineSizeWidget::_init(std::map<std::string, std::shared_ptr<UI::Action> > actions, const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                _toolBar = UI::ToolBar::create(context);
                _toolBar->addAction(actions["LineSizeSmall"]);
                _toolBar->addAction(actions["LineSizeMedium"]);
                _toolBar->addAction(actions["LineSizeLarge"]);
                addChild(_toolBar);
            }

            AnnotateLineSizeWidget::AnnotateLineSizeWidget()
            {}

            AnnotateLineSizeWidget::~AnnotateLineSizeWidget()
            {}

            std::shared_ptr<AnnotateLineSizeWidget> AnnotateLineSizeWidget::create(std::map<std::string, std::shared_ptr<UI::Action> > actions, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<AnnotateLineSizeWidget>(new AnnotateLineSizeWidget);
                out->_init(actions, context);
                return out;
            }

            void AnnotateLineSizeWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_toolBar->getMinimumSize());
            }

            void AnnotateLineSizeWidget::_layoutEvent(System::Event::Layout&)
            {
                _toolBar->setGeometry(getGeometry());
            }

            class ColorButton : public UI::Button::IButton
            {
                DJV_NON_COPYABLE(ColorButton);

            protected:
                void _init(const Image::Color&, const std::shared_ptr<System::Context>&);
                ColorButton();

            public:
                ~ColorButton() override;

                static std::shared_ptr<ColorButton> create(const Image::Color&, const std::shared_ptr<System::Context>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

            private:
                std::shared_ptr<UI::IconWidget> _iconWidget;
            };

            void ColorButton::_init(const Image::Color& color, const std::shared_ptr<System::Context>& context)
            {
                IButton::_init(context);
                _iconWidget = UI::IconWidget::create(context);
                _iconWidget->setIcon("djvIconSwatch");
                _iconWidget->setIconColorRole(UI::ColorRole::None);
                _iconWidget->setIconColor(color);
                addChild(_iconWidget);
            }

            ColorButton::ColorButton()
            {}

            ColorButton::~ColorButton()
            {}

            std::shared_ptr<ColorButton> ColorButton::create(const Image::Color& color, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<ColorButton>(new ColorButton);
                out->_init(color, context);
                return out;
            }

            void ColorButton::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_iconWidget->getMinimumSize());
            }

            void ColorButton::_layoutEvent(System::Event::Layout&)
            {
                _iconWidget->setGeometry(getGeometry());
            }

            void ColorButton::_paintEvent(System::Event::Paint&)
            {
                const auto& style = _getStyle();
                const Math::BBox2f& g = getGeometry();
                const auto& render = _getRender();
                render->setFillColor(style->getColor(getBackgroundColorRole()));
                render->drawRect(g);
                if (_isPressed())
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Pressed));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Hovered));
                    render->drawRect(g);
                }
            }

            class AnnotateColorWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(AnnotateColorWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                AnnotateColorWidget();

            public:
                ~AnnotateColorWidget() override;

                static std::shared_ptr<AnnotateColorWidget> create(const std::shared_ptr<System::Context>&);

                void setColorCallback(const std::function<void(const Image::Color&)>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                std::vector<Image::Color> _colors;
                std::shared_ptr<UI::ButtonGroup> _buttonGroup;
                std::shared_ptr<UI::ToolBar> _toolBar;
                std::function<void(const Image::Color&)> _colorCallback;
            };

            void AnnotateColorWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                const std::vector<Image::Color> colors =
                {
                    Image::Color(1.F, 0.F, 0.F),
                    Image::Color(0.F, 1.F, 0.F),
                    Image::Color(0.F, 0.F, 1.F),
                    Image::Color(1.F, 1.F, 1.F),
                    Image::Color(0.F, 0.F, 0.F)
                };
                std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
                for (const auto& color : colors)
                {
                    buttons.push_back(ColorButton::create(color, context));
                }
                _buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
                _buttonGroup->setButtons(buttons);

                _toolBar = UI::ToolBar::create(context);
                for (const auto& button : buttons)
                {
                    _toolBar->addChild(button);
                }
                addChild(_toolBar);

                auto weak = std::weak_ptr<AnnotateColorWidget>(std::dynamic_pointer_cast<AnnotateColorWidget>(shared_from_this()));
                _buttonGroup->setRadioCallback(
                    [weak, colors](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_colorCallback && value >= 0 && value < static_cast<int>(colors.size()))
                            {
                                widget->_colorCallback(colors[value]);
                            }
                        }
                    });
            }

            AnnotateColorWidget::AnnotateColorWidget()
            {}

            AnnotateColorWidget::~AnnotateColorWidget()
            {}

            std::shared_ptr<AnnotateColorWidget> AnnotateColorWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<AnnotateColorWidget>(new AnnotateColorWidget);
                out->_init(context);
                return out;
            }

            void AnnotateColorWidget::setColorCallback(const std::function<void(const Image::Color&)>& value)
            {
                _colorCallback = value;
            }

            void AnnotateColorWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_toolBar->getMinimumSize());
            }

            void AnnotateColorWidget::_layoutEvent(System::Event::Layout&)
            {
                _toolBar->setGeometry(getGeometry());
            }

        } // namespace

        struct AnnotateToolBar::Private
        {
            AnnotateTool tool = AnnotateTool::First;
            AnnotateLineSize lineSize = AnnotateLineSize::First;
            Image::Color color;

            std::shared_ptr<UI::PopupButton> toolPopupButton;
            std::shared_ptr<UI::PopupButton> lineSizePopupButton;
            std::shared_ptr<UI::PopupButton> colorPopupButton;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<AnnotateTool> > toolObserver;
            std::shared_ptr<Observer::Value<AnnotateLineSize> > lineSizeObserver;
            std::shared_ptr<Observer::Value<Image::Color> > colorObserver;
        };

        void AnnotateToolBar::_init(
            std::map<std::string, std::shared_ptr<UI::Action> >& actions,
            const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::AnnotateToolBar");

            p.toolPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.toolPopupButton->setPopupIcon("djvIconPopupMenu");

            p.lineSizePopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.lineSizePopupButton->setPopupIcon("djvIconPopupMenu");

            p.colorPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.colorPopupButton->setIcon("djvIconSwatch");
            p.colorPopupButton->setPopupIcon("djvIconPopupMenu");
            p.colorPopupButton->setIconColorRole(UI::ColorRole::None);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            auto toolBar = UI::ToolBar::create(context);
            toolBar->addChild(p.toolPopupButton);
            toolBar->addChild(p.lineSizePopupButton);
            toolBar->addChild(p.colorPopupButton);
            toolBar->addAction(actions["Clear"]);
            toolBar->addSeparator();
            toolBar->addAction(actions["Add"]);
            toolBar->addAction(actions["Prev"]);
            toolBar->addAction(actions["Next"]);
            p.layout->addChild(toolBar);
            p.layout->addSeparator();
            toolBar = UI::ToolBar::create(context);
            toolBar->addExpander();
            toolBar->addChild(UIComponents::SearchBox::create(context));
            p.layout->addChild(toolBar);
            addChild(p.layout);

            _widgetUpdate();

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.toolPopupButton->setOpenCallback(
                [actions, contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = AnnotateToolWidget::create(actions, context);
                    }
                    return out;
                });

            p.lineSizePopupButton->setOpenCallback(
                [actions, contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = AnnotateLineSizeWidget::create(actions, context);
                    }
                    return out;
                });

            p.colorPopupButton->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        auto widget = AnnotateColorWidget::create(context);
                        widget->setColorCallback(
                            [contextWeak](const Image::Color& value)
                            {
                                if (auto context = contextWeak.lock())
                                {
                                    auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                                    auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
                                    annotateSettings->setColor(value);
                                }
                            });
                        out = widget;
                    }
                    return out;
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
            auto weak = std::weak_ptr<AnnotateToolBar>(std::dynamic_pointer_cast<AnnotateToolBar>(shared_from_this()));
            p.toolObserver = Observer::Value<AnnotateTool>::create(
                annotateSettings->observeTool(),
                [weak](AnnotateTool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->tool = value;
                        widget->_widgetUpdate();
                    }
                });

            p.lineSizeObserver = Observer::Value<AnnotateLineSize>::create(
                annotateSettings->observeLineSize(),
                [weak](AnnotateLineSize value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->lineSize = value;
                        widget->_widgetUpdate();
                    }
                });

            p.colorObserver = Observer::Value<Image::Color>::create(
                annotateSettings->observeColor(),
                [weak](const Image::Color& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->color = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        AnnotateToolBar::AnnotateToolBar() :
            _p(new Private)
        {}

        AnnotateToolBar::~AnnotateToolBar()
        {}

        std::shared_ptr<AnnotateToolBar> AnnotateToolBar::create(
            std::map<std::string, std::shared_ptr<UI::Action> >& actions,
            const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateToolBar>(new AnnotateToolBar);
            out->_init(actions, context);
            return out;
        }

        void AnnotateToolBar::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void AnnotateToolBar::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void AnnotateToolBar::_initEvent(System::Event::Init& event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.toolPopupButton->setTooltip(_getText(DJV_TEXT("annotate_tool_tooltip")));
                p.lineSizePopupButton->setTooltip(_getText(DJV_TEXT("annotate_line_size_tooltip")));
                p.colorPopupButton->setTooltip(_getText(DJV_TEXT("annotate_color_tooltip")));
            }
        }

        void AnnotateToolBar::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::string icon;
                switch (p.tool)
                {
                case AnnotateTool::Freehand:
                    icon = "djvIconAnnotateFreehand";
                    break;
                case AnnotateTool::Arrow:
                    icon = "djvIconAnnotateArrow";
                    break;
                case AnnotateTool::Line:
                    icon = "djvIconAnnotateLine";
                    break;
                case AnnotateTool::Rectangle:
                    icon = "djvIconAnnotateRectangle";
                    break;
                case AnnotateTool::Ellipse:
                    icon = "djvIconAnnotateEllipse";
                    break;
                default: break;
                }
                p.toolPopupButton->setIcon(icon);

                switch (p.lineSize)
                {
                case AnnotateLineSize::Small:
                    icon = "djvIconAnnotateLineSizeSmall";
                    break;
                case AnnotateLineSize::Medium:
                    icon = "djvIconAnnotateLineSizeMedium";
                    break;
                case AnnotateLineSize::Large:
                    icon = "djvIconAnnotateLineSizeLarge";
                    break;
                default: break;
                }
                p.lineSizePopupButton->setIcon(icon);

                p.colorPopupButton->setIconColor(p.color);
            }
        }

    } // namespace ViewApp
} // namespace djv

