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

#include <djvViewLib/SettingsSystem.h>

#include <djvViewLib/DisplaySettingsWidget.h>
#include <djvViewLib/LanguageSettingsWidget.h>
#include <djvViewLib/PaletteSettingsWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/GroupBox.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/ToolButton.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Splitter.h>
#include <djvUI/Overlay.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            class SettingsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(SettingsWidget);

            protected:
                void _init(Context *);

                SettingsWidget();

            public:
                static std::shared_ptr<SettingsWidget> create(Context *);
                
                void setCloseCallback(const std::function<void(void)> &);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;
                void _buttonPressEvent(Event::ButtonPress&) override;
                void _buttonReleaseEvent(Event::ButtonRelease&) override;

                void _localeEvent(Event::Locale &) override;

            private:
                std::shared_ptr<UI::Label> _titleLabel;
                std::shared_ptr<UI::Button::Tool> _closeButton;
                std::shared_ptr<UI::Button::Group> _buttonGroup;
                std::map<std::string, std::string> _names;
                std::map<std::string, std::shared_ptr<UI::Widget> > _widgets;
                std::map<std::string, std::shared_ptr<UI::Button::List> > _buttons;
                std::shared_ptr<UI::Layout::Vertical> _layout;
            };

            void SettingsWidget::_init(Context * context)
            {
                Widget::_init(context);

                setPointerEnabled(true);

                _titleLabel = UI::Label::create(context);
                _titleLabel->setFontSizeRole(UI::Style::MetricsRole::FontHeader);
                _titleLabel->setTextHAlign(UI::TextHAlign::Left);
                _titleLabel->setTextColorRole(UI::Style::ColorRole::ForegroundHeader);
                _titleLabel->setMargin(UI::Layout::Margin(
                    UI::Style::MetricsRole::MarginLarge,
                    UI::Style::MetricsRole::None,
                    UI::Style::MetricsRole::Margin,
                    UI::Style::MetricsRole::Margin));

                _closeButton = UI::Button::Tool::create(context);
                _closeButton->setIcon("djvIconClose");
                _closeButton->setForegroundColorRole(UI::Style::ColorRole::ForegroundHeader);
                _closeButton->setCheckedForegroundColorRole(UI::Style::ColorRole::ForegroundHeader);
                _closeButton->setInsideMargin(UI::Style::MetricsRole::MarginSmall);

                _buttonGroup = UI::Button::Group::create(UI::ButtonType::Radio);
                for (auto i : context->getSystemsT<IViewSystem>())
                {
                    if (auto system = i.lock())
                    {
                        for (auto widget : system->createSettingsWidgets())
                        {
                            _names[widget.sortKey] = widget.name;
                            _widgets[widget.sortKey] = widget.widget;
                            auto button = UI::Button::List::create(context);
                            button->setTextHAlign(UI::TextHAlign::Left);
                            button->setInsideMargin(UI::Style::MetricsRole::Margin);
                            _buttons[widget.sortKey] = button;
                        }
                    }
                }

                auto buttonLayout = UI::Layout::Vertical::create(context);
                buttonLayout->setSpacing(UI::Style::MetricsRole::None);
                for (auto i : _buttons)
                {
                    _buttonGroup->addButton(i.second);
                    buttonLayout->addWidget(i.second);
                }
                auto buttonScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                buttonScrollWidget->addWidget(buttonLayout);

                auto soloLayout = UI::Layout::Solo::create(context);
                soloLayout->setMargin(UI::Style::MetricsRole::MarginLarge);
                for (auto i : _widgets)
                {
                    soloLayout->addWidget(i.second);
                }
                auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                scrollWidget->setBackgroundRole(UI::Style::ColorRole::Background);
                scrollWidget->addWidget(soloLayout);

                _layout = UI::Layout::Vertical::create(context);
                _layout->setSpacing(UI::Style::MetricsRole::None);
                auto hLayout = UI::Layout::Horizontal::create(context);
                hLayout->setSpacing(UI::Style::MetricsRole::None);
                hLayout->setBackgroundRole(UI::Style::ColorRole::BackgroundHeader);
                hLayout->addWidget(_titleLabel, UI::Layout::RowStretch::Expand);
                hLayout->addWidget(_closeButton);
                _layout->addWidget(hLayout);
                _layout->addSeparator();
                auto splitter = UI::Layout::Splitter::create(UI::Orientation::Horizontal, context);
                splitter->setBackgroundRole(UI::Style::ColorRole::Background);
                splitter->setMargin(UI::Style::MetricsRole::Margin);
                splitter->addWidget(buttonScrollWidget);
                splitter->addWidget(scrollWidget);
                splitter->setSplit(.15f);
                _layout->addWidget(splitter, UI::Layout::RowStretch::Expand);
                _layout->setParent(shared_from_this());

                auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
                _buttonGroup->setRadioCallback(
                    [soloLayout](int value)
                {
                    soloLayout->setCurrentIndex(value);
                });
            }

            SettingsWidget::SettingsWidget()
            {}

            std::shared_ptr<SettingsWidget> SettingsWidget::create(Context * context)
            {
                auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
                out->_init(context);
                return out;
            }

            void SettingsWidget::setCloseCallback(const std::function<void(void)> & value)
            {
                _closeButton->setClickedCallback(value);
            }

            float SettingsWidget::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void SettingsWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void SettingsWidget::_layoutEvent(Event::Layout &)
            {
                _layout->setGeometry(getGeometry());
            }

            void SettingsWidget::_buttonPressEvent(Event::ButtonPress& event)
            {
                event.accept();
            }

            void SettingsWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                event.accept();
            }

            void SettingsWidget::_localeEvent(Event::Locale &)
            {
                _titleLabel->setText(_getText(DJV_TEXT("djv::ViewLib", "Settings")));
                for (const auto & i : _names)
                {
                    const auto j = _buttons.find(i.first);
                    if (j != _buttons.end())
                    {
                        j->second->setText(_getText(i.second));
                    }
                }
            }

            class GeneralSettingsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(GeneralSettingsWidget);

            protected:
                void _init(Context *);

                GeneralSettingsWidget();

            public:
                static std::shared_ptr<GeneralSettingsWidget> create(Context *);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                std::shared_ptr<UI::Layout::GroupBox> _displayGroupBox;
                std::shared_ptr<UI::Layout::GroupBox> _languageGroupBox;
                std::shared_ptr<UI::Layout::GroupBox> _paletteGroupBox;
                std::shared_ptr<UI::Layout::Vertical> _layout;
            };

            void GeneralSettingsWidget::_init(Context * context)
            {
                Widget::_init(context);

                _displayGroupBox = UI::Layout::GroupBox::create(context);
                _displayGroupBox->addWidget(DisplaySettingsWidget::create(context));

                _languageGroupBox = UI::Layout::GroupBox::create(context);
                _languageGroupBox->addWidget(LanguageSettingsWidget::create(context));

                _paletteGroupBox = UI::Layout::GroupBox::create(context);
                _paletteGroupBox->addWidget(PaletteSettingsWidget::create(context));

                _layout = UI::Layout::Vertical::create(context);
                _layout->setSpacing(UI::Style::MetricsRole::SpacingLarge);
                _layout->addWidget(_displayGroupBox);
                _layout->addWidget(_languageGroupBox);
                _layout->addWidget(_paletteGroupBox);
                _layout->setParent(shared_from_this());
            }

            GeneralSettingsWidget::GeneralSettingsWidget()
            {}

            std::shared_ptr<GeneralSettingsWidget> GeneralSettingsWidget::create(Context * context)
            {
                auto out = std::shared_ptr<GeneralSettingsWidget>(new GeneralSettingsWidget);
                out->_init(context);
                return out;
            }

            float GeneralSettingsWidget::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void GeneralSettingsWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void GeneralSettingsWidget::_layoutEvent(Event::Layout &)
            {
                _layout->setGeometry(getGeometry());
            }

            void GeneralSettingsWidget::_localeEvent(Event::Locale &)
            {
                _displayGroupBox->setText(_getText(DJV_TEXT("djv::ViewLib", "Display")));
                _languageGroupBox->setText(_getText(DJV_TEXT("djv::ViewLib", "Language")));
                _paletteGroupBox->setText(_getText(DJV_TEXT("djv::ViewLib", "Palette")));
            }

        } // namespace
        
        struct SettingsSystem::Private
        {
            std::shared_ptr<SettingsWidget> settingsWidget;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void SettingsSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::SettingsSystem", context);
        }

        SettingsSystem::SettingsSystem() :
            _p(new Private)
        {}

        SettingsSystem::~SettingsSystem()
        {}

        std::shared_ptr<SettingsSystem> SettingsSystem::create(Context * context)
        {
            auto out = std::shared_ptr<SettingsSystem>(new SettingsSystem);
            out->_init(context);
            return out;
        }

        void SettingsSystem::showSettingsDialog()
        {
            auto context = getContext();
            if (!_p->settingsWidget)
            {
                _p->settingsWidget = SettingsWidget::create(context);

                _p->overlay = UI::Layout::Overlay::create(context);
                _p->overlay->setBackgroundRole(UI::Style::ColorRole::Overlay);
                _p->overlay->setMargin(UI::Style::MetricsRole::MarginDialog);
                _p->overlay->addWidget(_p->settingsWidget);
            }
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    window->addWidget(_p->overlay);
                    _p->overlay->show();

                    auto weak = std::weak_ptr<SettingsSystem>(std::dynamic_pointer_cast<SettingsSystem>(shared_from_this()));
                    _p->settingsWidget->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->overlay);
                        }
                    });
                    _p->overlay->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->overlay);
                        }
                    });
                }
            }
        }
        
        std::map<std::string, std::shared_ptr<UI::Action> > SettingsSystem::getActions()
        {
            return _p->actions;
        }

        std::vector<NewSettingsWidget> SettingsSystem::createSettingsWidgets()
        {
            return { { GeneralSettingsWidget::create(getContext()), DJV_TEXT("djv::ViewLib", "General"), "A", } };
        }
        
    } // namespace ViewLib
} // namespace djv

