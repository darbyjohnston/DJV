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

#include <djvViewApp/ImageViewSettingsWidget.h>

#include <djvViewApp/ImageViewSettings.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/ColorSwatch.h>

#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageViewSettingsWidget::Private
        {
            std::shared_ptr<UI::ColorSwatch> colorSwatch;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver;
        };

        void ImageViewSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ImageViewSettingsWidget");

            p.colorSwatch = UI::ColorSwatch::create(context);
            p.colorSwatch->setPointerEnabled(true);

            addChild(p.colorSwatch);

            auto weak = std::weak_ptr<ImageViewSettingsWidget>(std::dynamic_pointer_cast<ImageViewSettingsWidget>(shared_from_this()));
            p.colorSwatch->setClickedCallback(
                [weak, context]
            {
                if (auto widget = weak.lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    auto imageViewSettings = settingsSystem->getSettingsT<ImageViewSettings>();
                    auto colorPickerDialogSystem = context->getSystemT<UI::ColorPickerDialogSystem>();
                    colorPickerDialogSystem->colorPicker(
                        widget->_getText(DJV_TEXT("Color Picker")),
                        imageViewSettings->observeBackgroundColor()->get(),
                        [context](const AV::Image::Color& value)
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto imageViewSettings = settingsSystem->getSettingsT<ImageViewSettings>();
                            imageViewSettings->setBackgroundColor(value);
                        });
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto imageViewSettings = settingsSystem->getSettingsT<ImageViewSettings>();
            p.backgroundColorObserver = ValueObserver<AV::Image::Color>::create(
                imageViewSettings->observeBackgroundColor(),
                [weak](const AV::Image::Color& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->colorSwatch->setColor(value);
                }
            });
        }

        ImageViewSettingsWidget::ImageViewSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ImageViewSettingsWidget> ImageViewSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<ImageViewSettingsWidget>(new ImageViewSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ImageViewSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("View");
        }

        std::string ImageViewSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("View");
        }

        std::string ImageViewSettingsWidget::getSettingsSortKey() const
        {
            return "E";
        }

    } // namespace ViewApp
} // namespace djv

