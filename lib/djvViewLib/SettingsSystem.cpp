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
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>
#include <djvUI/ScrollWidget.h>
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
            class SettingsWidget : public UI::Layout::Vertical
            {
                DJV_NON_COPYABLE(SettingsWidget);

            protected:
                void _init(Context * context)
                {
                    Vertical::_init(context);

                    setSpacing(UI::Style::MetricsRole::None);
                    setBackgroundRole(UI::Style::ColorRole::Background);
                    setPointerEnabled(true);
                    
                    _titleLabel = UI::Label::create(context);
                    _titleLabel->setFontSizeRole(UI::Style::MetricsRole::FontHeader);
                    _titleLabel->setTextHAlign(UI::TextHAlign::Left);
                    _titleLabel->setTextColorRole(UI::Style::ColorRole::ForegroundHeader);
                    _titleLabel->setMargin(UI::Layout::Margin(
                        UI::Style::MetricsRole::Margin,
                        UI::Style::MetricsRole::None,
                        UI::Style::MetricsRole::MarginSmall,
                        UI::Style::MetricsRole::MarginSmall));
                    
                    _closeButton = UI::Button::Tool::create(context);
                    _closeButton->setIcon("djvIconClose");
                    _closeButton->setForegroundColorRole(UI::Style::ColorRole::ForegroundHeader);
                    _closeButton->setCheckedForegroundColorRole(UI::Style::ColorRole::ForegroundHeader);
                    _closeButton->setInsideMargin(UI::Style::MetricsRole::MarginSmall);
                    
                    std::map<std::string, std::shared_ptr<UI::Widget> > settingsWidgets;
                    for (auto i : context->getSystemsT<IViewSystem>())
                    {
                        if (auto system = i.lock())
                        {
                            for (auto j : system->createSettingsWidgets())
                            {
                                settingsWidgets[j.sortKey] = j.widget;
                            }
                        }
                    }
                    auto settingsLayout = UI::Layout::Vertical::create(context);
                    settingsLayout->setMargin(UI::Style::MetricsRole::Margin);
                    for (auto i : settingsWidgets)
                    {
                        settingsLayout->addWidget(i.second);
                    }
                    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                    scrollWidget->setBorder(false);
                    scrollWidget->setBackgroundRole(UI::Style::ColorRole::Background);
                    scrollWidget->addWidget(settingsLayout);
                    
                    auto hLayout = UI::Layout::Horizontal::create(context);
                    hLayout->setSpacing(UI::Style::MetricsRole::None);
                    hLayout->setBackgroundRole(UI::Style::ColorRole::BackgroundHeader);
                    hLayout->addWidget(_titleLabel, UI::Layout::RowStretch::Expand);
                    hLayout->addWidget(_closeButton);
                    addWidget(hLayout);
                    addWidget(scrollWidget, UI::Layout::RowStretch::Expand);
                }

                SettingsWidget()
                {}

            public:
                static std::shared_ptr<SettingsWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
                    out->_init(context);
                    return out;
                }
                
                void setCloseCallback(const std::function<void(void)> & value)
                {
                    _closeButton->setClickedCallback(value);
                }

            protected:
                void _localeEvent(Event::Locale &) override
                {
                    _titleLabel->setText(_getText(DJV_TEXT("djv::ViewLib", "Settings")));
                }

            private:
                std::shared_ptr<UI::Label> _titleLabel;
                std::shared_ptr<UI::Button::Tool> _closeButton;
            };
            
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
            return
            {
                { DisplaySettingsWidget::create(getContext()), "HA" },
                { LanguageSettingsWidget::create(getContext()), "HB" },
                { PaletteSettingsWidget::create(getContext()), "HC" }
            };
        }
        
    } // namespace ViewLib
} // namespace djv

