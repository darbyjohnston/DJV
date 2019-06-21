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
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TextBlock.h>

#include <djvAV/ThumbnailSystem.h>

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
            std::future<std::shared_ptr<AV::Image::Image> > imageFuture;
        };

        void AboutDialog::_init(Context * context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::AboutDialog");

            p.headers["Copyright"] = UI::Label::create(context);
            p.headers["Contributors"] = UI::Label::create(context);
            p.headers["ThirdParty"] = UI::Label::create(context);
            p.headers["Trademarks"] = UI::Label::create(context);
            for (auto& i : p.headers)
            {
                i.second->setTextHAlign(UI::TextHAlign::Left);
                i.second->setFontSizeRole(UI::MetricsRole::FontLarge);
            }

            p.textBlocks["Copyright"] = UI::TextBlock::create(context);
            p.textBlocks["License1"] = UI::TextBlock::create(context);
            p.textBlocks["License2"] = UI::TextBlock::create(context);
            p.textBlocks["License3"] = UI::TextBlock::create(context);
            p.textBlocks["License4"] = UI::TextBlock::create(context);
            p.textBlocks["License5"] = UI::TextBlock::create(context);
            p.textBlocks["ContributorsHeader"] = UI::TextBlock::create(context);
            p.textBlocks["ContributorsHeader"]->setFontSizeRole(UI::MetricsRole::FontLarge);
            p.textBlocks["Contributors1"] = UI::TextBlock::create(context);
            p.textBlocks["Contributors2"] = UI::TextBlock::create(context);
            p.textBlocks["Contributors3"] = UI::TextBlock::create(context);
            p.textBlocks["Contributors4"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdPartyHeader"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdPartyHeader"]->setFontSizeRole(UI::MetricsRole::FontLarge);
            p.textBlocks["ThirdParty1"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty2"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty3"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty4"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty5"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty6"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty7"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty8"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty9"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty9B"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty10"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty11"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty12"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty13"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty14"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty15"] = UI::TextBlock::create(context);
            p.textBlocks["ThirdParty16"] = UI::TextBlock::create(context);
            p.textBlocks["TrademarksHeader"] = UI::TextBlock::create(context);
            p.textBlocks["TrademarksHeader"]->setFontSizeRole(UI::MetricsRole::FontLarge);
            p.textBlocks["Trademarks1"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks2"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks3"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks4"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks5"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks6"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks7"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks8"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks9"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks10"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks11"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks12"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks13"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks14"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks15"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks16"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks17"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks18"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks19"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks20"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks21"] = UI::TextBlock::create(context);
            p.textBlocks["Trademarks22"] = UI::TextBlock::create(context);
            p.textBlocks["MadeIn"] = UI::TextBlock::create(context);

            auto textLayout = UI::VerticalLayout::create(context);
            textLayout->setMargin(UI::MetricsRole::Margin);
            textLayout->setSpacing(UI::MetricsRole::SpacingLarge);
            
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["Copyright"]);
            vLayout->addChild(p.textBlocks["Copyright"]);
            vLayout->addChild(p.textBlocks["License1"]);
            auto vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::MetricsRole::None);
            vLayout2->addChild(p.textBlocks["License2"]);
            vLayout2->addChild(p.textBlocks["License3"]);
            vLayout2->addChild(p.textBlocks["License4"]);
            vLayout->addChild(vLayout2);
            vLayout->addChild(p.textBlocks["License5"]);
            textLayout->addChild(vLayout);
            
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["Contributors"]);
            vLayout->addChild(p.textBlocks["Contributors1"]);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::MetricsRole::None);
            vLayout2->addChild(p.textBlocks["Contributors2"]);
            vLayout2->addChild(p.textBlocks["Contributors3"]);
            vLayout2->addChild(p.textBlocks["Contributors4"]);
            vLayout->addChild(vLayout2);
            textLayout->addChild(vLayout);
            
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["ThirdParty"]);
            vLayout->addChild(p.textBlocks["ThirdParty1"]);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::MetricsRole::None);
            vLayout2->addChild(p.textBlocks["ThirdParty2"]);
            vLayout2->addChild(p.textBlocks["ThirdParty3"]);
            vLayout2->addChild(p.textBlocks["ThirdParty4"]);
            vLayout2->addChild(p.textBlocks["ThirdParty5"]);
            vLayout2->addChild(p.textBlocks["ThirdParty6"]);
            vLayout2->addChild(p.textBlocks["ThirdParty7"]);
            vLayout2->addChild(p.textBlocks["ThirdParty8"]);
            vLayout2->addChild(p.textBlocks["ThirdParty9"]);
            vLayout2->addChild(p.textBlocks["ThirdParty10"]);
            vLayout2->addChild(p.textBlocks["ThirdParty11"]);
            vLayout2->addChild(p.textBlocks["ThirdParty12"]);
            vLayout2->addChild(p.textBlocks["ThirdParty13"]);
            vLayout2->addChild(p.textBlocks["ThirdParty14"]);
            vLayout2->addChild(p.textBlocks["ThirdParty15"]);
            vLayout2->addChild(p.textBlocks["ThirdParty16"]);
            vLayout->addChild(vLayout2);
            textLayout->addChild(vLayout);

            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.headers["Trademarks"]);
            vLayout->addChild(p.textBlocks["Trademarks1"]);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::MetricsRole::None);
            vLayout2->addChild(p.textBlocks["Trademarks2"]);
            vLayout2->addChild(p.textBlocks["Trademarks3"]);
            vLayout2->addChild(p.textBlocks["Trademarks4"]);
            vLayout2->addChild(p.textBlocks["Trademarks5"]);
            vLayout2->addChild(p.textBlocks["Trademarks6"]);
            vLayout2->addChild(p.textBlocks["Trademarks7"]);
            vLayout2->addChild(p.textBlocks["Trademarks8"]);
            vLayout2->addChild(p.textBlocks["Trademarks9"]);
            vLayout2->addChild(p.textBlocks["Trademarks10"]);
            vLayout2->addChild(p.textBlocks["Trademarks11"]);
            vLayout2->addChild(p.textBlocks["Trademarks12"]);
            vLayout2->addChild(p.textBlocks["Trademarks13"]);
            vLayout2->addChild(p.textBlocks["Trademarks14"]);
            vLayout2->addChild(p.textBlocks["Trademarks15"]);
            vLayout2->addChild(p.textBlocks["Trademarks16"]);
            vLayout2->addChild(p.textBlocks["Trademarks17"]);
            vLayout2->addChild(p.textBlocks["Trademarks18"]);
            vLayout2->addChild(p.textBlocks["Trademarks19"]);
            vLayout2->addChild(p.textBlocks["Trademarks20"]);
            vLayout2->addChild(p.textBlocks["Trademarks21"]);
            vLayout->addChild(vLayout2);
            vLayout->addChild(p.textBlocks["Trademarks22"]);
            textLayout->addChild(vLayout);

            vLayout = UI::VerticalLayout::create(context);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->addChild(p.textBlocks["MadeIn"]);
            vLayout->addChild(vLayout2);
            textLayout->addChild(vLayout);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(textLayout);
            addChild(scrollWidget);
            setStretch(scrollWidget, UI::RowStretch::Expand);
        }

        AboutDialog::AboutDialog() :
            _p(new Private)
        {}

        AboutDialog::~AboutDialog()
        {}

        std::shared_ptr<AboutDialog> AboutDialog::create(Context * context)
        {
            auto out = std::shared_ptr<AboutDialog>(new AboutDialog);
            out->_init(context);
            return out;
        }

        void AboutDialog::_localeEvent(Event::Locale & event)
        {
            IDialog::_localeEvent(event);
            DJV_PRIVATE_PTR();

            std::stringstream ss;
            ss << _getText(DJV_TEXT("About Title"));
            ss << " " << DJV_VERSION;
            setTitle(ss.str());

            p.headers["Copyright"]->setText(_getText(DJV_TEXT("Copyright and License")));
            p.headers["Contributors"]->setText(_getText(DJV_TEXT("Contributors")));
            p.headers["ThirdParty"]->setText(_getText(DJV_TEXT("Third Party")));
            p.headers["Trademarks"]->setText(_getText(DJV_TEXT("Trademarks")));

            p.textBlocks["Copyright"]->setText(_getText(DJV_TEXT("Copyright")));
            p.textBlocks["License1"]->setText(_getText(DJV_TEXT("License 1")));
            p.textBlocks["License2"]->setText(_getText(DJV_TEXT("License 2")));
            p.textBlocks["License3"]->setText(_getText(DJV_TEXT("License 3")));
            p.textBlocks["License4"]->setText(_getText(DJV_TEXT("License 4")));
            p.textBlocks["License5"]->setText(_getText(DJV_TEXT("License 5")));
            p.textBlocks["Contributors1"]->setText(_getText(DJV_TEXT("Contributors Text")));
            p.textBlocks["Contributors2"]->setText(_getText(DJV_TEXT("Contributors Darby Johnston")));
            p.textBlocks["Contributors3"]->setText(_getText(DJV_TEXT("Contributors Alan Jones")));
            p.textBlocks["Contributors4"]->setText(_getText(DJV_TEXT("Contributors Mikael Sundell")));
            p.textBlocks["ThirdParty1"]->setText(_getText(DJV_TEXT("Third Party Text")));
            p.textBlocks["ThirdParty2"]->setText(_getText(DJV_TEXT("Third Party CMake")));
            p.textBlocks["ThirdParty3"]->setText(_getText(DJV_TEXT("Third Party dr_libs")));
            p.textBlocks["ThirdParty4"]->setText(_getText(DJV_TEXT("Third Party FFMpeg")));
            p.textBlocks["ThirdParty5"]->setText(_getText(DJV_TEXT("Third Party FreeType")));
            p.textBlocks["ThirdParty6"]->setText(_getText(DJV_TEXT("Third Party GLFW")));
            p.textBlocks["ThirdParty7"]->setText(_getText(DJV_TEXT("Third Party GLM")));
            p.textBlocks["ThirdParty8"]->setText(_getText(DJV_TEXT("Third Party glad")));
            p.textBlocks["ThirdParty9"]->setText(_getText(DJV_TEXT("Third Party libjpeg")));
            p.textBlocks["ThirdParty10"]->setText(_getText(DJV_TEXT("Third Party libjpeg-turbo")));
            p.textBlocks["ThirdParty11"]->setText(_getText(DJV_TEXT("Third Party libpng")));
            p.textBlocks["ThirdParty12"]->setText(_getText(DJV_TEXT("Third Party libtiff")));
            p.textBlocks["ThirdParty13"]->setText(_getText(DJV_TEXT("Third Party OpenAL")));
            p.textBlocks["ThirdParty14"]->setText(_getText(DJV_TEXT("Third Party OpenColorIO")));
            p.textBlocks["ThirdParty15"]->setText(_getText(DJV_TEXT("Third Party OpenEXR")));
            p.textBlocks["ThirdParty16"]->setText(_getText(DJV_TEXT("Third Party zlib")));
            p.textBlocks["Trademarks1"]->setText(_getText(DJV_TEXT("Trademarks Text")));
            p.textBlocks["Trademarks2"]->setText(_getText(DJV_TEXT("Trademarks Apple")));
            p.textBlocks["Trademarks3"]->setText(_getText(DJV_TEXT("Trademarks AMD")));
            p.textBlocks["Trademarks4"]->setText(_getText(DJV_TEXT("Trademarks Autodesk")));
            p.textBlocks["Trademarks5"]->setText(_getText(DJV_TEXT("Trademarks Debian")));
            p.textBlocks["Trademarks6"]->setText(_getText(DJV_TEXT("Trademarks FFmpeg")));
            p.textBlocks["Trademarks7"]->setText(_getText(DJV_TEXT("Trademarks FreeBSD")));
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
            p.textBlocks["Trademarks22"]->setText(_getText(DJV_TEXT("Trademarks End")));
            p.textBlocks["MadeIn"]->setText(_getText(DJV_TEXT("Made In")));
        }

    } // namespace ViewApp
} // namespace djv

