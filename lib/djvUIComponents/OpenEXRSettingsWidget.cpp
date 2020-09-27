// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/OpenEXRSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/IntSlider.h>

#include <djvAV/IOSystem.h>
#include <djvAV/OpenEXRFunc.h>

#include <djvSystem/Context.h>

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

        void OpenEXRSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::OpenEXRSettingsWidget");

            p.threadCountSlider = IntSlider::create(context);
            p.threadCountSlider->setRange(Math::IntRange(1, 16));

            p.channelsComboBox = ComboBox::create(context);
            
            p.compressionComboBox = ComboBox::create(context);

            p.dwaCompressionLevelSlider = FloatSlider::create(context);
            p.dwaCompressionLevelSlider->setRange(Math::FloatRange(0.F, 200.F));

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.threadCountSlider);
            p.layout->addChild(p.channelsComboBox);
            p.layout->addChild(p.compressionComboBox);
            p.layout->addChild(p.dwaCompressionLevelSlider);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<OpenEXRSettingsWidget>(std::dynamic_pointer_cast<OpenEXRSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.threadCountSlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::IOSystem>();
                            AV::IO::OpenEXR::Options options;
                            rapidjson::Document document;
                            auto& allocator = document.GetAllocator();
                            fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName, allocator), options);
                            options.threadCount = value;
                            io->setOptions(AV::IO::OpenEXR::pluginName, toJSON(options, allocator));
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
                            auto io = context->getSystemT<AV::IO::IOSystem>();
                            AV::IO::OpenEXR::Options options;
                            rapidjson::Document document;
                            auto& allocator = document.GetAllocator();
                            fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName, allocator), options);
                            options.channels = static_cast<AV::IO::OpenEXR::Channels>(value);
                            io->setOptions(AV::IO::OpenEXR::pluginName, toJSON(options, allocator));
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
                            auto io = context->getSystemT<AV::IO::IOSystem>();
                            AV::IO::OpenEXR::Options options;
                            rapidjson::Document document;
                            auto& allocator = document.GetAllocator();
                            fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName, allocator), options);
                            options.compression = static_cast<AV::IO::OpenEXR::Compression>(value);
                            io->setOptions(AV::IO::OpenEXR::pluginName, toJSON(options, allocator));
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
                            auto io = context->getSystemT<AV::IO::IOSystem>();
                            AV::IO::OpenEXR::Options options;
                            rapidjson::Document document;
                            auto& allocator = document.GetAllocator();
                            fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName, allocator), options);
                            options.dwaCompressionLevel = value;
                            io->setOptions(AV::IO::OpenEXR::pluginName, toJSON(options, allocator));
                        }
                    }
                });
        }

        OpenEXRSettingsWidget::OpenEXRSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<OpenEXRSettingsWidget> OpenEXRSettingsWidget::create(const std::shared_ptr<System::Context>& context)
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
            return "d";
        }

        void OpenEXRSettingsWidget::setLabelSizeGroup(const std::weak_ptr<LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void OpenEXRSettingsWidget::_initEvent(System::Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.threadCountSlider, _getText(DJV_TEXT("settings_io_exr_thread_count")) + ":");
                p.layout->setText(p.channelsComboBox, _getText(DJV_TEXT("settings_io_exr_channel_grouping")) + ":");
                p.layout->setText(p.compressionComboBox, _getText(DJV_TEXT("settings_io_exr_compression")) + ":");
                p.layout->setText(p.dwaCompressionLevelSlider, _getText(DJV_TEXT("settings_io_exr_dwa_compression_level")) + ":");
                _widgetUpdate();
            }
        }

        void OpenEXRSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<AV::IO::IOSystem>();
                AV::IO::OpenEXR::Options options;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                fromJSON(io->getOptions(AV::IO::OpenEXR::pluginName, allocator), options);

                p.threadCountSlider->setValue(options.threadCount);

                std::vector<std::string> items;
                for (auto i : AV::IO::OpenEXR::getChannelsEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                p.channelsComboBox->setItems(items);
                p.channelsComboBox->setCurrentItem(static_cast<int>(options.channels));

                items.clear();
                for (auto i : AV::IO::OpenEXR::getCompressionEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                p.compressionComboBox->setItems(items);
                p.compressionComboBox->setCurrentItem(static_cast<int>(options.compression));

                p.dwaCompressionLevelSlider->setValue(options.dwaCompressionLevel);
            }
        }

    } // namespace UI
} // namespace djv

