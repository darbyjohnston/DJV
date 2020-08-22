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

                auto sortByNameButton = ActionButton::create(context);
                sortByNameButton->addAction(actions.at("SortByName"));
                auto sortBySizeButton = ActionButton::create(context);
                sortBySizeButton->addAction(actions.at("SortBySize"));
                auto sortByTimeButton = ActionButton::create(context);
                sortByTimeButton->addAction(actions.at("SortByTime"));
                auto reverseSortButton = ActionButton::create(context);
                reverseSortButton->addAction(actions.at("ReverseSort"));
                auto sortDirectoriesFirstButton = ActionButton::create(context);
                sortDirectoriesFirstButton->addAction(actions.at("SortDirectoriesFirst"));

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

                auto fileSequencesButton = ActionButton::create(context);
                fileSequencesButton->addAction(actions.at("FileSequences"));
                auto showHiddenButton = ActionButton::create(context);
                showHiddenButton->addAction(actions.at("ShowHidden"));

                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->addChild(sortByNameButton);
                vLayout->addChild(sortBySizeButton);
                vLayout->addChild(sortByTimeButton);
                vLayout->addSeparator();
                vLayout->addChild(reverseSortButton);
                vLayout->addChild(sortDirectoriesFirstButton);
                vLayout->addSeparator();
                vLayout->addChild(tilesButton);
                vLayout->addChild(listButton);
                vLayout->addSeparator();
                vLayout->addChild(increaseThumbnailSizeButton);
                vLayout->addChild(decreaseThumbnailSizeButton);
                vLayout->addChild(p.thumbnailSizeSlider);
                vLayout->addSeparator();
                vLayout->addChild(fileSequencesButton);
                vLayout->addChild(showHiddenButton);
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
                    p.thumbnailSizeSlider->setTooltip(_getText(DJV_TEXT("file_browser_settings_thumbnail_size_tooltip")));
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
