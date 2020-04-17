// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/PaletteSettingsWidget.h>

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

        void PaletteWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::PaletteWidget");
            setHAlign(HAlign::Left);

            p.comboBox = ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<PaletteWidget>(std::dynamic_pointer_cast<PaletteWidget>(shared_from_this()));
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
                            const auto i = widget->_p->indexToPalette.find(value);
                            if (i != widget->_p->indexToPalette.end())
                            {
                                styleSettings->setCurrentPalette(i->second);
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<Settings::System>();
            auto styleSettings = settingsSystem->getSettingsT<Settings::Style>();
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

        PaletteWidget::PaletteWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PaletteWidget> PaletteWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PaletteWidget>(new PaletteWidget);
            out->_init(context);
            return out;
        }

        void PaletteWidget::_preLayoutEvent(Core::Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void PaletteWidget::_layoutEvent(Core::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void PaletteWidget::_initEvent(Event::Init& event)
        {
            Widget::_initEvent(event);
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

        struct PaletteSettingsWidget::Private
        {};

        void PaletteSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            setClassName("djv::UI::PaletteSettingsWidget");
            addChild(PaletteWidget::create(context));
        }

        PaletteSettingsWidget::PaletteSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PaletteSettingsWidget> PaletteSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PaletteSettingsWidget>(new PaletteSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string PaletteSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_general_section_palette");
        }

        std::string PaletteSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string PaletteSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

    } // namespace UI
} // namespace djv

