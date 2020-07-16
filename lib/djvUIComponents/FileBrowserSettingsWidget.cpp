// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/ShortcutsWidget.h>

#include <djvUI/ActionButton.h>
#include <djvUI/Bellows.h>
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
                std::shared_ptr<IntSlider> thumbnailSizeSlider;
                std::shared_ptr<UI::ShortcutsWidget> keyShortcutsWidget;
                std::map<std::string, std::shared_ptr<Bellows> > bellows;
                std::shared_ptr<ScrollWidget> scrollWidget;

                std::shared_ptr<ValueObserver<AV::Image::Size> > thumbnailSizeObserver;
                std::shared_ptr<MapObserver<std::string, UI::ShortcutDataPair> > keyShortcutsObserver;
            };

            void SettingsWidget::_init(
                const std::map<std::string, std::shared_ptr<Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::FileBrowser::SettingsWidget");

                auto tilesButton = ActionButton::create(context);
                tilesButton->addAction(actions.at("Tiles"));
                auto listButton = ActionButton::create(context);
                listButton->addAction(actions.at("List"));

                auto increaseThumbnailSizeButton = ActionButton::create(context);
                increaseThumbnailSizeButton->addAction(actions.at("IncreaseThumbnailSize"));
                auto decreaseThumbnailSizeButton = ActionButton::create(context);
                decreaseThumbnailSizeButton->addAction(actions.at("DecreaseThumbnailSize"));
                p.thumbnailSizeSlider = IntSlider::create(context);
                p.thumbnailSizeSlider->setRange(thumbnailSizeRange);
                p.thumbnailSizeSlider->setDelay(Time::getTime(Time::TimerValue::Medium));

                p.keyShortcutsWidget = UI::ShortcutsWidget::create(context);
                p.keyShortcutsWidget->setMargin(MetricsRole::Margin);

                auto fileSequencesButton = ActionButton::create(context);
                fileSequencesButton->addAction(actions.at("FileSequences"));
                auto showHiddenButton = ActionButton::create(context);
                showHiddenButton->addAction(actions.at("ShowHidden"));

                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                auto vLayout2 = VerticalLayout::create(context);
                vLayout2->setMargin(MetricsRole::Margin);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(tilesButton);
                vLayout2->addChild(listButton);
                p.bellows["View"] = Bellows::create(context);
                p.bellows["View"]->addChild(vLayout2);
                vLayout->addChild(p.bellows["View"]);
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setMargin(MetricsRole::Margin);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(increaseThumbnailSizeButton);
                vLayout2->addChild(decreaseThumbnailSizeButton);
                vLayout2->addChild(p.thumbnailSizeSlider);
                p.bellows["Thumbnails"] = Bellows::create(context);
                p.bellows["Thumbnails"]->addChild(vLayout2);
                vLayout->addChild(p.bellows["Thumbnails"]);
                p.bellows["Keyboard"] = Bellows::create(context);
                p.bellows["Keyboard"]->addChild(p.keyShortcutsWidget);
                vLayout->addChild(p.bellows["Keyboard"]);
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setMargin(MetricsRole::Margin);
                vLayout2->setSpacing(MetricsRole::None);
                vLayout2->addChild(fileSequencesButton);
                vLayout2->addChild(showHiddenButton);
                p.bellows["Misc"] = Bellows::create(context);
                p.bellows["Misc"]->addChild(vLayout2);
                vLayout->addChild(p.bellows["Misc"]);
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

                p.keyShortcutsWidget->setShortcutsCallback(
                    [contextWeak](const UI::ShortcutDataMap& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setKeyShortcuts(value);
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

                p.keyShortcutsObserver = MapObserver<std::string, UI::ShortcutDataPair>::create(
                    fileBrowserSettings->observeKeyShortcuts(),
                    [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->keyShortcutsWidget->setShortcuts(value);
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

            std::map<std::string, bool> SettingsWidget::getBellowsState() const
            {
                DJV_PRIVATE_PTR();
                std::map<std::string, bool> out;
                for (const auto& i : p.bellows)
                {
                    out[i.first] = i.second->isOpen();
                }
                return out;
            }

            void SettingsWidget::setBellowsState(const std::map<std::string, bool>& value)
            {
                DJV_PRIVATE_PTR();
                for (const auto& i : value)
                {
                    const auto j = p.bellows.find(i.first);
                    if (j != p.bellows.end())
                    {
                        j->second->setOpen(i.second, false);
                    }
                }
            }

            void SettingsWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->scrollWidget->getMinimumSize());
            }

            void SettingsWidget::_layoutEvent(Event::Layout& event)
            {
                _p->scrollWidget->setGeometry(getGeometry());
            }

            void SettingsWidget::_initEvent(Event::Init& event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.bellows["View"]->setText(_getText(DJV_TEXT("file_browser_settings_view")));
                    p.bellows["Thumbnails"]->setText(_getText(DJV_TEXT("file_browser_settings_thumbnails")));
                    p.bellows["Keyboard"]->setText(_getText(DJV_TEXT("file_browser_settings_keyboard_shortcuts")));
                    p.bellows["Misc"]->setText(_getText(DJV_TEXT("file_browser_settings_miscellaneous")));
                    p.thumbnailSizeSlider->setTooltip(_getText(DJV_TEXT("file_browser_settings_thumbnail_size_tooltip")));
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
