// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/RecentFilesPrivate.h>

#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/FileBrowserSettings.h>

#include <djvUI/IntSlider.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/TimerFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct RecentFilesThumbnailWidget::Private
        {
            std::shared_ptr<UI::Numeric::IntSlider> thumbnailSizeSlider;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<Image::Size> > thumbnailSizeSettingsObserver;
        };

        void RecentFilesThumbnailWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::RecentFilesThumbnailWidget");

            p.thumbnailSizeSlider = UI::Numeric::IntSlider::create(context);
            p.thumbnailSizeSlider->setRange(UI::FileBrowser::thumbnailSizeRange);
            p.thumbnailSizeSlider->setDelay(System::getTimerDuration(System::TimerValue::Medium));

            p.layout = UI::VerticalLayout::create(context);
            p.layout->addChild(p.thumbnailSizeSlider);
            addChild(p.layout);

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
            auto weak = std::weak_ptr<RecentFilesThumbnailWidget>(std::dynamic_pointer_cast<RecentFilesThumbnailWidget>(shared_from_this()));
            p.thumbnailSizeSettingsObserver = Observer::Value<Image::Size>::create(
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

        RecentFilesThumbnailWidget::RecentFilesThumbnailWidget() :
            _p(new Private)
        {}

        RecentFilesThumbnailWidget::~RecentFilesThumbnailWidget()
        {}

        std::shared_ptr<RecentFilesThumbnailWidget> RecentFilesThumbnailWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<RecentFilesThumbnailWidget>(new RecentFilesThumbnailWidget);
            out->_init(context);
            return out;
        }

        void RecentFilesThumbnailWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void RecentFilesThumbnailWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

    } // namespace ViewApp
} // namespace djv

