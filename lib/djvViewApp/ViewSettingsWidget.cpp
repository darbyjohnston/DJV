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

#include <djvViewApp/ViewSettingsWidget.h>

#include <djvViewApp/ViewSettings.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Speed.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewSettingsWidget::Private
        {
            std::shared_ptr<UI::ComboBox> scrollWheelZoomSpeedComboBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<ScrollWheelZoomSpeed> > scrollWheelZoomSpeedObserver;
        };

        void ViewSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewSettingsWidget");

            p.scrollWheelZoomSpeedComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.scrollWheelZoomSpeedComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ViewSettingsWidget>(std::dynamic_pointer_cast<ViewSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.scrollWheelZoomSpeedComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
                            {
                                viewSettings->setScrollWheelZoomSpeed(static_cast<ScrollWheelZoomSpeed>(value));
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
            {
                p.scrollWheelZoomSpeedObserver = ValueObserver<ScrollWheelZoomSpeed>::create(
                    viewSettings->observeScrollWheelZoomSpeed(),
                    [weak](ScrollWheelZoomSpeed value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->scrollWheelZoomSpeedComboBox->setCurrentItem(static_cast<int>(value));
                        }
                    });
            }
        }

        ViewSettingsWidget::ViewSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ViewSettingsWidget> ViewSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ViewSettingsWidget>(new ViewSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ViewSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("widget_color_space_display_view");
        }

        std::string ViewSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("widget_color_space_display_view");
        }

        std::string ViewSettingsWidget::getSettingsSortKey() const
        {
            return "C";
        }

        void ViewSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.scrollWheelZoomSpeedComboBox, _getText(DJV_TEXT("scroll_wheel_zoom_speed")) + ":");
            _widgetUpdate();
        }

        void ViewSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.scrollWheelZoomSpeedComboBox->clearItems();
                for (auto i : getScrollWheelZoomSpeedEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    p.scrollWheelZoomSpeedComboBox->addItem(_getText(ss.str()));
                }
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
                {
                    p.scrollWheelZoomSpeedComboBox->setCurrentItem(static_cast<int>(viewSettings->observeScrollWheelZoomSpeed()->get()));
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

