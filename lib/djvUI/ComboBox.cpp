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

#include <djvUI/ComboBox.h>

#include <djvUI/Action.h>
#include <djvUI/PushButton.h>
#include <djvUI/Menu.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ComboBox::Private
        {
            std::vector<std::string> items;
            int currentItem = -1;
            std::shared_ptr<Button::Push> button;
            std::shared_ptr<Menu> menu;
            std::function<void(int)> callback;
        };

        void ComboBox::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::ComboBox");

            _p->button = Button::Push::create(context);
            _p->button->setButtonType(ButtonType::Toggle);
            _p->button->setParent(shared_from_this());

            _p->menu = Menu::create(context);

            _updateCurrentItem();

            auto weak = std::weak_ptr<ComboBox>(std::dynamic_pointer_cast<ComboBox>(shared_from_this()));
            _p->button->setCheckedCallback(
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto window = widget->getWindow().lock())
                        {
                            widget->_p->menu->show(window, widget->getGeometry(), Orientation::Vertical);
                        }
                    }
                }
            });
        }

        ComboBox::ComboBox() :
            _p(new Private)
        {}

        ComboBox::~ComboBox()
        {}

        std::shared_ptr<ComboBox> ComboBox::create(Context * context)
        {
            auto out = std::shared_ptr<ComboBox>(new ComboBox);
            out->_init(context);
            return out;
        }

        std::shared_ptr<ComboBox> ComboBox::create(const std::vector<std::string> & items, Context * context)
        {
            auto out = ComboBox::create(context);
            out->setItems(items);
            return out;
        }

        const std::vector<std::string> & ComboBox::getItems() const
        {
            return _p->items;
        }

        void ComboBox::setItems(const std::vector<std::string> & value)
        {
            if (value == _p->items)
                return;
            _p->items = value;
            _updateItems();
            setCurrentItem(Math::clamp(_p->currentItem, 0, static_cast<int>(_p->items.size()) - 1));
        }

        int ComboBox::getCurrentItem() const
        {
            return _p->currentItem;
        }

        void ComboBox::setCurrentItem(int value)
        {
            if (value == _p->currentItem)
                return;
            _p->currentItem = value;
            _updateItems();
            _updateCurrentItem();
        }

        void ComboBox::setCallback(const std::function<void(int)> & value)
        {
            _p->callback = value;
        }

        const std::string & ComboBox::getFontFace() const
        {
            return _p->button->getFontFace();
        }

        Style::MetricsRole ComboBox::getFontSizeRole() const
        {
            return _p->button->getFontSizeRole();
        }

        void ComboBox::setFontFace(const std::string & value)
        {
            _p->button->setFontFace(value);
        }

        void ComboBox::setFontSizeRole(Style::MetricsRole value)
        {
            _p->button->setFontSizeRole(value);
        }

        void ComboBox::preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void ComboBox::layoutEvent(Event::Layout& event)
        {
            _p->button->setGeometry(getGeometry());
        }

        void ComboBox::_updateItems()
        {
            _p->menu->clear();
            for (const auto & i : _p->items)
            {
                auto action = Action::create();
                action->setText(i);
                _p->menu->addAction(action);
            }
        }

        void ComboBox::_updateCurrentItem()
        {
            if (_p->currentItem >= 0 && _p->currentItem < _p->items.size())
            {
                _p->button->setText(_p->items[_p->currentItem]);
            }
        }

    } // namespace UI
} // namespace djv
