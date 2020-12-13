// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/TIFFSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/Label.h>
#include <djvUI/FormLayout.h>

#include <djvAV/IOSystem.h>
#include <djvAV/TIFFFunc.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct TIFFWidget::Private
            {
                std::shared_ptr<UI::ComboBox> compressionComboBox;
                std::shared_ptr<UI::FormLayout> layout;
            };

            void TIFFWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UIComponents::Settings::TIFFWidget");

                p.compressionComboBox = UI::ComboBox::create(context);

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.compressionComboBox);
                addChild(p.layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<TIFFWidget>(std::dynamic_pointer_cast<TIFFWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.compressionComboBox->setCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::IOSystem>();
                            AV::IO::TIFF::Options options;
                            rapidjson::Document document;
                            auto& allocator = document.GetAllocator();
                            fromJSON(io->getOptions(AV::IO::TIFF::pluginName, allocator), options);
                            options.compression = static_cast<AV::IO::TIFF::Compression>(value);
                            io->setOptions(AV::IO::TIFF::pluginName, toJSON(options, allocator));
                        }
                    });
            }

            TIFFWidget::TIFFWidget() :
                _p(new Private)
            {}

            std::shared_ptr<TIFFWidget> TIFFWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<TIFFWidget>(new TIFFWidget);
                out->_init(context);
                return out;
            }

            std::string TIFFWidget::getSettingsName() const
            {
                return DJV_TEXT("settings_io_section_tiff");
            }

            std::string TIFFWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_io");
            }

            std::string TIFFWidget::getSettingsSortKey() const
            {
                return "d";
            }

            void TIFFWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.compressionComboBox, _getText(DJV_TEXT("settings_io_tiff_compression")) + ":");
                    _widgetUpdate();
                }
            }

            void TIFFWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    auto io = context->getSystemT<AV::IO::IOSystem>();
                    AV::IO::TIFF::Options options;
                    rapidjson::Document document;
                    auto& allocator = document.GetAllocator();
                    fromJSON(io->getOptions(AV::IO::TIFF::pluginName, allocator), options);
                    std::vector<std::string> items;
                    for (auto i : AV::IO::TIFF::getCompressionEnums())
                    {
                        std::stringstream ss;
                        ss << i;
                        items.push_back(_getText(ss.str()));
                    }
                    p.compressionComboBox->setItems(items);
                    p.compressionComboBox->setCurrentItem(static_cast<int>(options.compression));
                }
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

