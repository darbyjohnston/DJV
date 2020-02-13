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

#include <djvUIComponents/OpenEXRSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/IntSlider.h>

#include <djvAV/OpenEXR.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct OpenEXRSettingsWidget::Private
        {
            std::shared_ptr<IntSlider> threadCountSlider;
            std::shared_ptr<ComboBox> channelsComboBox;
            std::shared_ptr<ComboBox> compressionComboBox;
            std::shared_ptr<FloatSlider> dwaCompressionLevelSlider;
            std::shared_ptr<FormLayout> layout;
        };

        void OpenEXRSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::OpenEXRSettingsWidget");

            p.threadCountSlider = IntSlider::create(context);
            p.threadCountSlider->setRange(IntRange(1, 16));

            p.channelsComboBox = ComboBox::create(context);
            
            p.compressionComboBox = ComboBox::create(context);

            p.dwaCompressionLevelSlider = FloatSlider::create(context);
            p.dwaCompressionLevelSlider->setRange(FloatRange(0.F, 200.F));

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.threadCountSlider);
            p.layout->addChild(p.channelsComboBox);
            p.layout->addChild(p.compressionComboBox);
            p.layout->addChild(p.dwaCompressionLevelSlider);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<OpenEXRSettingsWidget>(std::dynamic_pointer_cast<OpenEXRSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.threadCountSlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::System>();
                            AV::IO::OpenEXR::Options options;
                            fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName), options);
                            options.threadCount = value;
                            io->setOptions(AV::IO::OpenEXR::pluginName, toJSON(options));
                        }
                    }
                });

            p.channelsComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::System>();
                            AV::IO::OpenEXR::Options options;
                            fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName), options);
                            options.channels = static_cast<AV::IO::OpenEXR::Channels>(value);
                            io->setOptions(AV::IO::OpenEXR::pluginName, toJSON(options));
                        }
                    }
                });

            p.compressionComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::System>();
                            AV::IO::OpenEXR::Options options;
                            fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName), options);
                            options.compression = static_cast<AV::IO::OpenEXR::Compression>(value);
                            io->setOptions(AV::IO::OpenEXR::pluginName, toJSON(options));
                        }
                    }
                });

            p.dwaCompressionLevelSlider->setValueCallback(
                [weak, contextWeak](float value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::System>();
                            AV::IO::OpenEXR::Options options;
                            fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName), options);
                            options.dwaCompressionLevel = value;
                            io->setOptions(AV::IO::OpenEXR::pluginName, toJSON(options));
                        }
                    }
                });
        }

        OpenEXRSettingsWidget::OpenEXRSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<OpenEXRSettingsWidget> OpenEXRSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<OpenEXRSettingsWidget>(new OpenEXRSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string OpenEXRSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_io_section_openexr");
        }

        std::string OpenEXRSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_io");
        }

        std::string OpenEXRSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void OpenEXRSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.threadCountSlider, _getText(DJV_TEXT("settings_io_exr_thread_count")) + ":");
            p.layout->setText(p.channelsComboBox, _getText(DJV_TEXT("settings_io_exr_channel_grouping")) + ":");
            p.layout->setText(p.compressionComboBox, _getText(DJV_TEXT("settings_io_exr_compression")) + ":");
            p.layout->setText(p.dwaCompressionLevelSlider, _getText(DJV_TEXT("settings_io_exr_dwa_compression_level")) + ":");
            _widgetUpdate();
        }

        void OpenEXRSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<AV::IO::System>();
                AV::IO::OpenEXR::Options options;
                fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName), options);

                p.threadCountSlider->setValue(options.threadCount);

                p.channelsComboBox->clearItems();
                for (auto i : AV::IO::OpenEXR::getChannelsEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    p.channelsComboBox->addItem(_getText(ss.str()));
                }
                p.channelsComboBox->setCurrentItem(static_cast<int>(options.channels));

                p.compressionComboBox->clearItems();
                for (auto i : AV::IO::OpenEXR::getCompressionEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    p.compressionComboBox->addItem(_getText(ss.str()));
                }
                p.compressionComboBox->setCurrentItem(static_cast<int>(options.compression));

                p.dwaCompressionLevelSlider->setValue(options.dwaCompressionLevel);
            }
        }

    } // namespace UI
} // namespace djv

