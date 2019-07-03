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

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
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
        struct DisplaySizeWidget::Private
        {
            std::vector<std::string> metrics;
            std::string currentMetrics;
            std::shared_ptr<ComboBox> comboBox;
            std::map<size_t, std::string> indexToMetrics;
            std::map<std::string, size_t> metricsToIndex;
            std::shared_ptr<MapObserver<std::string, Style::Metrics> > metricsObserver;
            std::shared_ptr<ValueObserver<std::string> > currentMetricsObserver;
        };

        void DisplaySizeWidget::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::Ui::DisplaySizeWidget");
            setHAlign(HAlign::Left);

            p.comboBox = ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<DisplaySizeWidget>(std::dynamic_pointer_cast<DisplaySizeWidget>(shared_from_this()));
            p.comboBox->setCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
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

            if (auto settingsSystem = context->getSystemT<Settings::System>())
            {
                if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                {
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
            }
        }

        DisplaySizeWidget::DisplaySizeWidget() :
            _p(new Private)
        {}

        std::shared_ptr<DisplaySizeWidget> DisplaySizeWidget::create(Context* context)
        {
            auto out = std::shared_ptr<DisplaySizeWidget>(new DisplaySizeWidget);
            out->_init(context);
            return out;
        }

        void DisplaySizeWidget::_preLayoutEvent(Core::Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void DisplaySizeWidget::_layoutEvent(Core::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void DisplaySizeWidget::_localeEvent(Event::Locale& event)
        {
            _widgetUpdate();
        }

        void DisplaySizeWidget::_widgetUpdate()
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

        void DisplaySizeWidget::_currentItemUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto i = p.metricsToIndex.find(p.currentMetrics);
            if (i != p.metricsToIndex.end())
            {
                p.comboBox->setCurrentItem(static_cast<int>(i->second));
            }
        }

        struct DisplaySizeSettingsWidget::Private
        {};

        void DisplaySizeSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);
            setClassName("djv::Ui::DisplaySizeSettingsWidget");
            addChild(DisplaySizeWidget::create(context));
        }

        DisplaySizeSettingsWidget::DisplaySizeSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<DisplaySizeSettingsWidget> DisplaySizeSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<DisplaySizeSettingsWidget>(new DisplaySizeSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string DisplaySizeSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Display Size");
        }

        std::string DisplaySizeSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("General");
        }

        std::string DisplaySizeSettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

        struct DisplayPaletteWidget::Private
        {
            std::vector<std::string> palettes;
            std::string currentPalette;
            std::shared_ptr<ComboBox> comboBox;
            std::map<size_t, std::string> indexToPalette;
            std::map<std::string, size_t> paletteToIndex;
            std::shared_ptr<MapObserver<std::string, Style::Palette> > palettesObserver;
            std::shared_ptr<ValueObserver<std::string> > currentPaletteObserver;
        };

        void DisplayPaletteWidget::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::Ui::DisplayPaletteWidget");
            setHAlign(HAlign::Left);

            p.comboBox = ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<DisplayPaletteWidget>(std::dynamic_pointer_cast<DisplayPaletteWidget>(shared_from_this()));
            p.comboBox->setCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
                    {
                        if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                        {
                            const auto i = widget->_p->indexToPalette.find(value);
                            if (i != widget->_p->indexToPalette.end())
                            {
                                styleSettings->setCurrentPalette(i->second);
                            }
                        }
                    }
                }
            });

            if (auto settingsSystem = context->getSystemT<Settings::System>())
            {
                if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                {
                    p.palettesObserver = MapObserver<std::string, Style::Palette>::create(
                        styleSettings->observePalettes(),
                        [weak](const std::map<std::string, Style::Palette > & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->palettes.clear();
                            for (const auto& i : value)
                            {
                                widget->_p->palettes.push_back(i.first);
                            }
                            widget->_widgetUpdate();
                        }
                    });
                    p.currentPaletteObserver = ValueObserver<std::string>::create(
                        styleSettings->observeCurrentPaletteName(),
                        [weak](const std::string & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->currentPalette = value;
                            widget->_currentItemUpdate();
                        }
                    });
                }
            }
        }

        DisplayPaletteWidget::DisplayPaletteWidget() :
            _p(new Private)
        {}

        std::shared_ptr<DisplayPaletteWidget> DisplayPaletteWidget::create(Context* context)
        {
            auto out = std::shared_ptr<DisplayPaletteWidget>(new DisplayPaletteWidget);
            out->_init(context);
            return out;
        }

        void DisplayPaletteWidget::_preLayoutEvent(Core::Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void DisplayPaletteWidget::_layoutEvent(Core::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void DisplayPaletteWidget::_localeEvent(Event::Locale& event)
        {
            _widgetUpdate();
        }

        void DisplayPaletteWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.comboBox->clearItems();
            p.indexToPalette.clear();
            p.paletteToIndex.clear();
            for (size_t i = 0; i < p.palettes.size(); ++i)
            {
                const auto& name = p.palettes[i];
                p.comboBox->addItem(_getText(name));
                p.indexToPalette[i] = name;
                p.paletteToIndex[name] = i;
            }
            _currentItemUpdate();
        }

        void DisplayPaletteWidget::_currentItemUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto i = p.paletteToIndex.find(p.currentPalette);
            if (i != p.paletteToIndex.end())
            {
                p.comboBox->setCurrentItem(static_cast<int>(i->second));
            }
        }

        struct DisplayPaletteSettingsWidget::Private
        {};

        void DisplayPaletteSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);
            setClassName("djv::Ui::DisplayPaletteSettingsWidget");
            addChild(DisplayPaletteWidget::create(context));
        }

        DisplayPaletteSettingsWidget::DisplayPaletteSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<DisplayPaletteSettingsWidget> DisplayPaletteSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<DisplayPaletteSettingsWidget>(new DisplayPaletteSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string DisplayPaletteSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Palette");
        }

        std::string DisplayPaletteSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("General");
        }

        std::string DisplayPaletteSettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

    } // namespace UI
} // namespace djv

