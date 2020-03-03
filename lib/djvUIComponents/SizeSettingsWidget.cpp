//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUIComponents/SizeSettingsWidget.h>

#include <djvUI/ComboBox.h>
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
        struct SizeWidget::Private
        {
            std::vector<std::string> metrics;
            std::string currentMetrics;
            std::shared_ptr<ComboBox> comboBox;
            std::map<size_t, std::string> indexToMetrics;
            std::map<std::string, size_t> metricsToIndex;
            std::shared_ptr<MapObserver<std::string, Style::Metrics> > metricsObserver;
            std::shared_ptr<ValueObserver<std::string> > currentMetricsObserver;
        };

        void SizeWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::SizeWidget");
            setHAlign(HAlign::Left);

            p.comboBox = ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<SizeWidget>(std::dynamic_pointer_cast<SizeWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            auto styleSettings = settingsSystem->getSettingsT<Settings::Style>();
                            const auto i = widget->_p->indexToMetrics.find(value);
                            if (i != widget->_p->indexToMetrics.end())
                            {
                                styleSettings->setCurrentMetrics(i->second);
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<Settings::System>();
            auto styleSettings = settingsSystem->getSettingsT<Settings::Style>();
            p.metricsObserver = MapObserver<std::string, Style::Metrics>::create(
                styleSettings->observeMetrics(),
                [weak](const std::map<std::string, Style::Metrics> & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->metrics.clear();
                    for (const auto& i : value)
                    {
                        widget->_p->metrics.push_back(i.first);
                    }
                    widget->_widgetUpdate();
                }
            });
            p.currentMetricsObserver = ValueObserver<std::string>::create(
                styleSettings->observeCurrentMetricsName(),
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->currentMetrics = value;
                    widget->_currentItemUpdate();
                }
            });
        }

        SizeWidget::SizeWidget() :
            _p(new Private)
        {}

        std::shared_ptr<SizeWidget> SizeWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<SizeWidget>(new SizeWidget);
            out->_init(context);
            return out;
        }

        void SizeWidget::_preLayoutEvent(Core::Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void SizeWidget::_layoutEvent(Core::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void SizeWidget::_initEvent(Event::Init& event)
        {
            Widget::_initEvent(event);
            _widgetUpdate();
        }

        void SizeWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.comboBox->clearItems();
            p.indexToMetrics.clear();
            p.metricsToIndex.clear();
            for (size_t i = 0; i < p.metrics.size(); ++i)
            {
                const auto& name = p.metrics[i];
                p.comboBox->addItem(_getText(name));
                p.indexToMetrics[i] = name;
                p.metricsToIndex[name] = i;
            }
            _currentItemUpdate();
        }

        void SizeWidget::_currentItemUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto i = p.metricsToIndex.find(p.currentMetrics);
            if (i != p.metricsToIndex.end())
            {
                p.comboBox->setCurrentItem(static_cast<int>(i->second));
            }
        }

        struct SizeSettingsWidget::Private
        {};

        void SizeSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            setClassName("djv::UI::SizeSettingsWidget");
            addChild(SizeWidget::create(context));
        }

        SizeSettingsWidget::SizeSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<SizeSettingsWidget> SizeSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<SizeSettingsWidget>(new SizeSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string SizeSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_general_section_size");
        }

        std::string SizeSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string SizeSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

    } // namespace UI
} // namespace djv

