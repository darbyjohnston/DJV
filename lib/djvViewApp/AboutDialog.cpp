// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AboutDialog.h>

#include <djvUI/Label.h>
#include <djvUI/GridLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TextBlock.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AboutDialog::Private
        {
            std::map<std::string, std::shared_ptr<UI::Label> > headers;
            std::map<std::string, std::shared_ptr<UI::TextBlock> > textBlocks;
        };

        void AboutDialog::_init(const std::shared_ptr<Core::Context>& context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::AboutDialog");

            const std::vector<std::string> headers = 
            {
                "Credits", "Sponsors", "License", "Copyright", "ThirdParty", "Trademarks"
            };
            for (const auto& i : headers)
            {
                p.headers[i] = UI::Label::create(context);
                p.headers[i]->setTextHAlign(UI::TextHAlign::Left);
                p.headers[i]->setFontSizeRole(UI::MetricsRole::FontHeader);
            }
            
            const std::vector<std::string> credits = 
            {
                "Credits1", "Credits2", "Credits3", "Credits4", "Credits5",
                "Credits6", "Credits7", "Credits8", "Credits9"
            };
            for (const auto& i : credits)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
                p.textBlocks[i]->setTextSizeRole(UI::MetricsRole::TextColumnLarge);
            }
            
            const std::vector<std::string> creditsText = 
            {
                "Credits1Text", "Credits2Text", "Credits3Text", "Credits4Text",
                "Credits5Text", "Credits6Text", "Credits7Text", "Credits8Text",
                "Credits9Text"
            };
            for (const auto& i : creditsText)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
            }

            const std::vector<std::string> sponsorsText = 
            {
                "Sponsors1"
            };
            for (const auto& i : sponsorsText)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
            }

            const std::vector<std::string> licenseText = 
            {
                "License1", "License2", "License3", "License4", "License5", "License6"
            };
            for (const auto& i : licenseText)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
            }

            p.textBlocks["CopyrightText"] = UI::TextBlock::create(context);
            const std::vector<std::string> copyrightText =
            {
                "Copyright1", "Copyright2", "Copyright3"
            };
            for (const auto& i : copyrightText)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
            }

            p.textBlocks["ThirdPartyText"] = UI::TextBlock::create(context);
            const std::vector<std::string> thirdPartyText =
            {
                "ThirdParty1", "ThirdParty2", "ThirdParty3", "ThirdParty4",
                "ThirdParty5", "ThirdParty6", "ThirdParty7", "ThirdParty8",
                "ThirdParty9", "ThirdParty10", "ThirdParty11", "ThirdParty12",
                "ThirdParty13", "ThirdParty14", "ThirdParty15", "ThirdParty16",
                "ThirdParty17", "ThirdParty18", "ThirdParty19"
            };
            for (const auto& i : thirdPartyText)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
            }

            p.textBlocks["TrademarksText"] = UI::TextBlock::create(context);
            const std::vector<std::string> trademarksText =
            {
                "Trademarks1", "Trademarks2", "Trademarks3", "Trademarks4",
                "Trademarks5", "Trademarks6", "Trademarks7", "Trademarks8",
                "Trademarks9", "Trademarks10", "Trademarks11", "Trademarks12",
                "Trademarks13", "Trademarks14", "Trademarks15", "Trademarks16",
                "Trademarks17", "Trademarks18", "Trademarks19", "Trademarks20",
                "Trademarks21", "Trademarks22"
            };
            for (const auto& i : trademarksText)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
            }
            p.textBlocks["TrademarksEnd"] = UI::TextBlock::create(context);

            const std::vector<std::string> madeInText = 
            {
                "MadeIn"
            };
            for (const auto& i : madeInText)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
            }
                    
            auto textLayout = UI::VerticalLayout::create(context);
            textLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
            textLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingLarge));
            
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["Credits"]);
            vLayout->addSeparator();
            auto gridLayout = UI::GridLayout::create(context);
            gridLayout->addChild(p.textBlocks["Credits1"]);
            gridLayout->setGridPos(p.textBlocks["Credits1"], glm::ivec2(0, 0));
            gridLayout->addChild(p.textBlocks["Credits1Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits1Text"], glm::ivec2(1, 0));
            gridLayout->setStretch(p.textBlocks["Credits1Text"], UI::GridStretch::Horizontal);
            gridLayout->addChild(p.textBlocks["Credits2"]);
            gridLayout->setGridPos(p.textBlocks["Credits2"], glm::ivec2(0, 1));
            gridLayout->addChild(p.textBlocks["Credits2Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits2Text"], glm::ivec2(1, 1));
            gridLayout->addChild(p.textBlocks["Credits3"]);
            gridLayout->setGridPos(p.textBlocks["Credits3"], glm::ivec2(0, 2));
            gridLayout->addChild(p.textBlocks["Credits3Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits3Text"], glm::ivec2(1, 2));
            gridLayout->addChild(p.textBlocks["Credits4"]);
            gridLayout->setGridPos(p.textBlocks["Credits4"], glm::ivec2(0, 3));
            gridLayout->addChild(p.textBlocks["Credits4Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits4Text"], glm::ivec2(1, 3));
            gridLayout->addChild(p.textBlocks["Credits5"]);
            gridLayout->setGridPos(p.textBlocks["Credits5"], glm::ivec2(0, 4));
            gridLayout->addChild(p.textBlocks["Credits5Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits5Text"], glm::ivec2(1, 4));
            gridLayout->addChild(p.textBlocks["Credits6"]);
            gridLayout->setGridPos(p.textBlocks["Credits6"], glm::ivec2(0, 5));
            gridLayout->addChild(p.textBlocks["Credits6Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits6Text"], glm::ivec2(1, 5));
            gridLayout->addChild(p.textBlocks["Credits7"]);
            gridLayout->setGridPos(p.textBlocks["Credits7"], glm::ivec2(0, 6));
            gridLayout->addChild(p.textBlocks["Credits7Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits7Text"], glm::ivec2(1, 6));
            gridLayout->addChild(p.textBlocks["Credits8"]);
            gridLayout->setGridPos(p.textBlocks["Credits8"], glm::ivec2(0, 7));
            gridLayout->addChild(p.textBlocks["Credits8Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits8Text"], glm::ivec2(1, 7));
            gridLayout->addChild(p.textBlocks["Credits9"]);
            gridLayout->setGridPos(p.textBlocks["Credits9"], glm::ivec2(0, 8));
            gridLayout->addChild(p.textBlocks["Credits9Text"]);
            gridLayout->setGridPos(p.textBlocks["Credits9Text"], glm::ivec2(1, 8));
            vLayout->addChild(gridLayout);
            textLayout->addChild(vLayout);

            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["Sponsors"]);
            vLayout->addSeparator();
            for (const auto& i : sponsorsText)
            {
                vLayout->addChild(p.textBlocks[i]);
            }
            textLayout->addChild(vLayout);

            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["License"]);
            vLayout->addSeparator();
            for (size_t i = 1; i <= 2; ++i)
            {
                std::stringstream ss;
                ss << "License" << i;
                vLayout->addChild(p.textBlocks[ss.str()]);
            }
            auto vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            for (size_t i = 3; i <= 5; ++i)
            {
                std::stringstream ss;
                ss << "License" << i;
                vLayout2->addChild(p.textBlocks[ss.str()]);
            }
            vLayout->addChild(vLayout2);
            vLayout->addChild(p.textBlocks["License6"]);
            textLayout->addChild(vLayout);
            
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["Copyright"]);
            vLayout->addSeparator();
            vLayout->addChild(p.textBlocks["CopyrightText"]);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            for (size_t i = 1; i <= copyrightText.size(); ++i)
            {
                std::stringstream ss;
                ss << "Copyright" << i;
                vLayout2->addChild(p.textBlocks[ss.str()]);
            }
            vLayout->addChild(vLayout2);
            textLayout->addChild(vLayout);
            
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["ThirdParty"]);
            vLayout->addSeparator();
            vLayout->addChild(p.textBlocks["ThirdPartyText"]);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            for (size_t i = 1; i <= thirdPartyText.size(); ++i)
            {
                std::stringstream ss;
                ss << "ThirdParty" << i;
                vLayout2->addChild(p.textBlocks[ss.str()]);
            }
            vLayout->addChild(vLayout2);
            textLayout->addChild(vLayout);

            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["Trademarks"]);
            vLayout->addSeparator();
            vLayout->addChild(p.textBlocks["TrademarksText"]);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            for (size_t i = 1; i <= trademarksText.size(); ++i)
            {
                std::stringstream ss;
                ss << "Trademarks" << i;
                vLayout2->addChild(p.textBlocks[ss.str()]);
            }
            vLayout->addChild(vLayout2);
            vLayout->addChild(p.textBlocks["TrademarksEnd"]);
            textLayout->addChild(vLayout);

            vLayout = UI::VerticalLayout::create(context);
            vLayout->addSeparator();
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->addChild(p.textBlocks["MadeIn"]);
            vLayout->addChild(vLayout2);
            textLayout->addChild(vLayout);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(textLayout);
            addChild(scrollWidget);
            setStretch(scrollWidget, UI::RowStretch::Expand);
        }

        AboutDialog::AboutDialog() :
            _p(new Private)
        {}

        AboutDialog::~AboutDialog()
        {}

        std::shared_ptr<AboutDialog> AboutDialog::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<AboutDialog>(new AboutDialog);
            out->_init(context);
            return out;
        }

        void AboutDialog::_initEvent(Event::Init & event)
        {
            IDialog::_initEvent(event);
            DJV_PRIVATE_PTR();

            std::stringstream ss;
            ss << _getText(DJV_TEXT("about_title"));
            ss << " " << DJV_VERSION;
            setTitle(ss.str());

            p.headers["Credits"]->setText(_getText(DJV_TEXT("about_section_credits")));
            p.headers["Sponsors"]->setText(_getText(DJV_TEXT("about_section_sponsors")));
            p.headers["License"]->setText(_getText(DJV_TEXT("about_section_license")));
            p.headers["Copyright"]->setText(_getText(DJV_TEXT("about_section_copyright")));
            p.headers["ThirdParty"]->setText(_getText(DJV_TEXT("about_section_third_party")));
            p.headers["Trademarks"]->setText(_getText(DJV_TEXT("about_section_trademarks")));

            p.textBlocks["Credits1"]->setText(_getText(DJV_TEXT("about_credits_darby_johnston")));
            p.textBlocks["Credits1Text"]->setText(_getText(DJV_TEXT("about_credits_darby_johnston_text")));
            p.textBlocks["Credits2"]->setText(_getText(DJV_TEXT("about_credits_kent_oberheu")));
            p.textBlocks["Credits2Text"]->setText(_getText(DJV_TEXT("about_credits_kent_oberheu_text")));
            p.textBlocks["Credits3"]->setText(_getText(DJV_TEXT("about_credits_siciliana_johnston")));
            p.textBlocks["Credits3Text"]->setText(_getText(DJV_TEXT("about_credits_siciliana_johnston_text")));
            p.textBlocks["Credits4"]->setText(_getText(DJV_TEXT("about_credits_mikael_sundell")));
            p.textBlocks["Credits4Text"]->setText(_getText(DJV_TEXT("about_credits_mikael_sundell_text")));
            p.textBlocks["Credits5"]->setText(_getText(DJV_TEXT("about_credits_cansecogpc")));
            p.textBlocks["Credits5Text"]->setText(_getText(DJV_TEXT("about_credits_cansecogpc_text")));
            p.textBlocks["Credits6"]->setText(_getText(DJV_TEXT("about_credits_jean-francois_panisset")));
            p.textBlocks["Credits6Text"]->setText(_getText(DJV_TEXT("about_credits_jean-francois_panisset_text")));
            p.textBlocks["Credits7"]->setText(_getText(DJV_TEXT("about_credits_haryo_sukmawanto")));
            p.textBlocks["Credits7Text"]->setText(_getText(DJV_TEXT("about_credits_haryo_sukmawanto_text")));
            p.textBlocks["Credits8"]->setText(_getText(DJV_TEXT("about_credits_damien_picard")));
            p.textBlocks["Credits8Text"]->setText(_getText(DJV_TEXT("about_credits_damien_picard_text")));
            p.textBlocks["Credits9"]->setText(_getText(DJV_TEXT("about_credits_stefan_ihringer")));
            p.textBlocks["Credits9Text"]->setText(_getText(DJV_TEXT("about_credits_stefan_ihringer_text")));
            p.textBlocks["Sponsors1"]->setText(_getText(DJV_TEXT("about_sponsors_unexpected")));
            p.textBlocks["License1"]->setText(_getText(DJV_TEXT("about_license_1")));
            p.textBlocks["License2"]->setText(_getText(DJV_TEXT("about_license_2")));
            p.textBlocks["License3"]->setText(_getText(DJV_TEXT("about_license_3")));
            p.textBlocks["License4"]->setText(_getText(DJV_TEXT("about_license_4")));
            p.textBlocks["License5"]->setText(_getText(DJV_TEXT("about_license_5")));
            p.textBlocks["License6"]->setText(_getText(DJV_TEXT("about_license_6")));
            p.textBlocks["CopyrightText"]->setText(_getText(DJV_TEXT("about_copyright_text")));
            p.textBlocks["Copyright1"]->setText(_getText(DJV_TEXT("about_copyright_darby_johnston")));
            p.textBlocks["Copyright2"]->setText(_getText(DJV_TEXT("about_copyright_kent_oberheu")));
            p.textBlocks["Copyright3"]->setText(_getText(DJV_TEXT("about_copyright_mikael_sundell")));
            p.textBlocks["ThirdPartyText"]->setText(_getText(DJV_TEXT("about_third_party_text")));
            p.textBlocks["ThirdParty1"]->setText(_getText(DJV_TEXT("about_third_party_cmake")));
            p.textBlocks["ThirdParty2"]->setText(_getText(DJV_TEXT("about_third_party_ffmpeg")));
            p.textBlocks["ThirdParty3"]->setText(_getText(DJV_TEXT("about_third_party_freetype")));
            p.textBlocks["ThirdParty4"]->setText(_getText(DJV_TEXT("about_third_party_glfw")));
            p.textBlocks["ThirdParty5"]->setText(_getText(DJV_TEXT("about_third_party_glm")));
            p.textBlocks["ThirdParty6"]->setText(_getText(DJV_TEXT("about_third_party_mbedtls")));
            p.textBlocks["ThirdParty7"]->setText(_getText(DJV_TEXT("about_third_party_openal")));
            p.textBlocks["ThirdParty8"]->setText(_getText(DJV_TEXT("about_third_party_opencolorio")));
            p.textBlocks["ThirdParty9"]->setText(_getText(DJV_TEXT("about_third_party_openexr")));
            p.textBlocks["ThirdParty10"]->setText(_getText(DJV_TEXT("about_third_party_rtaudio")));
            p.textBlocks["ThirdParty11"]->setText(_getText(DJV_TEXT("about_third_party_curl")));
            p.textBlocks["ThirdParty12"]->setText(_getText(DJV_TEXT("about_third_party_dr_libs")));
            p.textBlocks["ThirdParty13"]->setText(_getText(DJV_TEXT("about_third_party_glad")));
            p.textBlocks["ThirdParty14"]->setText(_getText(DJV_TEXT("about_third_party_libjpeg")));
            p.textBlocks["ThirdParty15"]->setText(_getText(DJV_TEXT("about_third_party_libjpeg-turbo")));
            p.textBlocks["ThirdParty16"]->setText(_getText(DJV_TEXT("about_third_party_libpng")));
            p.textBlocks["ThirdParty17"]->setText(_getText(DJV_TEXT("about_third_party_libtiff")));
            p.textBlocks["ThirdParty18"]->setText(_getText(DJV_TEXT("about_third_party_opennurbs")));
            p.textBlocks["ThirdParty19"]->setText(_getText(DJV_TEXT("about_third_party_zlib")));
            p.textBlocks["TrademarksText"]->setText(_getText(DJV_TEXT("about_trademarks_text")));
            p.textBlocks["Trademarks1"]->setText(_getText(DJV_TEXT("about_trademarks_apple")));
            p.textBlocks["Trademarks2"]->setText(_getText(DJV_TEXT("about_trademarks_amd")));
            p.textBlocks["Trademarks3"]->setText(_getText(DJV_TEXT("about_trademarks_autodesk")));
            p.textBlocks["Trademarks4"]->setText(_getText(DJV_TEXT("about_trademarks_debian")));
            p.textBlocks["Trademarks5"]->setText(_getText(DJV_TEXT("about_trademarks_ffmpeg")));
            p.textBlocks["Trademarks6"]->setText(_getText(DJV_TEXT("about_trademarks_freebsd")));
            p.textBlocks["Trademarks7"]->setText(_getText(DJV_TEXT("about_trademarks_github")));
            p.textBlocks["Trademarks8"]->setText(_getText(DJV_TEXT("about_trademarks_intel")));
            p.textBlocks["Trademarks9"]->setText(_getText(DJV_TEXT("about_trademarks_lucasfilm")));
            p.textBlocks["Trademarks10"]->setText(_getText(DJV_TEXT("about_trademarks_kodak")));
            p.textBlocks["Trademarks11"]->setText(_getText(DJV_TEXT("about_trademarks_linux")));
            p.textBlocks["Trademarks12"]->setText(_getText(DJV_TEXT("about_trademarks_microsoft")));
            p.textBlocks["Trademarks13"]->setText(_getText(DJV_TEXT("about_trademarks_mips")));
            p.textBlocks["Trademarks14"]->setText(_getText(DJV_TEXT("about_trademarks_nvidia")));
            p.textBlocks["Trademarks15"]->setText(_getText(DJV_TEXT("about_trademarks_red_hat")));
            p.textBlocks["Trademarks16"]->setText(_getText(DJV_TEXT("about_trademarks_rhino")));
            p.textBlocks["Trademarks17"]->setText(_getText(DJV_TEXT("about_trademarks_sgi")));
            p.textBlocks["Trademarks18"]->setText(_getText(DJV_TEXT("about_trademarks_smpte")));
            p.textBlocks["Trademarks19"]->setText(_getText(DJV_TEXT("about_trademarks_sourceforge")));
            p.textBlocks["Trademarks20"]->setText(_getText(DJV_TEXT("about_trademarks_suse")));
            p.textBlocks["Trademarks21"]->setText(_getText(DJV_TEXT("about_trademarks_ubuntu")));
            p.textBlocks["Trademarks22"]->setText(_getText(DJV_TEXT("about_trademarks_unix")));
            p.textBlocks["TrademarksEnd"]->setText(_getText(DJV_TEXT("about_trademarks_end")));
            p.textBlocks["MadeIn"]->setText(_getText(DJV_TEXT("about_made_in")));
        }

    } // namespace ViewApp
} // namespace djv

