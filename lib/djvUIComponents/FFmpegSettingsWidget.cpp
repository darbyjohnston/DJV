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

#include <djvUIComponents/FFmpegSettingsWidget.h>

#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/IntSlider.h>

#include <djvAV/FFmpeg.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct FFmpegSettingsWidget::Private
        {
            std::shared_ptr<IntSlider> threadCountSlider;
            std::shared_ptr<FormLayout> layout;
        };

        void FFmpegSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::FFmpegSettingsWidget");

            p.threadCountSlider = IntSlider::create(context);
            p.threadCountSlider->setRange(IntRange(1, 16));

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.threadCountSlider);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<FFmpegSettingsWidget>(std::dynamic_pointer_cast<FFmpegSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.threadCountSlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::System>();
                            AV::IO::FFmpeg::Options options;
                            fromJSON(io->getOptions(AV::IO::FFmpeg::pluginName), options);
                            options.threadCount = value;
                            io->setOptions(AV::IO::FFmpeg::pluginName, toJSON(options));
                        }
                    }
                });
        }

        FFmpegSettingsWidget::FFmpegSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<FFmpegSettingsWidget> FFmpegSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FFmpegSettingsWidget>(new FFmpegSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string FFmpegSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("FFmpeg");
        }

        std::string FFmpegSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("I/O");
        }

        std::string FFmpegSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void FFmpegSettingsWidget::_textUpdateEvent(Event::TextUpdate& event)
        {
            ISettingsWidget::_textUpdateEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.threadCountSlider, _getText(DJV_TEXT("Thread count")) + ":");
            _widgetUpdate();
        }

        void FFmpegSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<AV::IO::System>();
                AV::IO::FFmpeg::Options options;
                fromJSON(io->getOptions(AV::IO::FFmpeg::pluginName), options);

                p.threadCountSlider->setValue(options.threadCount);
            }
        }

    } // namespace UI
} // namespace djv

