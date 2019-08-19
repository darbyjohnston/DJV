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

#include <djvUIComponents/CineonSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>

#include <djvAV/Cineon.h>
#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct CineonSettingsWidget::Private
        {
            std::vector<std::string> colorSpaces;
            std::shared_ptr<ComboBox> colorSpaceComboBox;
            std::shared_ptr<FormLayout> layout;
            std::shared_ptr<ListObserver<std::string> > colorSpaceObserver;
        };

        void CineonSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::CineonSettingsWidget");

            p.colorSpaceComboBox = ComboBox::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.colorSpaceComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<CineonSettingsWidget>(std::dynamic_pointer_cast<CineonSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.colorSpaceComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto io = context->getSystemT<AV::IO::System>();
                            AV::IO::Cineon::Options options;
                            fromJSON(io->getOptions(AV::IO::Cineon::pluginName), options);
                            options.colorSpace = widget->_p->colorSpaces[value];
                            io->setOptions(AV::IO::Cineon::pluginName, toJSON(options));
                        }
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            ListObserver<std::string>::create(
                ocioSystem->observeColorSpaces(),
                [weak, contextWeak](const std::vector<std::string>&)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_widgetUpdate();
                        }
                    }
                });
        }

        CineonSettingsWidget::CineonSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<CineonSettingsWidget> CineonSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<CineonSettingsWidget>(new CineonSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string CineonSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Cineon");
        }

        std::string CineonSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("I/O");
        }

        std::string CineonSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void CineonSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.colorSpaceComboBox, _getText(DJV_TEXT("Color space")) + ":");
            _widgetUpdate();
        }

        void CineonSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<AV::IO::System>();
                AV::IO::Cineon::Options options;
                fromJSON(io->getOptions(AV::IO::Cineon::pluginName), options);

                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                p.colorSpaces.clear();
                p.colorSpaces.push_back(std::string());
                for (const auto& i : ocioSystem->observeColorSpaces()->get())
                {
                    p.colorSpaces.push_back(i);
                }
                p.colorSpaceComboBox->setItems(p.colorSpaces);

                int index = 0;
                const auto i = std::find(p.colorSpaces.begin(), p.colorSpaces.end(), options.colorSpace);
                if (i != p.colorSpaces.end())
                {
                    index = i - p.colorSpaces.begin();
                }
                p.colorSpaceComboBox->setCurrentItem(index);
            }
        }

    } // namespace UI
} // namespace djv

