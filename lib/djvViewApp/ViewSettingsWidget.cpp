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

#include <djvViewApp/ViewSettingsWidget.h>

#include <djvViewApp/ViewSettings.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewBackgroundSettingsWidget::Private
        {
            std::shared_ptr<UI::ColorPickerSwatch> colorSwatch;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver;
        };

        void ViewBackgroundSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewBackgroundSettingsWidget");

            p.colorSwatch = UI::ColorPickerSwatch::create(context);
            addChild(p.colorSwatch);

            auto weak = std::weak_ptr<ViewBackgroundSettingsWidget>(std::dynamic_pointer_cast<ViewBackgroundSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.colorSwatch->setColorCallback(
                [contextWeak](const AV::Image::Color& value)
            {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundColor(value);
                    }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.backgroundColorObserver = ValueObserver<AV::Image::Color>::create(
                viewSettings->observeBackgroundColor(),
                [weak](const AV::Image::Color& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->colorSwatch->setColor(value);
                }
            });
        }

        ViewBackgroundSettingsWidget::ViewBackgroundSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ViewBackgroundSettingsWidget> ViewBackgroundSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ViewBackgroundSettingsWidget>(new ViewBackgroundSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ViewBackgroundSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Background Color");
        }

        std::string ViewBackgroundSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("View");
        }

        std::string ViewBackgroundSettingsWidget::getSettingsSortKey() const
        {
            return "E";
        }

    } // namespace ViewApp
} // namespace djv

