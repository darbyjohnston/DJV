// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/TIFFSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/Label.h>
#include <djvUI/FormLayout.h>

#include <djvAV/TIFF.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TIFFSettingsWidget::Private
        {
            std::shared_ptr<ComboBox> compressionComboBox;
            std::shared_ptr<FormLayout> layout;
        };

        void TIFFSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::TIFFSettingsWidget");

            p.compressionComboBox = ComboBox::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.compressionComboBox);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<TIFFSettingsWidget>(std::dynamic_pointer_cast<TIFFSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.compressionComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto io = context->getSystemT<AV::IO::System>();
                        AV::IO::TIFF::Options options;
                        fromJSON(io->getOptions(AV::IO::TIFF::pluginName), options);
                        options.compression = static_cast<AV::IO::TIFF::Compression>(value);
                        io->setOptions(AV::IO::TIFF::pluginName, toJSON(options));
                    }
                });
        }

        TIFFSettingsWidget::TIFFSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TIFFSettingsWidget> TIFFSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TIFFSettingsWidget>(new TIFFSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string TIFFSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_io_section_tiff");
        }

        std::string TIFFSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_io");
        }

        std::string TIFFSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void TIFFSettingsWidget::setLabelSizeGroup(const std::weak_ptr<LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void TIFFSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.compressionComboBox, _getText(DJV_TEXT("settings_io_tiff_compression")) + ":");
                _widgetUpdate();
            }
        }

        void TIFFSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<AV::IO::System>();
                AV::IO::TIFF::Options options;
                fromJSON(io->getOptions(AV::IO::TIFF::pluginName), options);

                p.compressionComboBox->clearItems();
                for (auto i : AV::IO::TIFF::getCompressionEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    p.compressionComboBox->addItem(_getText(ss.str()));
                }
                p.compressionComboBox->setCurrentItem(static_cast<int>(options.compression));
            }
        }

    } // namespace UI
} // namespace djv

