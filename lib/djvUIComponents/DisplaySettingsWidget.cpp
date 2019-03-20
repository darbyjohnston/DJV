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

#include <djvUIComponents/DisplaySettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlatButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct DisplaySettingsWidget::Private
        {
            std::vector<std::shared_ptr<FlatButton> > buttons;
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<VerticalLayout> layout;
            std::map<int, std::string> indexToMetrics;
            std::map<std::shared_ptr<FlatButton>, std::string> buttonToMetrics;
            std::map<std::string, int> metricsToIndex;
            std::shared_ptr<MapObserver<std::string, Style::Metrics> > metricsObserver;
            std::shared_ptr<ValueObserver<std::string> > currentMetricsObserver;
        };

        void DisplaySettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            addChild(p.layout);

            auto weak = std::weak_ptr<DisplaySettingsWidget>(std::dynamic_pointer_cast<DisplaySettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                        {
                            const auto i = widget->_p->indexToMetrics.find(value);
                            if (i != widget->_p->indexToMetrics.end())
                            {
                                styleSettings->setCurrentMetrics(i->second);
                            }
                        }
                    }
                }
            });

            if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
            {
                if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                {
                    p.metricsObserver = MapObserver<std::string, Style::Metrics>::create(
                        styleSettings->observeMetrics(),
                        [weak, context](const std::map<std::string, Style::Metrics> & value)
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
                            widget->_p->layout->clearChildren();
                            widget->_p->indexToMetrics.clear();
                            widget->_p->buttonToMetrics.clear();
                            widget->_p->metricsToIndex.clear();
                            int j = 0;
                            for (const auto & i : value)
                            {
                                auto button = FlatButton::create(context);
                                widget->_p->buttons.push_back(button);
                                widget->_p->buttonGroup->addButton(button);
                                widget->_p->layout->addChild(button);
                                widget->_p->indexToMetrics[j] = i.first;
                                widget->_p->buttonToMetrics[button] = i.first;
                                widget->_p->metricsToIndex[i.first] = j;
                                ++j;
                            }
                            widget->_p->buttonGroup->setChecked(currentItem);
                            widget->_buttonTextUpdate();
                        }
                    });
                    p.currentMetricsObserver = ValueObserver<std::string>::create(
                        styleSettings->observeCurrentMetricsName(),
                        [weak](const std::string & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            const auto i = widget->_p->metricsToIndex.find(value);
                            if (i != widget->_p->metricsToIndex.end())
                            {
                                widget->_p->buttonGroup->setChecked(static_cast<int>(i->second));
                            }
                        }
                    });
                }
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

        std::string DisplaySettingsWidget::getName() const
        {
            return DJV_TEXT("Display Size");
        }

        std::string DisplaySettingsWidget::getSortKey() const
        {
            return "A";
        }

        void DisplaySettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            _buttonTextUpdate();
        }

        void DisplaySettingsWidget::_buttonTextUpdate()
        {
            DJV_PRIVATE_PTR();
            for (auto i : p.buttons)
            {
                const auto j = p.buttonToMetrics.find(i);
                if (j != p.buttonToMetrics.end())
                {
                    i->setText(_getText(j->second));
                }
            }
        }

    } // namespace UI
} // namespace djv

