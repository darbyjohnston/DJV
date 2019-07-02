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

#include <djvUIComponents/PPMSettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/ListButton.h>

#include <djvAV/PPM.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct PPMSettingsWidget::Private
        {
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::map<AV::IO::PPM::Data, std::shared_ptr<UI::ListButton> > buttons;
            std::shared_ptr<FlowLayout> buttonLayout;
            std::shared_ptr<FormLayout> layout;
        };

        void PPMSettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::PPMSettingsWidget");

            p.buttonGroup = ButtonGroup::create(ButtonType::Radio);
            for (auto i : AV::IO::PPM::getDataEnums())
            {
                auto button = UI::ListButton::create(context);
                p.buttonGroup->addButton(button);
                p.buttons[i] = button;
            }

            p.layout = FormLayout::create(context);
            p.buttonLayout = FlowLayout::create(context);
            for (auto i : AV::IO::PPM::getDataEnums())
            {
                p.buttonLayout->addChild(p.buttons[i]);
            }
            p.layout->addChild(p.buttonLayout);
            addChild(p.layout);

            AV::IO::PPM::Settings settings;
            auto io = context->getSystemT<AV::IO::System>();
            fromJSON(io->getOptions(AV::IO::PPM::pluginName), settings);
            p.buttonGroup->setChecked(static_cast<int>(settings.data));

            auto weak = std::weak_ptr<PPMSettingsWidget>(std::dynamic_pointer_cast<PPMSettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak, context](int value)
            {
                AV::IO::PPM::Settings settings;
                settings.data = static_cast<AV::IO::PPM::Data>(value);
                auto io = context->getSystemT<AV::IO::System>();
                io->setOptions(AV::IO::PPM::pluginName, toJSON(settings));
            });
        }

        PPMSettingsWidget::PPMSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PPMSettingsWidget> PPMSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<PPMSettingsWidget>(new PPMSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string PPMSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("PPM");
        }

        std::string PPMSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("I/O");
        }

        std::string PPMSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void PPMSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.buttonLayout, _getText(DJV_TEXT("Data type")) + ":");
            for (auto i : AV::IO::PPM::getDataEnums())
            {
                std::stringstream ss;
                ss << i;
                p.buttons[i]->setText(ss.str());
            }
        }

    } // namespace UI
} // namespace djv

