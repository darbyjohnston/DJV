// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/JPEGSettingsWidget.h>

#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>

#include <djvAV/IOSystem.h>
#include <djvAV/JPEGFunc.h>

#include <djvSystem/Context.h>

#include <djvMath/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct JPEGWidget::Private
            {
                std::shared_ptr<UI::Numeric::IntSlider> qualitySlider;
                std::shared_ptr<UI::FormLayout> layout;
            };

            void JPEGWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::Settings::JPEGWidget");

                p.qualitySlider = UI::Numeric::IntSlider::create(context);
                p.qualitySlider->setRange(Math::IntRange(0, 100));

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.qualitySlider);
                addChild(p.layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<JPEGWidget>(std::dynamic_pointer_cast<JPEGWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.qualitySlider->setValueCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto io = context->getSystemT<AV::IO::IOSystem>();
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

            JPEGWidget::JPEGWidget() :
                _p(new Private)
            {}

            std::shared_ptr<JPEGWidget> JPEGWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<JPEGWidget>(new JPEGWidget);
                out->_init(context);
                return out;
            }

            std::string JPEGWidget::getSettingsName() const
            {
                return DJV_TEXT("settings_io_section_jpeg");
            }

            std::string JPEGWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_io");
            }

            std::string JPEGWidget::getSettingsSortKey() const
            {
                return "d";
            }

            void JPEGWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.qualitySlider, _getText(DJV_TEXT("settings_io_jpeg_compression_quality")) + ":");
                }
            }

            void JPEGWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    auto io = context->getSystemT<AV::IO::IOSystem>();
                    AV::IO::JPEG::Options options;
                    rapidjson::Document document;
                    auto& allocator = document.GetAllocator();
                    fromJSON(io->getOptions(AV::IO::JPEG::pluginName, allocator), options);
                    p.qualitySlider->setValue(options.quality);
                }
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

