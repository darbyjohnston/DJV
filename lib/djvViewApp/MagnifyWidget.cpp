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

#include <djvViewApp/MagnifyWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MagnifyWidget::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ImageWidget> imageWidget;
            std::shared_ptr<UI::IntSlider> magnificationSlider;
            std::shared_ptr<UI::ToolButton> zoomInButton;
            std::shared_ptr<UI::ToolButton> zoomOutButton;
        };

        void MagnifyWidget::_init(Context * context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MagnifyWidget");

            p.imageWidget = UI::ImageWidget::create(context);
            p.imageWidget->setSizeRole(UI::MetricsRole::Swatch);
            p.imageWidget->setShadowOverlay({ UI::Side::Top });
            
            p.magnificationSlider = UI::IntSlider::create(context);
            p.magnificationSlider->setRange(IntRange(1, 10));
            p.magnificationSlider->setMargin(UI::MetricsRole::MarginSmall);

            p.zoomInButton = UI::ToolButton::create(context);
            p.zoomInButton->setIcon("djvIconAdd");

            p.zoomOutButton = UI::ToolButton::create(context);
            p.zoomOutButton->setIcon("djvIconSubtract");

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
            layout->addChild(hLayout);
            addChild(layout);
        }

        MagnifyWidget::MagnifyWidget() :
            _p(new Private)
        {}

        MagnifyWidget::~MagnifyWidget()
        {}

        std::shared_ptr<MagnifyWidget> MagnifyWidget::create(Context * context)
        {
            auto out = std::shared_ptr<MagnifyWidget>(new MagnifyWidget);
            out->_init(context);
            return out;
        }

        void MagnifyWidget::_localeEvent(Event::Locale & event)
        {
            MDIWidget::_localeEvent(event);

            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Magnify")));

            p.zoomInButton->setTooltip(_getText(DJV_TEXT("Magnify zoom in tooltip")));
            p.zoomOutButton->setTooltip(_getText(DJV_TEXT("Magnify zoom out tooltip")));
        }

    } // namespace ViewApp
} // namespace djv

