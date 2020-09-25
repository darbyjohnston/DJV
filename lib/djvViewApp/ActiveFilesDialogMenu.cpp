// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ActiveFilesDialogPrivate.h>

#include <djvViewApp/FileSettings.h>

#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/FileBrowserSettings.h>

#include <djvUI/ActionButton.h>
#include <djvUI/IntSlider.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>

#include <djvImage/ImageData.h>

#include <djvSystem/Context.h>
#include <djvSystem/TimerFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ActiveFilesMenu::Private
        {
            std::shared_ptr<UI::IntSlider> thumbnailSizeSlider;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;

            std::shared_ptr<ValueObserver<Image::Size> > thumbnailSizeSettingsObserver;
        };

        void ActiveFilesMenu::_init(
            const std::map<std::string, std::shared_ptr<UI::Action> >& actions,
            const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ActiveFilesMenu");

            auto increaseThumbnailSizeButton = UI::ActionButton::create(context);
            increaseThumbnailSizeButton->addAction(actions.at("IncreaseThumbnailSize"));
            auto decreaseThumbnailSizeButton = UI::ActionButton::create(context);
            decreaseThumbnailSizeButton->addAction(actions.at("DecreaseThumbnailSize"));

            p.thumbnailSizeSlider = UI::IntSlider::create(context);
            p.thumbnailSizeSlider->setRange(UI::FileBrowser::thumbnailSizeRange);
            p.thumbnailSizeSlider->setDelay(System::getTimerDuration(System::TimerValue::Medium));

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(increaseThumbnailSizeButton);
            vLayout->addChild(decreaseThumbnailSizeButton);
            vLayout->addChild(p.thumbnailSizeSlider);
            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setMinimumSizeRole(UI::MetricsRole::None);
            p.scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            p.scrollWidget->addChild(vLayout);
            addChild(p.scrollWidget);

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
            auto weak = std::weak_ptr<ActiveFilesMenu>(std::dynamic_pointer_cast<ActiveFilesMenu>(shared_from_this()));
            p.thumbnailSizeSettingsObserver = ValueObserver<Image::Size>::create(
                fileBrowserSettings->observeThumbnailSize(),
                [weak](const Image::Size& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->thumbnailSizeSlider->setValue(value.w);
                    }
                });

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.thumbnailSizeSlider->setValueCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
                        fileBrowserSettings->setThumbnailSize(Image::Size(value, ceilf(value / 2.F)));
                    }
                });
        }

        ActiveFilesMenu::ActiveFilesMenu() :
            _p(new Private)
        {}

        ActiveFilesMenu::~ActiveFilesMenu()
        {}

        std::shared_ptr<ActiveFilesMenu> ActiveFilesMenu::create(
            const std::map<std::string, std::shared_ptr<UI::Action> >& actions,
            const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ActiveFilesMenu>(new ActiveFilesMenu);
            out->_init(actions, context);
            return out;
        }

        void ActiveFilesMenu::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->scrollWidget->getMinimumSize());
        }

        void ActiveFilesMenu::_layoutEvent(System::Event::Layout&)
        {
            _p->scrollWidget->setGeometry(getGeometry());
        }

        void ActiveFilesMenu::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.thumbnailSizeSlider->setTooltip(_getText(DJV_TEXT("active_files_settings_thumbnail_size_tooltip")));
            }
        }

    } // namespace ViewApp
} // namespace djv

