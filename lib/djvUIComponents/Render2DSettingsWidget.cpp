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

#include <djvUIComponents/Render2DSettingsWidget.h>

#include <djvUI/CheckBox.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Render2DTextSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> lcdCheckBox;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > lcdTextObserver;
        };

        void Render2DTextSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::Render2DTextSettingsWidget");

            p.lcdCheckBox = UI::CheckBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->addChild(p.lcdCheckBox);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.lcdCheckBox->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto render2D = context->getSystemT<AV::Render::Render2D>();
                        render2D->setLCDText(value);
                    }
                });

            auto render2D = context->getSystemT<AV::Render::Render2D>();
            auto weak = std::weak_ptr<Render2DTextSettingsWidget>(std::dynamic_pointer_cast<Render2DTextSettingsWidget>(shared_from_this()));
            p.lcdTextObserver = ValueObserver<bool>::create(
                render2D->observeLCDText(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->lcdCheckBox->setChecked(value);
                        widget->_redraw();
                    }
                });
        }

        Render2DTextSettingsWidget::Render2DTextSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<Render2DTextSettingsWidget> Render2DTextSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Render2DTextSettingsWidget>(new Render2DTextSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string Render2DTextSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Text");
        }

        std::string Render2DTextSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Render 2D");
        }

        std::string Render2DTextSettingsWidget::getSettingsSortKey() const
        {
            return "ZZ";
        }

        void Render2DTextSettingsWidget::_textUpdateEvent(Event::TextUpdate& event)
        {
            ISettingsWidget::_textUpdateEvent(event);
            DJV_PRIVATE_PTR();
            p.lcdCheckBox->setText(_getText(DJV_TEXT("Enable LCD text rendering")));
        }

    } // namespace UI
} // namespace djv

