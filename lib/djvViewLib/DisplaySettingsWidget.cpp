//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/DisplaySettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/PushButton.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct DisplaySettingsWidget::Private
        {
            std::vector<std::shared_ptr<UI::Style::Style> > customStyles;
            std::vector<std::shared_ptr<UI::Button::Push> > buttons;
            std::shared_ptr<UI::Button::Group> buttonGroup;
            std::shared_ptr<UI::Layout::Flow> layout;
            std::map<int, int> indexToDPI;
            std::map<int, int> dpiToIndex;
            std::shared_ptr<ListObserver<int> > dpiListObserver;
            std::shared_ptr<ValueObserver<int> > dpiObserver;
        };

        void DisplaySettingsWidget::_init(Context * context)
        {
            GroupBox::_init(context);

            DJV_PRIVATE_PTR();
            p.buttonGroup = UI::Button::Group::create(UI::ButtonType::Radio);

            p.layout = UI::Layout::Flow::create(context);
            addWidget(p.layout);

            auto weak = std::weak_ptr<DisplaySettingsWidget>(std::dynamic_pointer_cast<DisplaySettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto uiSystem = context->getSystemT<UI::UISystem>().lock())
                    {
                        const auto i = widget->_p->indexToDPI.find(value);
                        if (i != widget->_p->indexToDPI.end())
                        {
                            uiSystem->getStyleSettings()->setDPI(i->second);
                        }
                    }
                }
            });

            if (auto avSystem = context->getSystemT<AV::AVSystem>().lock())
            {
                p.dpiListObserver = ListObserver<int>::create(
                    avSystem->observeDPIList(),
                    [weak, context](const std::vector<int> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        int currentItem = widget->_p->buttonGroup->getChecked();
                        if (-1 == currentItem && value.size())
                        {
                            currentItem = 0;
                        }
                        widget->_p->buttons.clear();
                        widget->_p->buttonGroup->clearButtons();
                        widget->_p->layout->clearWidgets();
                        widget->_p->indexToDPI.clear();
                        widget->_p->dpiToIndex.clear();
                        int j = 0;
                        for (const auto & i : value)
                        {
                            auto customStyle = UI::Style::Style::create(context);
                            customStyle->setDPI(i);
                            widget->_p->customStyles.push_back(customStyle);
                            auto button = UI::Button::Push::create(context);
                            button->setStyle(customStyle);
                            widget->_p->buttons.push_back(button);
                            widget->_p->buttonGroup->addButton(button);
                            widget->_p->layout->addWidget(button);
                            widget->_p->indexToDPI[j] = i;
                            widget->_p->dpiToIndex[i] = j;
                            ++j;
                        }
                        widget->_p->buttonGroup->setChecked(currentItem);
                        widget->_buttonTextUpdate();
                    }
                });
            }

            if (auto uiSystem = context->getSystemT<UI::UISystem>().lock())
            {
                p.dpiObserver = ValueObserver<int>::create(
                    uiSystem->getStyleSettings()->observeDPI(),
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = widget->_p->dpiToIndex.find(value);
                        if (i != widget->_p->dpiToIndex.end())
                        {
                            widget->_p->buttonGroup->setChecked(i->second);
                        }
                    }
                });
            }
        }

        DisplaySettingsWidget::DisplaySettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<DisplaySettingsWidget> DisplaySettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<DisplaySettingsWidget>(new DisplaySettingsWidget);
            out->_init(context);
            return out;
        }

        void DisplaySettingsWidget::_localeEvent(Event::Locale &)
        {
            setText(_getText(DJV_TEXT("djv::ViewLib", "Display")));
            _buttonTextUpdate();
        }

        void DisplaySettingsWidget::_styleEvent(Event::Style &)
        {
            if (auto style = _getStyle().lock())
            {
                for (auto i : _p->customStyles)
                {
                    i->setPalette(style->getPalette());
                    i->setMetrics(style->getMetrics());
                    i->setFont(style->getFont());
                }
            }
        }

        void DisplaySettingsWidget::_buttonTextUpdate()
        {
            DJV_PRIVATE_PTR();
            int j = 0;
            for (auto i : p.buttons)
            {
                const auto k = p.indexToDPI[j];
                std::stringstream ss;
                ss << k << " " << _getText(DJV_TEXT("djv::ViewLib", "DPI"));
                i->setText(ss.str());
                ++j;
            }
        }
        
    } // namespace ViewLib
} // namespace djv

