//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUIComponents/ListViewHeader.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/IButton.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Splitter.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class HeaderButton : public Button::IButton
            {
                DJV_NON_COPYABLE(HeaderButton);

            protected:
                void _init(const std::shared_ptr<Context>&);
                HeaderButton();

            public:
                virtual ~HeaderButton();

                static std::shared_ptr<HeaderButton> create(const std::shared_ptr<Context>&);

                void setIcon(const std::string &);

                const std::string & getText() const { return _label->getText(); }
                void setText(const std::string &);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;
                void _paintEvent(Event::Paint &) override;

            private:
                std::shared_ptr<Label> _label;
                std::shared_ptr<Icon> _icon;
                std::shared_ptr<HorizontalLayout> _layout;
            };

            void HeaderButton::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::ListViewHeader::HeaderButton");
                setButtonType(ButtonType::Toggle);
                setBackgroundRole(ColorRole::Button);

                _label = Label::create(context);
                _label->setTextHAlign(TextHAlign::Left);

                _icon = Icon::create(context);
                _icon->setIconSizeRole(MetricsRole::IconSmall);
                _icon->setVAlign(VAlign::Center);

                _layout = HorizontalLayout::create(context);
                _layout->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                _layout->setSpacing(Layout::Spacing(MetricsRole::SpacingSmall));
                _layout->addChild(_label);
                _layout->setStretch(_label, RowStretch::Expand);
                _layout->addChild(_icon);
                addChild(_layout);
            }

            HeaderButton::HeaderButton()
            {}

            HeaderButton::~HeaderButton()
            {}

            std::shared_ptr<HeaderButton> HeaderButton::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<HeaderButton>(new HeaderButton);
                out->_init(context);
                return out;
            }

            void HeaderButton::setIcon(const std::string & value)
            {
                _icon->setIcon(value);
                _icon->setVisible(!value.empty());
            }

            void HeaderButton::setText(const std::string & value)
            {
                _label->setText(value);
            }

            float HeaderButton::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void HeaderButton::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void HeaderButton::_layoutEvent(Event::Layout &)
            {
                _layout->setGeometry(getGeometry());
            }

            void HeaderButton::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                const BBox2f & g = getGeometry();
                auto render = _getRender();
                const auto& style = _getStyle();
                if (_isPressed())
                {
                    render->setFillColor(style->getColor(ColorRole::Pressed));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g);
                }
            }

        } // namespace

        struct ListViewHeader::Private
        {
            size_t sort = 0;
            bool reverseSort = false;
            std::function<void(size_t, bool)> sortCallback;
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<Layout::Splitter> splitter;
        };

        void ListViewHeader::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            
            setClassName("djv::UI::ListViewHeader");
            setBackgroundRole(ColorRole::Button);
            
            p.buttonGroup = ButtonGroup::create(ButtonType::Push);

            p.splitter = Layout::Splitter::create(Orientation::Horizontal, context);
            p.splitter->setHandleColorRole(ColorRole::Button);
            addChild(p.splitter);

            _sortUpdate();

            auto weak = std::weak_ptr<ListViewHeader>(std::dynamic_pointer_cast<ListViewHeader>(shared_from_this()));
            p.buttonGroup->setPushCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (value == widget->_p->sort)
                    {
                        widget->setReverseSort(!widget->_p->reverseSort);
                        if (widget->_p->sortCallback)
                        {
                            widget->_p->sortCallback(widget->_p->sort, widget->_p->reverseSort);
                        }
                    }
                    else
                    {
                        widget->setSort(value);
                        if (widget->_p->sortCallback)
                        {
                            widget->_p->sortCallback(widget->_p->sort, widget->_p->reverseSort);
                        }
                    }
                }
            });
        }

        ListViewHeader::ListViewHeader() :
            _p(new Private)
        {}

        ListViewHeader::~ListViewHeader()
        {}

        std::shared_ptr<ListViewHeader> ListViewHeader::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ListViewHeader>(new ListViewHeader);
            out->_init(context);
            return out;
        }

        void ListViewHeader::setText(const std::vector<std::string> & value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttonGroup->clearButtons();
                auto split = p.splitter->getSplit();
                p.splitter->clearChildren();
                for (const auto& i : value)
                {
                    auto button = HeaderButton::create(context);
                    button->setText(i);
                    p.buttonGroup->addButton(button);
                    p.splitter->addChild(button);
                }
                p.splitter->setSplit(split);
                _sortUpdate();
            }
        }

        size_t ListViewHeader::getSort() const
        {
            return _p->sort;
        }

        bool ListViewHeader::getReverseSort() const
        {
            return _p->reverseSort;
        }

        void ListViewHeader::setSort(size_t value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sort)
                return;
            p.sort = value;
            _sortUpdate();
        }

        void ListViewHeader::setReverseSort(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.reverseSort)
                return;
            p.reverseSort = value;
            _sortUpdate();
        }

        void ListViewHeader::setSortCallback(const std::function<void(size_t, bool)> & callback)
        {
            _p->sortCallback = callback;
        }

        const std::vector<float> & ListViewHeader::getSplit() const
        {
            return _p->splitter->getSplit();
        }

        void ListViewHeader::setSplit(const std::vector<float> & value)
        {
            _p->splitter->setSplit(value);
        }

        void ListViewHeader::setSplitCallback(const std::function<void(const std::vector<float> &)> & callback)
        {
            _p->splitter->setSplitCallback(callback);
        }

        void ListViewHeader::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->splitter->getMinimumSize());
        }

        void ListViewHeader::_layoutEvent(Event::Layout & event)
        {
            _p->splitter->setGeometry(getGeometry());
        }

        void ListViewHeader::_sortUpdate()
        {
            DJV_PRIVATE_PTR();
            auto buttons = p.splitter->getChildrenT<HeaderButton>();
            for (size_t i = 0; i < buttons.size(); ++i)
            {
                if (i == p.sort)
                {
                    buttons[i]->setIcon(p.reverseSort ? "djvIconArrowSmallUp" : "djvIconArrowSmallDown");
                }
                else
                {
                    buttons[i]->setIcon(std::string());
                }
            }
        }

    } // namespace UI
} // namespace djv
