// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotateWidget.h>

#include <djvViewApp/Annotate.h>
#include <djvViewApp/AnnotateSettings.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/MultiStateButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/TabWidget.h>
#include <djvUI/TextEdit.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class ColorButton : public UI::Button::IButton
            {
                DJV_NON_COPYABLE(ColorButton);

            protected:
                void _init(const AV::Image::Color&, const std::shared_ptr<Context>&);
                ColorButton();

            public:
                virtual ~ColorButton();

                static std::shared_ptr<ColorButton> create(const AV::Image::Color&, const std::shared_ptr<Context>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                AV::Image::Color _color;
            };

            void ColorButton::_init(const AV::Image::Color& color, const std::shared_ptr<Context>& context)
            {
                IButton::_init(context);
                setButtonType(UI::ButtonType::Radio);
                _color = color;
            }

            ColorButton::ColorButton()
            {}

            ColorButton::~ColorButton()
            {}

            std::shared_ptr<ColorButton> ColorButton::create(const AV::Image::Color& color, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ColorButton>(new ColorButton);
                out->_init(color, context);
                return out;
            }

            void ColorButton::_preLayoutEvent(Event::PreLayout&)
            {
                const auto& style = _getStyle();
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                const float sw = style->getMetric(UI::MetricsRole::SwatchSmall);
                _setMinimumSize(glm::vec2(sw, sw) + m * 2.F);
            }

            void ColorButton::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const BBox2f& g = getGeometry();
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                const auto& render = _getRender();

                if (_isToggled())
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Checked));
                    render->drawRect(g);
                }

                const BBox2f g2 = g.margin(-m);
                render->setFillColor(_color);
                render->drawRect(g2);

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

        } // namespace

        struct AnnotateWidget::Private
        {
            std::vector<AV::Image::Color> colors;
            int currentColor = -1;

            std::shared_ptr<UI::ButtonGroup> colorButtonGroup;
            std::shared_ptr<UI::ToolBar> colorToolBar;
            std::shared_ptr<UI::TextEdit> noteTextEdit;
            std::shared_ptr<UI::ScrollWidget> listWidget;
            std::shared_ptr<UI::MultiStateButton> listButton;
            std::shared_ptr<UI::VerticalLayout> notesLayout;
            std::shared_ptr<UI::VerticalLayout> summaryLayout;
            std::shared_ptr<UI::VerticalLayout> exportLayout;
            std::shared_ptr<UI::TabWidget> tabWidget;

            std::function<void(const AV::Image::Color&)> colorCallback;
            std::function<void(float)> lineWidthCallback;

            std::shared_ptr<ListObserver<AV::Image::Color> > colorsObserver;
            std::shared_ptr<ValueObserver<int> > currentColorObserver;
        };

        void AnnotateWidget::_init(
            std::map<std::string, std::shared_ptr<UI::Action> >& actions,
            const std::shared_ptr<Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::ViewApp::AnnotateWidget");

            p.colorButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            
            p.noteTextEdit = UI::TextEdit::create(context);
            p.noteTextEdit->setBorder(false);

            p.listWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.listWidget->setBorder(false);

            p.listButton = UI::MultiStateButton::create(context);
            p.listButton->setVAlign(UI::VAlign::Center);
            p.listButton->addIcon("djvIconArrowUp");
            p.listButton->addIcon("djvIconArrowDown");
            
            p.notesLayout = UI::VerticalLayout::create(context);
            p.notesLayout->setSpacing(UI::MetricsRole::None);
            auto toolBar = UI::ToolBar::create(context);
            toolBar->addAction(actions["Polyline"]);
            toolBar->addAction(actions["Line"]);
            toolBar->addAction(actions["Rectangle"]);
            toolBar->addAction(actions["Ellipse"]);
            toolBar->addSeparator();
            toolBar->addAction(actions["Clear"]);
            p.notesLayout->addChild(toolBar);
            p.notesLayout->addSeparator();
            toolBar = UI::ToolBar::create(context);
            toolBar->addAction(actions["LineSizeSmall"]);
            toolBar->addAction(actions["LineSizeMedium"]);
            toolBar->addAction(actions["LineSizeLarge"]);
            p.notesLayout->addChild(toolBar);
            p.notesLayout->addSeparator();
            p.colorToolBar = UI::ToolBar::create(context);
            p.notesLayout->addChild(p.colorToolBar);
            p.notesLayout->addChild(p.noteTextEdit);
            p.notesLayout->setStretch(p.noteTextEdit, UI::RowStretch::Expand);
            p.notesLayout->addSeparator();
            p.notesLayout->addChild(p.listWidget);
            p.notesLayout->setStretch(p.listWidget, UI::RowStretch::Expand);
            p.notesLayout->addSeparator();
            toolBar = UI::ToolBar::create(context);
            toolBar->setBackgroundRole(UI::ColorRole::Background);
            toolBar->addAction(actions["Add"]);
            toolBar->addAction(actions["Delete"]);
            toolBar->addAction(actions["Prev"]);
            toolBar->addAction(actions["Next"]);
            toolBar->addExpander();
            toolBar->addChild(p.listButton);
            p.notesLayout->addChild(toolBar);
            
            p.summaryLayout = UI::VerticalLayout::create(context);
            p.summaryLayout->setMargin(UI::MetricsRole::MarginSmall);
            p.summaryLayout->setSpacing(UI::MetricsRole::SpacingSmall);

            p.exportLayout = UI::VerticalLayout::create(context);
            p.exportLayout->setMargin(UI::MetricsRole::MarginSmall);
            p.exportLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            
            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setBackgroundRole(UI::ColorRole::Background);
            p.tabWidget->addChild(p.notesLayout);
            p.tabWidget->addChild(p.summaryLayout);
            p.tabWidget->addChild(p.exportLayout);
            addChild(p.tabWidget);
            
            auto contextWeak = std::weak_ptr<Context>(context);
            p.colorButtonGroup->setRadioCallback(
                [contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
                    annotateSettings->setCurrentColor(value);
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto annotateSettings = settingsSystem->getSettingsT<AnnotateSettings>();
            auto weak = std::weak_ptr<AnnotateWidget>(std::dynamic_pointer_cast<AnnotateWidget>(shared_from_this()));
            p.colorsObserver = ListObserver<AV::Image::Color>::create(
                annotateSettings->observeColors(),
                [weak](const std::vector<AV::Image::Color>& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->colors = value;
                    widget->_widgetUpdate();
                }
            });

            p.currentColorObserver = ValueObserver<int>::create(
                annotateSettings->observeCurrentColor(),
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->currentColor = value;
                    widget->_widgetUpdate();
                }
            });
        }

        AnnotateWidget::AnnotateWidget() :
            _p(new Private)
        {}

        AnnotateWidget::~AnnotateWidget()
        {}

        std::shared_ptr<AnnotateWidget> AnnotateWidget::create(
            std::map<std::string, std::shared_ptr<UI::Action> >& actions,
            const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<AnnotateWidget>(new AnnotateWidget);
            out->_init(actions, context);
            return out;
        }
        
        void AnnotateWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_annotate")));

                for (const auto& i : p.colorButtonGroup->getButtons())
                {
                    i->setTooltip(_getText(DJV_TEXT("annotate_color_tooltip")));
                }

                p.listButton->setTooltip(_getText(DJV_TEXT("annotate_list_show_tooltip")));

                p.tabWidget->setText(p.notesLayout, _getText(DJV_TEXT("annotate_tab_notes")));
                p.tabWidget->setText(p.summaryLayout, _getText(DJV_TEXT("annotate_tab_summary")));
                p.tabWidget->setText(p.exportLayout, _getText(DJV_TEXT("annotate_tab_export")));
            }
        }

        void AnnotateWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.colorButtonGroup->clearButtons();
                p.colorToolBar->clearChildren();
                for (const auto& i : p.colors)
                {
                    auto button = ColorButton::create(i, context);
                    p.colorButtonGroup->addButton(button);
                    p.colorToolBar->addChild(button);
                }
                p.colorButtonGroup->setChecked(p.currentColor);
            }
        }

    } // namespace ViewApp
} // namespace djv

