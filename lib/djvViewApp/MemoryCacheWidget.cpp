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

#include <djvViewApp/MemoryCacheWidget.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>

#include <djvUI/CheckBox.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/OS.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MemoryCacheWidget::Private
        {
            float percentageUsed = 0.F;

            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::CheckBox> enabledCheckBox;
            std::shared_ptr<UI::IntSlider> maxGBSlider;
            std::shared_ptr<UI::Label> maxGBLabel;
            std::shared_ptr<UI::Label> percentageLabel;
            std::shared_ptr<UI::Label> percentageLabel2;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<ValueObserver<bool> > enabledObserver;
            std::shared_ptr<ValueObserver<int> > maxGBObserver;
            std::shared_ptr<ValueObserver<float> > percentageObserver;
        };

        void MemoryCacheWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MemoryCacheWidget");

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.titleLabel->setBackgroundRole(UI::ColorRole::Trough);

            p.enabledCheckBox = UI::CheckBox::create(context);

            p.maxGBSlider = UI::IntSlider::create(context);
            p.maxGBSlider->setRange(IntRange(1, OS::getRAMSize() / Memory::gigabyte));
            p.maxGBLabel = UI::Label::create(context);
            p.maxGBLabel->setTextHAlign(UI::TextHAlign::Left);

            p.percentageLabel = UI::Label::create(context);
            p.percentageLabel->setTextHAlign(UI::TextHAlign::Left);
            p.percentageLabel2 = UI::Label::create(context);
            p.percentageLabel2->setFont(AV::Font::familyMono);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.layout->addChild(p.titleLabel);
            p.layout->addSeparator();
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            vLayout->addChild(p.enabledCheckBox);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            hLayout->addChild(p.maxGBSlider);
            hLayout->setStretch(p.maxGBSlider, UI::RowStretch::Expand);
            hLayout->addChild(p.maxGBLabel);
            vLayout->addChild(hLayout);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            hLayout->addChild(p.percentageLabel);
            hLayout->addChild(p.percentageLabel2);
            vLayout->addChild(hLayout);
            p.layout->addChild(vLayout);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.enabledCheckBox->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheEnabled(value);
                        }
                    }
                });
            p.maxGBSlider->setValueCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheMaxGB(value);
                        }
                    }
                });

            auto weak = std::weak_ptr<MemoryCacheWidget>(
                std::dynamic_pointer_cast<MemoryCacheWidget>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.enabledObserver = ValueObserver<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->enabledCheckBox->setChecked(value);
                        }
                    });

                p.maxGBObserver = ValueObserver<int>::create(
                    fileSettings->observeCacheMaxGB(),
                    [weak](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->maxGBSlider->setValue(value);
                        }
                    });
            }

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.percentageObserver = ValueObserver<float>::create(
                    fileSystem->observeCachePercentage(),
                    [weak](float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->percentageUsed = value;
                            widget->_widgetUpdate();
                        }
                    });
            }
        }

        MemoryCacheWidget::MemoryCacheWidget() :
            _p(new Private)
        {}

        MemoryCacheWidget::~MemoryCacheWidget()
        {}

        std::shared_ptr<MemoryCacheWidget> MemoryCacheWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MemoryCacheWidget>(new MemoryCacheWidget);
            out->_init(context);
            return out;
        }

        void MemoryCacheWidget::_preLayoutEvent(Event::PreLayout&)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }
        
        void MemoryCacheWidget::_layoutEvent(Event::Layout&)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void MemoryCacheWidget::_textUpdateEvent(Event::TextUpdate & event)
        {
            _widgetUpdate();
        }

        void MemoryCacheWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.titleLabel->setText(_getText(DJV_TEXT("Memory Cache")));
            p.enabledCheckBox->setText(_getText(DJV_TEXT("Enable")));
            p.maxGBLabel->setText(_getText(DJV_TEXT("GB")));
            p.percentageLabel->setText(_getText(DJV_TEXT("Used")) + ":");
            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << p.percentageUsed;
            ss << "%";
            p.percentageLabel2->setText(ss.str());
        }

    } // namespace ViewApp
} // namespace djv

