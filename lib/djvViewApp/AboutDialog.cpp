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
                "Credits6"
            };
            for (const auto& i : credits)
            {
                p.textBlocks[i] = UI::TextBlock::create(context);
                p.textBlocks[i]->setTextSizeRole(UI::MetricsRole::TextColumnLarge);
            }
            
            const std::vector<std::string> creditsText = 
            {
                "Credits1Text", "Credits2Text", "Credits3Text", "Credits4Text",
                "Credits5Text", "Credits6Text"
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
                "ThirdParty17", "ThirdParty18"
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
                "Trademarks21"
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
            ss << _getText(DJV_TEXT("About Title"));
            ss << " " << DJV_VERSION;
            setTitle(ss.str());

            p.headers["Credits"]->setText(_getText(DJV_TEXT("Credits")));
            p.headers["Sponsors"]->setText(_getText(DJV_TEXT("Sponsors")));
            p.headers["License"]->setText(_getText(DJV_TEXT("License")));
            p.headers["Copyright"]->setText(_getText(DJV_TEXT("Copyright")));
            p.headers["ThirdParty"]->setText(_getText(DJV_TEXT("Third Party")));
            p.headers["Trademarks"]->setText(_getText(DJV_TEXT("Trademarks")));

            p.textBlocks["Credits1"]->setText(_getText(DJV_TEXT("Credits Darby Johnston")));
            p.textBlocks["Credits1Text"]->setText(_getText(DJV_TEXT("Credits Darby Johnston Text")));
            p.textBlocks["Credits2"]->setText(_getText(DJV_TEXT("Credits Kent Oberheu")));
            p.textBlocks["Credits2Text"]->setText(_getText(DJV_TEXT("Credits Kent Oberheu Text")));
            p.textBlocks["Credits3"]->setText(_getText(DJV_TEXT("Credits Siciliana Johnston")));
            p.textBlocks["Credits3Text"]->setText(_getText(DJV_TEXT("Credits Siciliana Johnston Text")));
            p.textBlocks["Credits4"]->setText(_getText(DJV_TEXT("Credits Mikael Sundell")));
            p.textBlocks["Credits4Text"]->setText(_getText(DJV_TEXT("Credits Mikael Sundell Text")));
            p.textBlocks["Credits5"]->setText(_getText(DJV_TEXT("Credits CansecoGPC")));
            p.textBlocks["Credits5Text"]->setText(_getText(DJV_TEXT("Credits CansecoGPC Text")));
            p.textBlocks["Credits6"]->setText(_getText(DJV_TEXT("Credits Jean-Francois Panisset")));
            p.textBlocks["Credits6Text"]->setText(_getText(DJV_TEXT("Credits Jean-Francois Panisset Text")));
            p.textBlocks["Sponsors1"]->setText(_getText(DJV_TEXT("Sponsors unexpected")));
            p.textBlocks["License1"]->setText(_getText(DJV_TEXT("License 1")));
            p.textBlocks["License2"]->setText(_getText(DJV_TEXT("License 2")));
            p.textBlocks["License3"]->setText(_getText(DJV_TEXT("License 3")));
            p.textBlocks["License4"]->setText(_getText(DJV_TEXT("License 4")));
            p.textBlocks["License5"]->setText(_getText(DJV_TEXT("License 5")));
            p.textBlocks["License6"]->setText(_getText(DJV_TEXT("License 6")));
            p.textBlocks["CopyrightText"]->setText(_getText(DJV_TEXT("Copyright Text")));
            p.textBlocks["Copyright1"]->setText(_getText(DJV_TEXT("Copyright Darby Johnston")));
            p.textBlocks["Copyright2"]->setText(_getText(DJV_TEXT("Copyright Kent Oberheu")));
            p.textBlocks["Copyright3"]->setText(_getText(DJV_TEXT("Copyright Mikael Sundell")));
            p.textBlocks["ThirdPartyText"]->setText(_getText(DJV_TEXT("Third Party Text")));
            p.textBlocks["ThirdParty1"]->setText(_getText(DJV_TEXT("Third Party CMake")));
            p.textBlocks["ThirdParty2"]->setText(_getText(DJV_TEXT("Third Party FFMpeg")));
            p.textBlocks["ThirdParty3"]->setText(_getText(DJV_TEXT("Third Party FreeType")));
            p.textBlocks["ThirdParty4"]->setText(_getText(DJV_TEXT("Third Party GLFW")));
            p.textBlocks["ThirdParty5"]->setText(_getText(DJV_TEXT("Third Party GLM")));
            p.textBlocks["ThirdParty6"]->setText(_getText(DJV_TEXT("Third Party MbedTLS")));
            p.textBlocks["ThirdParty7"]->setText(_getText(DJV_TEXT("Third Party OpenAL")));
            p.textBlocks["ThirdParty8"]->setText(_getText(DJV_TEXT("Third Party OpenColorIO")));
            p.textBlocks["ThirdParty9"]->setText(_getText(DJV_TEXT("Third Party OpenEXR")));
            p.textBlocks["ThirdParty10"]->setText(_getText(DJV_TEXT("Third Party RtAudio")));
            p.textBlocks["ThirdParty11"]->setText(_getText(DJV_TEXT("Third Party curl")));
            p.textBlocks["ThirdParty12"]->setText(_getText(DJV_TEXT("Third Party dr_libs")));
            p.textBlocks["ThirdParty13"]->setText(_getText(DJV_TEXT("Third Party glad")));
            p.textBlocks["ThirdParty14"]->setText(_getText(DJV_TEXT("Third Party libjpeg")));
            p.textBlocks["ThirdParty15"]->setText(_getText(DJV_TEXT("Third Party libjpeg-turbo")));
            p.textBlocks["ThirdParty16"]->setText(_getText(DJV_TEXT("Third Party libpng")));
            p.textBlocks["ThirdParty17"]->setText(_getText(DJV_TEXT("Third Party libtiff")));
            p.textBlocks["ThirdParty18"]->setText(_getText(DJV_TEXT("Third Party zlib")));
            p.textBlocks["TrademarksText"]->setText(_getText(DJV_TEXT("Trademarks Text")));
            p.textBlocks["Trademarks1"]->setText(_getText(DJV_TEXT("Trademarks Apple")));
            p.textBlocks["Trademarks2"]->setText(_getText(DJV_TEXT("Trademarks AMD")));
            p.textBlocks["Trademarks3"]->setText(_getText(DJV_TEXT("Trademarks Autodesk")));
            p.textBlocks["Trademarks4"]->setText(_getText(DJV_TEXT("Trademarks Debian")));
            p.textBlocks["Trademarks5"]->setText(_getText(DJV_TEXT("Trademarks FFmpeg")));
            p.textBlocks["Trademarks6"]->setText(_getText(DJV_TEXT("Trademarks FreeBSD")));
            p.textBlocks["Trademarks7"]->setText(_getText(DJV_TEXT("Trademarks GitHub")));
            p.textBlocks["Trademarks8"]->setText(_getText(DJV_TEXT("Trademarks Intel")));
            p.textBlocks["Trademarks9"]->setText(_getText(DJV_TEXT("Trademarks Lucasfilm")));
            p.textBlocks["Trademarks10"]->setText(_getText(DJV_TEXT("Trademarks Kodak")));
            p.textBlocks["Trademarks11"]->setText(_getText(DJV_TEXT("Trademarks Linux")));
            p.textBlocks["Trademarks12"]->setText(_getText(DJV_TEXT("Trademarks Microsoft")));
            p.textBlocks["Trademarks13"]->setText(_getText(DJV_TEXT("Trademarks MIPS")));
            p.textBlocks["Trademarks14"]->setText(_getText(DJV_TEXT("Trademarks NVIDIA")));
            p.textBlocks["Trademarks15"]->setText(_getText(DJV_TEXT("Trademarks Red Hat")));
            p.textBlocks["Trademarks16"]->setText(_getText(DJV_TEXT("Trademarks SGI")));
            p.textBlocks["Trademarks17"]->setText(_getText(DJV_TEXT("Trademarks SMPTE")));
            p.textBlocks["Trademarks18"]->setText(_getText(DJV_TEXT("Trademarks SourceForge")));
            p.textBlocks["Trademarks19"]->setText(_getText(DJV_TEXT("Trademarks SuSE")));
            p.textBlocks["Trademarks20"]->setText(_getText(DJV_TEXT("Trademarks Ubuntu")));
            p.textBlocks["Trademarks21"]->setText(_getText(DJV_TEXT("Trademarks UNIX")));
            p.textBlocks["TrademarksEnd"]->setText(_getText(DJV_TEXT("Trademarks End")));
            p.textBlocks["MadeIn"]->setText(_getText(DJV_TEXT("Made In")));
        }

    } // namespace ViewApp
} // namespace djv

