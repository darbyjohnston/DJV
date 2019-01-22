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

#include <djvViewLib/HelpAboutDialog.h>

#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TextBlock.h>

#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct HelpAboutDialog::Private
        {
            std::map<std::string, std::shared_ptr<UI::TextBlock> > textBlocks;
            std::future<std::shared_ptr<AV::Image::Image> > imageFuture;
        };

        void HelpAboutDialog::_init(Context * context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            p.textBlocks["Header"] = UI::TextBlock::create(context);
            p.textBlocks["Header"]->setFontSizeRole(UI::Style::MetricsRole::FontHeader);
            p.textBlocks["Copyright"] = UI::TextBlock::create(context);
            p.textBlocks["License"] = UI::TextBlock::create(context);
            p.textBlocks["ContributorsHeader"] = UI::TextBlock::create(context);
            p.textBlocks["ContributorsHeader"]->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
            p.textBlocks["Contributors"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdPartyHeader"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdPartyHeader"]->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
            p.textBlocks["ThirdParty"] = UI::TextBlock::create(context);
            p.textBlocks["TrademarksHeader"] = UI::TextBlock::create(context);
            p.textBlocks["TrademarksHeader"]->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
            p.textBlocks["Trademarks"] = UI::TextBlock::create(context);

            auto textLayout = UI::Layout::Vertical::create(context);
            textLayout->setMargin(UI::Style::MetricsRole::MarginLarge);
            textLayout->setSpacing(UI::Style::MetricsRole::SpacingLarge);
            textLayout->addWidget(p.textBlocks["Header"]);
            textLayout->addWidget(p.textBlocks["Copyright"]);
            textLayout->addWidget(p.textBlocks["License"]);
            textLayout->addSeparator();
            textLayout->addWidget(p.textBlocks["ContributorsHeader"]);
            textLayout->addWidget(p.textBlocks["Contributors"]);
            textLayout->addSeparator();
            textLayout->addWidget(p.textBlocks["ThirdPartyHeader"]);
            textLayout->addWidget(p.textBlocks["ThirdParty"]);
            textLayout->addSeparator();
            textLayout->addWidget(p.textBlocks["TrademarksHeader"]);
            textLayout->addWidget(p.textBlocks["Trademarks"]);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->addWidget(textLayout);

            auto layout = UI::Layout::Vertical::create(context);
            layout->setMargin(UI::Style::MetricsRole::Margin);
            layout->addWidget(scrollWidget, UI::Layout::RowStretch::Expand);

            addWidget(layout, UI::Layout::RowStretch::Expand);
        }

        HelpAboutDialog::HelpAboutDialog() :
            _p(new Private)
        {}

        HelpAboutDialog::~HelpAboutDialog()
        {}

        std::shared_ptr<HelpAboutDialog> HelpAboutDialog::create(Context * context)
        {
            auto out = std::shared_ptr<HelpAboutDialog>(new HelpAboutDialog);
            out->_init(context);
            return out;
        }

        void HelpAboutDialog::_localeEvent(Event::Locale &)
        {
            setTitle(_getText(DJV_TEXT("djv::ViewLib", "About")));
            std::stringstream ss;
            ss << _getText(DJV_TEXT("djv::ViewLib", "AboutHeader"));
            ss << " " << DJV_VERSION;
            DJV_PRIVATE_PTR();
            p.textBlocks["Header"]->setText(ss.str());
            p.textBlocks["Copyright"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutCopyright")));
            p.textBlocks["License"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutLicense")));
            p.textBlocks["ContributorsHeader"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutContributorsHeader")));
            p.textBlocks["Contributors"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutContributors")));
            p.textBlocks["ThirdPartyHeader"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutThirdPartyHeader")));
            p.textBlocks["ThirdParty"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutThirdParty")));
            p.textBlocks["TrademarksHeader"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutTrademarksHeader")));
            p.textBlocks["Trademarks"]->setText(_getText(DJV_TEXT("djv::ViewLib", "AboutTrademarks")));
        }

    } // namespace ViewLib
} // namespace djv

