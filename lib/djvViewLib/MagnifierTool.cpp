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

#include <djvViewLib/MagnifierTool.h>

#include <djvUI/Action.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/IntSlider.h>
#include <djvUI/FlatButton.h>
#include <djvUI/Menu.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MagnifierTool::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ImageWidget> imageWidget;
            std::shared_ptr<UI::IntSlider> magnificationSlider;
            std::shared_ptr<UI::FlatButton> zoomInButton;
            std::shared_ptr<UI::FlatButton> zoomOutButton;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<UI::PopupMenu> popupMenu;
        };

        void MagnifierTool::_init(Context * context)
        {
            ITool::_init(context);

            DJV_PRIVATE_PTR();
            p.actions["ColorProfile"] = UI::Action::create();
            p.actions["ColorProfile"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["DisplayProfile"] = UI::Action::create();
            p.actions["DisplayProfile"]->setButtonType(UI::ButtonType::Toggle);

            p.imageWidget = UI::ImageWidget::create(context);
            p.imageWidget->setSizeRole(UI::MetricsRole::TextColumn);
            
            p.magnificationSlider = UI::IntSlider::create(context);
            p.magnificationSlider->setRange(IntRange(1, 10));
            p.magnificationSlider->setMargin(UI::MetricsRole::MarginSmall);

            p.zoomInButton = UI::FlatButton::create(context);
            p.zoomInButton->setIcon("djvIconAdd");

            p.zoomOutButton = UI::FlatButton::create(context);
            p.zoomOutButton->setIcon("djvIconSubtract");

            p.menu = UI::Menu::create(context);
            p.menu->setIcon("djvIconMenuPopup");
            p.menu->addAction(p.actions["ColorProfile"]);
            p.menu->addAction(p.actions["DisplayProfile"]);
            p.popupMenu = UI::PopupMenu::create(context);
            p.popupMenu->setMenu(p.menu);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addChild(p.imageWidget);
            layout->setStretch(p.imageWidget, UI::RowStretch::Expand);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.magnificationSlider);
            hLayout->setStretch(p.magnificationSlider, UI::RowStretch::Expand);
            hLayout->addChild(p.zoomInButton);
            hLayout->addChild(p.zoomOutButton);
            hLayout->addChild(p.popupMenu);
            layout->addChild(hLayout);
            addChild(layout);
        }

        MagnifierTool::MagnifierTool() :
            _p(new Private)
        {}

        MagnifierTool::~MagnifierTool()
        {}

        std::shared_ptr<MagnifierTool> MagnifierTool::create(Context * context)
        {
            auto out = std::shared_ptr<MagnifierTool>(new MagnifierTool);
            out->_init(context);
            return out;
        }

        void MagnifierTool::_localeEvent(Event::Locale & event)
        {
            ITool::_localeEvent(event);

            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Magnifier")));

            p.actions["ColorProfile"]->setTitle("Enable Color Profile");
            p.actions["ColorProfile"]->setTooltip("Magnifier enable color profile tooltip");
            p.actions["DisplayProfile"]->setTitle("Enable Display Profile");
            p.actions["DisplayProfile"]->setTooltip("Magnifier enable display profile tooltip");

            p.zoomInButton->setTooltip("Magnifier zoom in tooltip");
            p.zoomOutButton->setTooltip("Magnifier zoom out tooltip");
        }

    } // namespace ViewLib
} // namespace djv

