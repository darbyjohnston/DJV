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

#include <djvViewApp/AnnotateWidget.h>

#include <djvViewApp/Annotate.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/MultiStateButton.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TabWidget.h>
#include <djvUI/TextEdit.h>
#include <djvUI/ToolButton.h>
#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateWidget::Private
        {
            std::shared_ptr<UI::ButtonGroup> typeButtonGroup;
            std::shared_ptr<UI::ColorPickerSwatch> colorPickerSwatch;
            std::shared_ptr<UI::FloatEdit> lineWidthEdit;
            std::shared_ptr<UI::ToolButton> clearButton;
            std::shared_ptr<UI::TextEdit> noteTextEdit;
            std::shared_ptr<UI::ScrollWidget> listWidget;
            std::shared_ptr<UI::ToolButton> addButton;
            std::shared_ptr<UI::ToolButton> deleteButton;
            std::shared_ptr<UI::ToolButton> prevButton;
            std::shared_ptr<UI::ToolButton> nextButton;
            std::shared_ptr<UI::MultiStateButton> listButton;
            
            std::shared_ptr<UI::VerticalLayout> editLayout;
            std::shared_ptr<UI::VerticalLayout> summaryLayout;
            std::shared_ptr<UI::VerticalLayout> exportLayout;
            std::shared_ptr<UI::TabWidget> tabWidget;
            std::function<void(const AV::Image::Color&)> colorCallback;
            std::function<void(float)> lineWidthCallback;
        };

        void AnnotateWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::ViewApp::AnnotateWidget");

            p.typeButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            const std::vector<std::string> typeIcons =
            {
                "djvIconAnnotatePolyline",
                "djvIconAnnotateLine",
                "djvIconAnnotateRectangle",
                "djvIconAnnotateEllipse",
            };
            const auto& typeEnums = getAnnotateTypeEnums();
            for (size_t i = 0; i < typeEnums.size(); ++i)
            {
                auto button = UI::ToolButton::create(context);
                button->setIcon(typeIcons[i]);
                button->setVAlign(UI::VAlign::Center);
                p.typeButtonGroup->addButton(button);
            }

            p.colorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.colorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.colorPickerSwatch->setVAlign(UI::VAlign::Center);
            
            p.lineWidthEdit = UI::FloatEdit::create(context);
            p.lineWidthEdit->setRange(FloatRange(1.F, 100.F));
            auto model = p.lineWidthEdit->getModel();
            model->setSmallIncrement(1.F);
            model->setLargeIncrement(10.F);
            
            p.clearButton = UI::ToolButton::create(context);
            p.clearButton->setIcon("djvIconClear");
            p.clearButton->setVAlign(UI::VAlign::Center);
            
            p.noteTextEdit = UI::TextEdit::create(context);
            p.noteTextEdit->setBorder(false);

            p.listWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.listWidget->setBorder(false);
            
            p.addButton = UI::ToolButton::create(context);
            p.addButton->setIcon("djvIconAddSmall");
            p.addButton->setVAlign(UI::VAlign::Center);

            p.deleteButton = UI::ToolButton::create(context);
            p.deleteButton->setIcon("djvIconCloseSmall");
            p.deleteButton->setVAlign(UI::VAlign::Center);

            p.prevButton = UI::ToolButton::create(context);
            p.prevButton->setIcon("djvIconArrowSmallLeft");
            p.prevButton->setVAlign(UI::VAlign::Center);

            p.nextButton = UI::ToolButton::create(context);
            p.nextButton->setIcon("djvIconArrowSmallRight");
            p.nextButton->setVAlign(UI::VAlign::Center);

            p.listButton = UI::MultiStateButton::create(context);
            p.listButton->setVAlign(UI::VAlign::Center);
            p.listButton->addIcon("djvIconArrowSmallUp");
            p.listButton->addIcon("djvIconArrowSmallDown");
            
            p.editLayout = UI::VerticalLayout::create(context);
            p.editLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            for (const auto& i : p.typeButtonGroup->getButtons())
            {
                hLayout->addChild(i);
            }
            hLayout->addChild(p.colorPickerSwatch);
            hLayout->addChild(p.lineWidthEdit);
            hLayout->addChild(p.clearButton);
            p.editLayout->addChild(hLayout);
            p.editLayout->addSeparator();
            p.editLayout->addChild(p.noteTextEdit);
            p.editLayout->setStretch(p.noteTextEdit, UI::RowStretch::Expand);
            p.editLayout->addSeparator();
            p.editLayout->addChild(p.listWidget);
            p.editLayout->setStretch(p.listWidget, UI::RowStretch::Expand);
            p.editLayout->addSeparator();
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addChild(p.addButton);
            hLayout->addChild(p.deleteButton);
            hLayout->addChild(p.prevButton);
            hLayout->addChild(p.nextButton);
            hLayout->addExpander();
            hLayout->addChild(p.listButton);
            p.editLayout->addChild(hLayout);
            
            p.summaryLayout = UI::VerticalLayout::create(context);
            p.summaryLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.summaryLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));

            p.exportLayout = UI::VerticalLayout::create(context);
            p.exportLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.exportLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            
            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setBackgroundRole(UI::ColorRole::Background);
            p.tabWidget->addChild(p.editLayout);
            p.tabWidget->addChild(p.summaryLayout);
            p.tabWidget->addChild(p.exportLayout);
            addChild(p.tabWidget);
            
            auto weak = std::weak_ptr<AnnotateWidget>(std::dynamic_pointer_cast<AnnotateWidget>(shared_from_this()));
            p.colorPickerSwatch->setColorCallback(
                [weak](const AV::Image::Color& value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->colorCallback)
                        {
                            widget->_p->colorCallback(value);
                        }
                    }
                });
            
            p.lineWidthEdit->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->lineWidthCallback)
                        {
                            widget->_p->lineWidthCallback(value);
                        }
                    }
                });
        }

        AnnotateWidget::AnnotateWidget() :
            _p(new Private)
        {}

        AnnotateWidget::~AnnotateWidget()
        {}

        std::shared_ptr<AnnotateWidget> AnnotateWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateWidget>(new AnnotateWidget);
            out->_init(context);
            return out;
        }

        void AnnotateWidget::setColor(const AV::Image::Color& value)
        {
            _p->colorPickerSwatch->setColor(value);
        }
        
        void AnnotateWidget::setColorCallback(const std::function<void(const AV::Image::Color)>& value)
        {
            _p->colorCallback = value;
        }
        
        void AnnotateWidget::setLineWidth(float value)
        {
            _p->lineWidthEdit->setValue(value);
        }

        void AnnotateWidget::setLineWidthCallback(const std::function<void(float)>& value)
        {
            _p->lineWidthCallback = value;
        }
        
        void AnnotateWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            
            setTitle(_getText(DJV_TEXT("Annotate")));
            
            const std::vector<std::string> typeTooltips =
            {
                DJV_TEXT("Annotate polyline tooltip"),
                DJV_TEXT("Annotate line tooltip"),
                DJV_TEXT("Annotate rectangle tooltip"),
                DJV_TEXT("Annotate ellipse tooltip")
            };
            const auto& typeEnums = getAnnotateTypeEnums();
            auto buttons = p.typeButtonGroup->getButtons();
            for (size_t i = 0; i < typeEnums.size(); ++i)
            {
                buttons[i]->setTooltip(_getText(typeTooltips[i]));
            }
            
            p.colorPickerSwatch->setTooltip(_getText(DJV_TEXT("Annotate color picker tooltip")));

            p.lineWidthEdit->setTooltip(_getText(DJV_TEXT("Annotate line width tooltip")));

            p.clearButton->setTooltip(_getText(DJV_TEXT("Annotate clear tooltip")));

            p.addButton->setTooltip(_getText(DJV_TEXT("Annotate add tooltip")));
            p.deleteButton->setTooltip(_getText(DJV_TEXT("Annotate delete tooltip")));

            p.prevButton->setTooltip(_getText(DJV_TEXT("Annotate previous tooltip")));
            p.nextButton->setTooltip(_getText(DJV_TEXT("Annotate next tooltip")));

            p.listButton->setTooltip(_getText(DJV_TEXT("Annotate list toggle tooltip")));
            
            p.tabWidget->setText(p.editLayout, _getText(DJV_TEXT("Edit")));
            p.tabWidget->setText(p.summaryLayout, _getText(DJV_TEXT("Summary")));
            p.tabWidget->setText(p.exportLayout, _getText(DJV_TEXT("Export")));
        }

    } // namespace ViewApp
} // namespace djv

