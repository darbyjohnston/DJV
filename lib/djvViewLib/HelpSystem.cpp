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

#include <djvViewLib/HelpSystem.h>

#include <djvUI/Action.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolButton.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Overlay.h>
#include <djvUI/Window.h>

#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            class AboutWidget : public UI::Layout::Vertical
            {
                DJV_NON_COPYABLE(AboutWidget);

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

                    _textBlocks["Header"] = UI::TextBlock::create(context);
                    _textBlocks["Header"]->setFontSizeRole(UI::Style::MetricsRole::FontHeader);
                    _textBlocks["CopyrightHeader"] = UI::TextBlock::create(context);
                    _textBlocks["CopyrightHeader"]->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
                    _textBlocks["Copyright"] = UI::TextBlock::create(context);
                    _textBlocks["LicenseHeader"] = UI::TextBlock::create(context);
                    _textBlocks["LicenseHeader"]->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
                    _textBlocks["License"] = UI::TextBlock::create(context);
                    _textBlocks["ContributorsHeader"] = UI::TextBlock::create(context);
                    _textBlocks["ContributorsHeader"]->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
                    _textBlocks["Contributors"] = UI::TextBlock::create(context);
                    _textBlocks["ThirdPartyHeader"] = UI::TextBlock::create(context);
                    _textBlocks["ThirdPartyHeader"]->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
                    _textBlocks["ThirdParty"] = UI::TextBlock::create(context);
                    _textBlocks["TrademarksHeader"] = UI::TextBlock::create(context);
                    _textBlocks["TrademarksHeader"]->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
                    _textBlocks["Trademarks"] = UI::TextBlock::create(context);

                    auto textLayout = UI::Layout::Vertical::create(context);
                    textLayout->setMargin(UI::Style::MetricsRole::MarginLarge);
                    textLayout->setSpacing(UI::Style::MetricsRole::SpacingLarge);
                    textLayout->addWidget(_textBlocks["Header"]);
                    textLayout->addSeparator();
                    textLayout->addWidget(_textBlocks["CopyrightHeader"]);
                    textLayout->addWidget(_textBlocks["Copyright"]);
                    textLayout->addSeparator();
                    textLayout->addWidget(_textBlocks["LicenseHeader"]);
                    textLayout->addWidget(_textBlocks["License"]);
                    textLayout->addSeparator();
                    textLayout->addWidget(_textBlocks["ContributorsHeader"]);
                    textLayout->addWidget(_textBlocks["Contributors"]);
                    textLayout->addSeparator();
                    textLayout->addWidget(_textBlocks["ThirdPartyHeader"]);
                    textLayout->addWidget(_textBlocks["ThirdParty"]);
                    textLayout->addSeparator();
                    textLayout->addWidget(_textBlocks["TrademarksHeader"]);
                    textLayout->addWidget(_textBlocks["Trademarks"]);

                    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                    scrollWidget->addWidget(textLayout);

                    auto hLayout = UI::Layout::Horizontal::create(context);
                    hLayout->setSpacing(UI::Style::MetricsRole::None);
                    hLayout->setBackgroundRole(UI::Style::ColorRole::BackgroundHeader);
                    hLayout->addWidget(_titleLabel, UI::Layout::RowStretch::Expand);
                    hLayout->addWidget(_closeButton);
                    addWidget(hLayout);
                    addWidget(scrollWidget, UI::Layout::RowStretch::Expand);
                }

                AboutWidget()
                {}

            public:
                static std::shared_ptr<AboutWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<AboutWidget>(new AboutWidget);
                    out->_init(context);
                    return out;
                }

                void setCloseCallback(const std::function<void(void)> & value)
                {
                    _closeButton->setClickedCallback(value);
                }

            protected:
                void _buttonPressEvent(Event::ButtonPress& event) override
                {
                    event.accept();
                }

                void _buttonReleaseEvent(Event::ButtonRelease& event) override
                {
                    event.accept();
                }

                void _localeEvent(Event::Locale &) override
                {
                    _titleLabel->setText(_getText(DJV_TEXT("djv::ViewLib", "About")));
                    _textBlocks["Header"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutHeader")));
                    _textBlocks["CopyrightHeader"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutCopyrightHeader")));
                    _textBlocks["Copyright"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutCopyright")));
                    _textBlocks["LicenseHeader"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutLicenseHeader")));
                    _textBlocks["License"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutLicense")));
                    _textBlocks["ContributorsHeader"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutContributorsHeader")));
                    _textBlocks["Contributors"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutContributors")));
                    _textBlocks["ThirdPartyHeader"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutThirdPartyHeader")));
                    _textBlocks["ThirdParty"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutThirdParty")));
                    _textBlocks["TrademarksHeader"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutTrademarksHeader")));
                    _textBlocks["Trademarks"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutTrademarks")));
                }

            private:
                std::shared_ptr<UI::Label> _titleLabel;
                std::shared_ptr<UI::Button::Tool> _closeButton;
                std::map<std::string, std::shared_ptr<UI::TextBlock> > _textBlocks;
                std::future<std::shared_ptr<AV::Image::Image> > _imageFuture;
            };

        } // namespace

        struct HelpSystem::Private
        {
            std::shared_ptr<AboutWidget> aboutWidget;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void HelpSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::HelpSystem", context);

            DJV_PRIVATE_PTR();
            //! \todo Implement me!
            p.actions["Documentation"] = UI::Action::create();
            p.actions["Documentation"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["Information"] = UI::Action::create();
            p.actions["Information"]->setEnabled(false);

            p.actions["About"] = UI::Action::create();

            auto weak = std::weak_ptr<HelpSystem>(std::dynamic_pointer_cast<HelpSystem>(shared_from_this()));
            p.clickedObservers["About"] = ValueObserver<bool>::create(
                p.actions["About"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        system->showAboutDialog();
                    }
                }
            });
        }

        HelpSystem::HelpSystem() :
            _p(new Private)
        {}

        HelpSystem::~HelpSystem()
        {}

        std::shared_ptr<HelpSystem> HelpSystem::create(Context * context)
        {
            auto out = std::shared_ptr<HelpSystem>(new HelpSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > HelpSystem::getActions()
        {
            return _p->actions;
        }

        void HelpSystem::showAboutDialog()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            if (!p.aboutWidget)
            {
                p.aboutWidget = AboutWidget::create(context);

                p.overlay = UI::Layout::Overlay::create(context);
                p.overlay->setBackgroundRole(UI::Style::ColorRole::Overlay);
                p.overlay->setMargin(UI::Style::MetricsRole::MarginDialog);
                p.overlay->addWidget(p.aboutWidget);
            }
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    window->addWidget(p.overlay);
                    p.overlay->show();

                    auto weak = std::weak_ptr<HelpSystem>(std::dynamic_pointer_cast<HelpSystem>(shared_from_this()));
                    p.aboutWidget->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->overlay);
                        }
                    });
                    p.overlay->setCloseCallback(
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

        NewMenu HelpSystem::createMenu()
        {
            DJV_PRIVATE_PTR();
            p.menus["Help"] = UI::Menu::create(_getText("Help"), getContext());
            p.menus["Help"]->addAction(p.actions["Documentation"]);
            p.menus["Help"]->addAction(p.actions["Information"]);
            p.menus["Help"]->addAction(p.actions["About"]);
            return { p.menus["Help"], "G" };
        }

        void HelpSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Documentation"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Documentation")));
            p.actions["Information"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Information")));
            p.actions["About"]->setText(_getText(DJV_TEXT("djv::ViewLib", "About")));

            p.menus["Help"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib", "Help")));
        }

    } // namespace ViewLib
} // namespace djv

