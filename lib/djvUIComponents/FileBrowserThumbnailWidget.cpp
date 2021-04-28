// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUIComponents/FileBrowserSettings.h>

#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace FileBrowser
        {
            struct ThumbnailWidget::Private
            {
                std::shared_ptr<UI::Numeric::IntSlider> thumbnailSizeSlider;

                std::shared_ptr<UI::VerticalLayout> layout;

                std::shared_ptr<Observer::Value<Image::Size> > thumbnailSizeObserver;
            };

            void ThumbnailWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UIComponents::FileBrowser::ThumbnailWidget");

                p.thumbnailSizeSlider = UI::Numeric::IntSlider::create(context);
                p.thumbnailSizeSlider->setRange(thumbnailSizeRange);
                p.thumbnailSizeSlider->setDelay(System::getTimerDuration(System::TimerValue::Medium));

                p.layout = UI::VerticalLayout::create(context);
                p.layout->addChild(p.thumbnailSizeSlider);
                addChild(p.layout);

                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.thumbnailSizeSlider->setValueCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setThumbnailSize(Image::Size(value, ceilf(value / 2.F)));
                        }
                    });

                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                auto weak = std::weak_ptr<ThumbnailWidget>(std::dynamic_pointer_cast<ThumbnailWidget>(shared_from_this()));
                p.thumbnailSizeObserver = Observer::Value<Image::Size>::create(
                    fileBrowserSettings->observeThumbnailSize(),
                    [weak](const Image::Size& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->thumbnailSizeSlider->setValue(value.w);
                        }
                    });
            }

            ThumbnailWidget::ThumbnailWidget() :
                _p(new Private)
            {}

            ThumbnailWidget::~ThumbnailWidget()
            {}

            std::shared_ptr<ThumbnailWidget> ThumbnailWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<ThumbnailWidget>(new ThumbnailWidget);
                out->_init(context);
                return out;
            }

            void ThumbnailWidget::_preLayoutEvent(System::Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void ThumbnailWidget::_layoutEvent(System::Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv
