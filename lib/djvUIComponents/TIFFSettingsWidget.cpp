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

#include <djvUIComponents/TIFFSettingsWidget.h>

#include <djvUI/ComboBox.h>
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

        void TIFFSettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();

            p.compressionComboBox = ComboBox::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.compressionComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<TIFFSettingsWidget>(std::dynamic_pointer_cast<TIFFSettingsWidget>(shared_from_this()));
            p.compressionComboBox->setCallback(
                [weak, context](int value)
            {
                if (auto io = context->getSystemT<AV::IO::System>())
                {
                    AV::IO::TIFF::Settings settings;
                    settings.compression = static_cast<AV::IO::TIFF::Compression>(value);
                    io->setOptions(AV::IO::TIFF::pluginName, toJSON(settings));
                }
            });
        }

        TIFFSettingsWidget::TIFFSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TIFFSettingsWidget> TIFFSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<TIFFSettingsWidget>(new TIFFSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string TIFFSettingsWidget::getName() const
        {
            return DJV_TEXT("TIFF");
        }

        std::string TIFFSettingsWidget::getSortKey() const
        {
            return "B";
        }

        void TIFFSettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.compressionComboBox, DJV_TEXT("Output file compression:"));
            _widgetUpdate();
        }

        void TIFFSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.compressionComboBox->clearItems();
            for (auto i : AV::IO::TIFF::getCompressionEnums())
            {
                std::stringstream ss;
                ss << i;
                p.compressionComboBox->addItem(_getText(ss.str()));
            }
            _currentItemUpdate();
        }

        void TIFFSettingsWidget::_currentItemUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto io = getContext()->getSystemT<AV::IO::System>())
            {
                AV::IO::TIFF::Settings settings;
                fromJSON(io->getOptions(AV::IO::TIFF::pluginName), settings);
                p.compressionComboBox->setCurrentItem(static_cast<int>(settings.compression));
            }
        }

    } // namespace UI
} // namespace djv

