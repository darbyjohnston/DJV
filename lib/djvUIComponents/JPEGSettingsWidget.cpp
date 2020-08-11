// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/JPEGSettingsWidget.h>

#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>

#include <djvAV/IOSystem.h>
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
                            rapidjson::Document document;
                            auto& allocator = document.GetAllocator();
                            fromJSON(io->getOptions(AV::IO::JPEG::pluginName, allocator), options);
                            options.quality = value;
                            io->setOptions(AV::IO::JPEG::pluginName, toJSON(options, allocator));
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
            return DJV_TEXT("settings_io_section_jpeg");
        }

        std::string JPEGSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_io");
        }

        std::string JPEGSettingsWidget::getSettingsSortKey() const
        {
            return "ZZZ";
        }

        void JPEGSettingsWidget::setLabelSizeGroup(const std::weak_ptr<LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void JPEGSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.qualitySlider, _getText(DJV_TEXT("settings_io_jpeg_compression_quality")) + ":");
            }
        }

        void JPEGSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<AV::IO::System>();
                AV::IO::JPEG::Options options;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                fromJSON(io->getOptions(AV::IO::JPEG::pluginName, allocator), options);
                p.qualitySlider->setValue(options.quality);
            }
        }

    } // namespace UI
} // namespace djv

