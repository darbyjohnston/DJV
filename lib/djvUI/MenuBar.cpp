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

#include <djvUI/MenuBar.h>

#include <djvUI/ListButton.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct MenuBar::Private
        {
            std::vector<std::shared_ptr<Menu> > menus;
            std::shared_ptr<HorizontalLayout> layout;
        };

        void MenuBar::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::MenuBar");

            _p->layout = HorizontalLayout::create(context);
            _p->layout->setSpacing(MetricsRole::None);
            _p->layout->setParent(shared_from_this());
        }

        MenuBar::MenuBar() :
            _p(new Private)
        {}

        MenuBar::~MenuBar()
        {}

        std::shared_ptr<MenuBar> MenuBar::create(Context * context)
        {
            auto out = std::shared_ptr<MenuBar>(new MenuBar);
            out->_init(context);
            return out;
        }

        void MenuBar::addMenu(const std::shared_ptr<Menu> & menu)
        {
            _p->menus.push_back(menu);
            auto button = ListButton::create(menu->getText(), getContext());
            _p->layout->addWidget(button);
        }

        float MenuBar::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void MenuBar::preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MenuBar::layoutEvent(LayoutEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f & g = getGeometry();
                _p->layout->setGeometry(getMargin().bbox(g, style));
            }
        }

    } // namespace UI
} // namespace djv
