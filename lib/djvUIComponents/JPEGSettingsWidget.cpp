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

#include <djvUIComponents/JPEGSettingsWidget.h>

#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>

#include <djvAV/JPEG.h>

#include <djvCore/Context.h>
#include <djvCore/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct JPEGSettingsWidget::Private
        {
            std::shared_ptr<IntSlider> qualitySlider;
            std::shared_ptr<FormLayout> layout;
        };

        void JPEGSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::JPEGSettingsWidget");

            p.qualitySlider = IntSlider::create(context);
            p.qualitySlider->setRange(IntRange(0, 100));

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.qualitySlider);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<JPEGSettingsWidget>(std::dynamic_pointer_cast<JPEGSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.qualitySlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::System>();
                            AV::IO::JPEG::Options options;
                            fromJSON(io->getOptions(AV::IO::JPEG::pluginName), options);
                            options.quality = value;
                            io->setOptions(AV::IO::JPEG::pluginName, toJSON(options));
                        }
                    }
                });
        }

        JPEGSettingsWidget::JPEGSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<JPEGSettingsWidget> JPEGSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<JPEGSettingsWidget>(new JPEGSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string JPEGSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("JPEG");
        }

        std::string JPEGSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("I/O");
        }

        std::string JPEGSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void JPEGSettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.qualitySlider, _getText(DJV_TEXT("Compression quality")) + ":");
        }

        void JPEGSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<AV::IO::System>();
                AV::IO::JPEG::Options options;
                fromJSON(io->getOptions(AV::IO::JPEG::pluginName), options);
                p.qualitySlider->setValue(options.quality);
            }
        }

    } // namespace UI
} // namespace djv

