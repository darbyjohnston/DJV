// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUIComponents/FileBrowserSettings.h>

#include <djvUI/ActionButton.h>
#include <djvUI/Label.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct SettingsWidget::Private
            {
                std::shared_ptr<Label> titleLabel;
                std::shared_ptr<Label> viewTypeLabel;
                std::shared_ptr<Label> thumbnailSizeLabel;
                std::shared_ptr<IntSlider> thumbnailSizeSlider;
                std::shared_ptr<Label> miscSettingsLabel;
                std::shared_ptr<VerticalLayout> layout;

                std::shared_ptr<ValueObserver<AV::Image::Size> > thumbnailSizeObserver;
            };

            void SettingsWidget::_init(
                const std::map<std::string, std::shared_ptr<Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::FileBrowser::SettingsWidget");

                p.titleLabel = Label::create(context);
                p.titleLabel->setTextHAlign(TextHAlign::Left);
                p.titleLabel->setMargin(MetricsRole::MarginSmall);
                p.titleLabel->setBackgroundRole(ColorRole::Trough);

                p.viewTypeLabel = Label::create(context);
                p.viewTypeLabel->setTextHAlign(TextHAlign::Left);
                p.viewTypeLabel->setMargin(MetricsRole::MarginSmall);
                p.viewTypeLabel->setBackgroundRole(ColorRole::Trough);
                auto tilesButton = ActionButton::create(context);
                tilesButton->addAction(actions.at("Tiles"));
                auto listButton = ActionButton::create(context);
                listButton->addAction(actions.at("List"));

                p.thumbnailSizeLabel = Label::create(context);
                p.thumbnailSizeLabel->setTextHAlign(TextHAlign::Left);
                p.thumbnailSizeLabel->setMargin(MetricsRole::MarginSmall);
                p.thumbnailSizeLabel->setBackgroundRole(ColorRole::Trough);
                auto increaseThumbnailSizeButton = ActionButton::create(context);
                increaseThumbnailSizeButton->addAction(actions.at("IncreaseThumbnailSize"));
                auto decreaseThumbnailSizeButton = ActionButton::create(context);
                decreaseThumbnailSizeButton->addAction(actions.at("DecreaseThumbnailSize"));

                p.miscSettingsLabel = Label::create(context);
                p.miscSettingsLabel->setTextHAlign(TextHAlign::Left);
                p.miscSettingsLabel->setMargin(MetricsRole::MarginSmall);
                p.miscSettingsLabel->setBackgroundRole(ColorRole::Trough);
                auto fileSequencesButton = ActionButton::create(context);
                fileSequencesButton->addAction(actions.at("FileSequences"));
                auto showHiddenButton = ActionButton::create(context);
                showHiddenButton->addAction(actions.at("ShowHidden"));

                p.thumbnailSizeSlider = IntSlider::create(context);
                p.thumbnailSizeSlider->setRange(thumbnailSizeRange);
                p.thumbnailSizeSlider->setDelay(Time::getTime(Time::TimerValue::Medium));
                p.thumbnailSizeSlider->setMargin(MetricsRole::MarginSmall);

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addChild(p.titleLabel);
                p.layout->addSeparator();
                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->addChild(p.viewTypeLabel);
                vLayout->addSeparator();
                auto vLayout2 = VerticalLayout::create(context);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(tilesButton);
                vLayout2->addChild(listButton);
                vLayout->addChild(vLayout2);
                vLayout->addSeparator();
                vLayout->addChild(p.thumbnailSizeLabel);
                vLayout->addSeparator();
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(increaseThumbnailSizeButton);
                vLayout2->addChild(decreaseThumbnailSizeButton);
                vLayout2->addChild(p.thumbnailSizeSlider);
                vLayout->addChild(vLayout2);
                vLayout->addSeparator();
                vLayout->addChild(p.miscSettingsLabel);
                vLayout->addSeparator();
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(fileSequencesButton);
                vLayout2->addChild(showHiddenButton);
                vLayout->addChild(vLayout2);
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                scrollWidget->setMinimumSizeRole(MetricsRole::None);
                scrollWidget->addChild(vLayout);
                p.layout->addChild(scrollWidget);
                p.layout->setStretch(scrollWidget, RowStretch::Expand);
                addChild(p.layout);

                auto contextWeak = std::weak_ptr<Context>(context);
                p.thumbnailSizeSlider->setValueCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setThumbnailSize(AV::Image::Size(value, ceilf(value / 2.F)));
                            }
                        }
                    });

                auto settingsSystem = context->getSystemT<Settings::System>();
                auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
                p.thumbnailSizeObserver = ValueObserver<AV::Image::Size>::create(
                    fileBrowserSettings->observeThumbnailSize(),
                    [weak](const AV::Image::Size& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->thumbnailSizeSlider->setValue(value.w);
                        }
                    });
            }

            SettingsWidget::SettingsWidget() :
                _p(new Private)
            {}

            SettingsWidget::~SettingsWidget()
            {}

            std::shared_ptr<SettingsWidget> SettingsWidget::create(
                const std::map<std::string, std::shared_ptr<Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
                out->_init(actions, context);
                return out;
            }

            void SettingsWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void SettingsWidget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void SettingsWidget::_initEvent(Event::Init& event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.titleLabel->setText(_getText(DJV_TEXT("file_browser_settings")));
                    p.viewTypeLabel->setText(_getText(DJV_TEXT("file_browser_view")));
                    p.thumbnailSizeLabel->setText(_getText(DJV_TEXT("file_browser_thumbnail_size")));
                    p.miscSettingsLabel->setText(_getText(DJV_TEXT("file_browser_miscellaneous")));
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
