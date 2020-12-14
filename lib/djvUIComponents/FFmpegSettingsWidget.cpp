// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FFmpegSettingsWidget.h>

#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/IntSlider.h>

#include <djvAV/FFmpegFunc.h>
#include <djvAV/IOSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct FFmpegWidget::Private
            {
                std::shared_ptr<UI::Numeric::IntSlider> threadCountSlider;
                std::shared_ptr<UI::FormLayout> layout;
            };

            void FFmpegWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::Settings::FFmpegWidget");

                p.threadCountSlider = UI::Numeric::IntSlider::create(context);
                p.threadCountSlider->setRange(Math::IntRange(1, 16));

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.threadCountSlider);
                addChild(p.layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<FFmpegWidget>(std::dynamic_pointer_cast<FFmpegWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.threadCountSlider->setValueCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto io = context->getSystemT<AV::IO::IOSystem>();
                                AV::IO::FFmpeg::Options options;
                                rapidjson::Document document;
                                auto& allocator = document.GetAllocator();
                                fromJSON(io->getOptions(AV::IO::FFmpeg::pluginName, allocator), options);
                                options.threadCount = value;
                                io->setOptions(AV::IO::FFmpeg::pluginName, toJSON(options, allocator));
                            }
                        }
                    });
            }

            FFmpegWidget::FFmpegWidget() :
                _p(new Private)
            {}

            std::shared_ptr<FFmpegWidget> FFmpegWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<FFmpegWidget>(new FFmpegWidget);
                out->_init(context);
                return out;
            }

            std::string FFmpegWidget::getSettingsName() const
            {
                return DJV_TEXT("settings_io_section_ffmpeg");
            }

            std::string FFmpegWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_io");
            }

            std::string FFmpegWidget::getSettingsSortKey() const
            {
                return "d";
            }

            void FFmpegWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.threadCountSlider, _getText(DJV_TEXT("settings_io_ffmpeg_thread_count")) + ":");
                }
            }

            void FFmpegWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    auto io = context->getSystemT<AV::IO::IOSystem>();
                    AV::IO::FFmpeg::Options options;
                    rapidjson::Document document;
                    auto& allocator = document.GetAllocator();
                    fromJSON(io->getOptions(AV::IO::FFmpeg::pluginName, allocator), options);
                    p.threadCountSlider->setValue(options.threadCount);
                }
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

