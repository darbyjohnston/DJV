//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include "MainWindow.h"

#include <djvUI/FormLayout.h>
#include <djvUI/IntEdit.h>
#include <djvUI/Label.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>

namespace djv
{
    struct MainWindow::Private
    {
        std::shared_ptr<UI::Label> titleLabel;
        std::shared_ptr<UI::IntEdit> frameCountIntEdit;
        std::shared_ptr<UI::IntEdit> widthIntEdit;
        std::shared_ptr<UI::IntEdit> heightIntEdit;
        std::shared_ptr<UI::PushButton> renderButton;
        std::shared_ptr<UI::FormLayout> formLayout;
        std::shared_ptr<UI::VerticalLayout> layout;
    };

    void MainWindow::_init(const std::shared_ptr<Core::Context>& context)
    {
        Window::_init(context);
        DJV_PRIVATE_PTR();
        
        p.titleLabel = UI::Label::create(context);
        p.titleLabel->setFontSizeRole(UI::MetricsRole::FontHeader);

        p.frameCountIntEdit = UI::IntEdit::create(context);
        p.frameCountIntEdit->setRange(Core::IntRange(1, 10000));

        p.widthIntEdit = UI::IntEdit::create(context);
        p.widthIntEdit->setRange(Core::IntRange(1, 16384));
        p.heightIntEdit = UI::IntEdit::create(context);
        p.heightIntEdit->setRange(Core::IntRange(1, 16384));

        p.renderButton = UI::PushButton::create(context);
        p.renderButton->setHAlign(UI::HAlign::Center);

        p.formLayout = UI::FormLayout::create(context);
        p.formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
        p.formLayout->addChild(p.frameCountIntEdit);
        p.formLayout->addChild(p.widthIntEdit);
        p.formLayout->addChild(p.heightIntEdit);

        p.layout = UI::VerticalLayout::create(context);
        p.layout->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
        p.layout->addChild(p.titleLabel);
        p.layout->addChild(p.formLayout);
        p.layout->addChild(p.renderButton);
        addChild(p.layout);
    }

    MainWindow::MainWindow() :
        _p(new Private)
    {}

    MainWindow::~MainWindow()
    {}

    std::shared_ptr<MainWindow> MainWindow::create(const std::shared_ptr<Core::Context>& context)
    {
        auto out = std::shared_ptr<MainWindow>(new MainWindow);
        out->_init(context);
        return out;
    }

    void MainWindow::_initEvent(Core::Event::Init&)
    {
        DJV_PRIVATE_PTR();
        p.titleLabel->setText(_getText(DJV_TEXT("Test Pattern Generator")));
        p.formLayout->setText(p.frameCountIntEdit, _getText(DJV_TEXT("Frame count")) + ":");
        p.formLayout->setText(p.widthIntEdit, _getText(DJV_TEXT("Width")) + ":");
        p.formLayout->setText(p.heightIntEdit, _getText(DJV_TEXT("Height")) + ":");
        p.renderButton->setText(_getText(DJV_TEXT("Start Render")));
    }

} // namespace djv
