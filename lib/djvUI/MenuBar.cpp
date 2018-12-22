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

#include <djvUI/IButton.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class MenuBarButton : public Button::IButton
            {
                DJV_NON_COPYABLE(MenuBarButton);

            protected:
                void _init(const std::string& text, Core::Context *);
                MenuBarButton();

            public:
                virtual ~MenuBarButton();

                static std::shared_ptr<MenuBarButton> create(const std::string&, Core::Context *);

                const std::string& getText() const;
                void setText(const std::string&);

                float getHeightForWidth(float) const override;
                void preLayoutEvent(Event::PreLayout&) override;
                void layoutEvent(Core::Event::Layout&) override;

            private:
                std::shared_ptr<Label> _label;
                std::shared_ptr<Layout::Stack> _layout;
            };

            void MenuBarButton::_init(const std::string& text, Context * context)
            {
                IButton::_init(context);

                setClassName("Gp::UI::MenuBarButton");

                _label = Label::create(text, context);
                _label->setMargin(Layout::Margin(Style::MetricsRole::Margin, Style::MetricsRole::Margin, Style::MetricsRole::MarginSmall, Style::MetricsRole::MarginSmall));
                _label->setVisible(!text.empty());

                _layout = Layout::Stack::create(context);
                _layout->addWidget(_label);
                _layout->setParent(shared_from_this());
            }

            MenuBarButton::MenuBarButton()
            {}

            MenuBarButton::~MenuBarButton()
            {}

            std::shared_ptr<MenuBarButton> MenuBarButton::create(const std::string& text, Context * context)
            {
                auto out = std::shared_ptr<MenuBarButton>(new MenuBarButton);
                out->_init(text, context);
                return out;
            }

            const std::string& MenuBarButton::getText() const
            {
                return _label->getText();
            }

            void MenuBarButton::setText(const std::string& value)
            {
                _label->setText(value);
                _label->setVisible(!value.empty());
            }

            float MenuBarButton::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void MenuBarButton::preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void MenuBarButton::layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

        } // namespace

        struct MenuBar::Private
        {
            std::vector<std::shared_ptr<Menu> > menus;
            std::shared_ptr<Layout::HorizontalLayout> layout;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<MenuBarButton> > menusToButtons;
        };

        void MenuBar::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::MenuBar");

            _p->layout = Layout::HorizontalLayout::create(context);
            _p->layout->setSpacing(Style::MetricsRole::None);
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
            auto button = MenuBarButton::create(menu->getText(), getContext());
            button->setButtonType(ButtonType::Toggle);
            _p->layout->addWidget(button);
            _p->menusToButtons[menu] = button;
            auto weak = std::weak_ptr<MenuBar>(std::dynamic_pointer_cast<MenuBar>(shared_from_this()));
            button->setCheckedCallback(
                [menu, button](bool value)
            {
                if (auto window = button->getWindow().lock())
                {
                    if (value)
                    {
                        menu->show(window, button->getGeometry(), Orientation::Vertical);
                    }
                    else
                    {
                        menu->hide();
                    }
                }
            });
        }

        float MenuBar::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void MenuBar::preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MenuBar::layoutEvent(Event::Layout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f & g = getGeometry();
                _p->layout->setGeometry(getMargin().bbox(g, style));
            }
        }

    } // namespace UI
} // namespace djv
