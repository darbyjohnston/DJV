// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
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
            struct Menu::Private
            {
                std::map<std::string, std::shared_ptr<Label> > labels;
                std::shared_ptr<IntSlider> thumbnailSizeSlider;
                std::shared_ptr<ScrollWidget> scrollWidget;

                std::shared_ptr<ValueObserver<AV::Image::Size> > thumbnailSizeObserver;
            };

            void Menu::_init(
                const std::map<std::string, std::shared_ptr<Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::FileBrowser::Menu");

                p.labels["View"] = Label::create(context);
                auto tilesButton = ActionButton::create(context);
                tilesButton->addAction(actions.at("Tiles"));
                auto listButton = ActionButton::create(context);
                listButton->addAction(actions.at("List"));

                p.labels["Thumbnails"] = Label::create(context);
                auto increaseThumbnailSizeButton = ActionButton::create(context);
                increaseThumbnailSizeButton->addAction(actions.at("IncreaseThumbnailSize"));
                auto decreaseThumbnailSizeButton = ActionButton::create(context);
                decreaseThumbnailSizeButton->addAction(actions.at("DecreaseThumbnailSize"));
                p.thumbnailSizeSlider = IntSlider::create(context);
                p.thumbnailSizeSlider->setRange(thumbnailSizeRange);
                p.thumbnailSizeSlider->setDelay(Time::getTime(Time::TimerValue::Medium));

                p.labels["Misc"] = Label::create(context);
                auto fileSequencesButton = ActionButton::create(context);
                fileSequencesButton->addAction(actions.at("FileSequences"));
                auto showHiddenButton = ActionButton::create(context);
                showHiddenButton->addAction(actions.at("ShowHidden"));

                for (const auto& i : p.labels)
                {
                    i.second->setTextHAlign(TextHAlign::Left);
                    i.second->setMargin(MetricsRole::MarginSmall);
                    i.second->setBackgroundRole(ColorRole::Trough);
                }

                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->addChild(p.labels["View"]);
                vLayout->addSeparator();
                auto vLayout2 = VerticalLayout::create(context);
                vLayout2->setMargin(MetricsRole::MarginSmall);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(tilesButton);
                vLayout2->addChild(listButton);
                vLayout->addChild(vLayout2);
                vLayout->addSeparator();
                vLayout->addChild(p.labels["Thumbnails"]);
                vLayout->addSeparator();
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setMargin(MetricsRole::MarginSmall);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(increaseThumbnailSizeButton);
                vLayout2->addChild(decreaseThumbnailSizeButton);
                vLayout2->addChild(p.thumbnailSizeSlider);
                vLayout->addChild(vLayout2);
                vLayout->addSeparator();
                vLayout->addChild(p.labels["Misc"]);
                vLayout->addSeparator();
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setMargin(MetricsRole::MarginSmall);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(fileSequencesButton);
                vLayout2->addChild(showHiddenButton);
                vLayout->addChild(vLayout2);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->setMinimumSizeRole(MetricsRole::None);
                p.scrollWidget->setBackgroundRole(ColorRole::Background);
                p.scrollWidget->addChild(vLayout);
                addChild(p.scrollWidget);

                auto contextWeak = std::weak_ptr<Context>(context);
                p.thumbnailSizeSlider->setValueCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setThumbnailSize(AV::Image::Size(value, ceilf(value / 2.F)));
                        }
                    });

                auto settingsSystem = context->getSystemT<Settings::System>();
                auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                auto weak = std::weak_ptr<Menu>(std::dynamic_pointer_cast<Menu>(shared_from_this()));
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

            Menu::Menu() :
                _p(new Private)
            {}

            Menu::~Menu()
            {}

            std::shared_ptr<Menu> Menu::create(
                const std::map<std::string, std::shared_ptr<Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Menu>(new Menu);
                out->_init(actions, context);
                return out;
            }

            void Menu::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->scrollWidget->getMinimumSize());
            }

            void Menu::_layoutEvent(Event::Layout& event)
            {
                _p->scrollWidget->setGeometry(getGeometry());
            }

            void Menu::_initEvent(Event::Init& event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.labels["View"]->setText(_getText(DJV_TEXT("file_browser_settings_view")));
                    p.labels["Thumbnails"]->setText(_getText(DJV_TEXT("file_browser_settings_thumbnails")));
                    p.labels["Misc"]->setText(_getText(DJV_TEXT("file_browser_settings_miscellaneous")));
                    p.thumbnailSizeSlider->setTooltip(_getText(DJV_TEXT("file_browser_settings_thumbnail_size_tooltip")));
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
