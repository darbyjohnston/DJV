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
        namespace
        {
            class DisplaySizeWidget : public Widget
            {
                DJV_NON_COPYABLE(DisplaySizeWidget);

            protected:
                void _init(Context*);
                DisplaySizeWidget();

            public:
                static std::shared_ptr<DisplaySizeWidget> create(Context*);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _localeEvent(Event::Locale&) override;

            private:
                void _widgetUpdate();
                void _currentItemUpdate();

                DJV_PRIVATE();
            };

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
                            [weak, context](const std::map<std::string, Style::Metrics> & value)
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

            float DisplaySizeWidget::getHeightForWidth(float value) const
            {
                return _p->comboBox->getHeightForWidth(value);
            }

            void DisplaySizeWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_p->comboBox->getMinimumSize());
            }

            void DisplaySizeWidget::_layoutEvent(Event::Layout&)
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

            class PaletteWidget : public Widget
            {
                DJV_NON_COPYABLE(PaletteWidget);

            protected:
                void _init(Context*);
                PaletteWidget();

            public:
                static std::shared_ptr<PaletteWidget> create(Context*);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _localeEvent(Event::Locale&) override;

            private:
                void _widgetUpdate();
                void _currentItemUpdate();

                DJV_PRIVATE();
            };

            struct PaletteWidget::Private
            {
                std::vector<std::string> palettes;
                std::string currentPalette;
                std::shared_ptr<ComboBox> comboBox;
                std::map<size_t, std::string> indexToPalette;
                std::map<std::string, size_t> paletteToIndex;
                std::shared_ptr<MapObserver<std::string, Style::Palette> > palettesObserver;
                std::shared_ptr<ValueObserver<std::string> > currentPaletteObserver;
            };

            void PaletteWidget::_init(Context* context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.comboBox = ComboBox::create(context);
                addChild(p.comboBox);

                auto weak = std::weak_ptr<PaletteWidget>(std::dynamic_pointer_cast<PaletteWidget>(shared_from_this()));
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

            PaletteWidget::PaletteWidget() :
                _p(new Private)
            {}

            std::shared_ptr<PaletteWidget> PaletteWidget::create(Context* context)
            {
                auto out = std::shared_ptr<PaletteWidget>(new PaletteWidget);
                out->_init(context);
                return out;
            }

            float PaletteWidget::getHeightForWidth(float value) const
            {
                return _p->comboBox->getHeightForWidth(value);
            }

            void PaletteWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_p->comboBox->getMinimumSize());
            }

            void PaletteWidget::_layoutEvent(Event::Layout&)
            {
                _p->comboBox->setGeometry(getGeometry());
            }

            void PaletteWidget::_localeEvent(Event::Locale& event)
            {
                _widgetUpdate();
            }

            void PaletteWidget::_widgetUpdate()
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

            void PaletteWidget::_currentItemUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto i = p.paletteToIndex.find(p.currentPalette);
                if (i != p.paletteToIndex.end())
                {
                    p.comboBox->setCurrentItem(static_cast<int>(i->second));
                }
            }

        } // namespace

        struct DisplaySettingsWidget::Private
        {
            std::shared_ptr<DisplaySizeWidget> sizeWidget;
            std::shared_ptr<PaletteWidget> paletteWidget;
            std::shared_ptr<FormLayout> formLayout;
        };

        void DisplaySettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.sizeWidget = DisplaySizeWidget::create(context);
            p.paletteWidget = PaletteWidget::create(context);

            p.formLayout = FormLayout::create(context);
            p.formLayout->addChild(p.sizeWidget);
            p.formLayout->addChild(p.paletteWidget);
            addChild(p.formLayout);
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

        std::string DisplaySettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Display");
        }

        std::string DisplaySettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("General");
        }

        std::string DisplaySettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

        void DisplaySettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.sizeWidget, _getText(DJV_TEXT("Size")) + ":");
            p.formLayout->setText(p.paletteWidget, _getText(DJV_TEXT("Palette")) + ":");
        }

    } // namespace UI
} // namespace djv

