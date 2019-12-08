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

#pragma once

#include <djvUI/Label.h>
#include <djvUI/MDIWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

class ITool : public djv::UI::MDI::IWidget
{
    DJV_NON_COPYABLE(ITool);

protected:
    void _init(const std::shared_ptr<djv::Core::Context>&);
    ITool();

public:
    virtual ~ITool();

    const std::string& getTitle() const;
    void setTitle(const std::string&);

    void close();
    void setCloseCallback(const std::function<void(void)>&);

    float getHeightForWidth(float) const override;

    void addChild(const std::shared_ptr<IObject>&) override;
    void removeChild(const std::shared_ptr<IObject>&) override;
    void clearChildren() override;

protected:
    std::map<djv::UI::MDI::Handle, std::vector<djv::Core::BBox2f> > _getHandles() const override;
    void _setActiveWidget(bool) override;

    void _preLayoutEvent(djv::Core::Event::PreLayout&) override;
    void _layoutEvent(djv::Core::Event::Layout&) override;

    void _initEvent(djv::Core::Event::Init&) override;

private:
    std::shared_ptr<djv::UI::Label> _titleLabel;
    std::shared_ptr<djv::UI::ToolButton> _closeButton;
    std::shared_ptr<djv::UI::HorizontalLayout> _titleBar;
    std::shared_ptr<djv::UI::VerticalLayout> _childLayout;
    std::shared_ptr<djv::UI::VerticalLayout> _layout;
    std::function<void(void)> _closeCallback;
};
