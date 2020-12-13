// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/ListViewHeader.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/IButton.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Splitter.h>

#include <djvRender2D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace
        {
            class HeaderButton : public UI::Button::IButton
            {
                DJV_NON_COPYABLE(HeaderButton);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                HeaderButton();

            public:
                virtual ~HeaderButton();

                static std::shared_ptr<HeaderButton> create(const std::shared_ptr<System::Context>&);

                void setIcon(const std::string &);

                const std::string & getText() const { return _label->getText(); }
                void setText(const std::string &);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint &) override;

            private:
                std::shared_ptr<UI::Text::Label> _label;
                std::shared_ptr<UI::Icon> _icon;
                std::shared_ptr<UI::HorizontalLayout> _layout;
            };

            void HeaderButton::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UIComponents::ListViewHeader::HeaderButton");
                setButtonType(UI::ButtonType::Toggle);

                _label = UI::Text::Label::create(context);
                _label->setTextHAlign(UI::TextHAlign::Left);

                _icon = UI::Icon::create(context);
                _icon->setIconSizeRole(UI::MetricsRole::IconSmall);
                _icon->setVAlign(UI::VAlign::Center);

                _layout = UI::HorizontalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::MarginSmall);
                _layout->setSpacing(UI::MetricsRole::SpacingSmall);
                _layout->addChild(_label);
                _layout->setStretch(_label, UI::RowStretch::Expand);
                _layout->addChild(_icon);
                addChild(_layout);
            }

            HeaderButton::HeaderButton()
            {}

            HeaderButton::~HeaderButton()
            {}

            std::shared_ptr<HeaderButton> HeaderButton::create(const std::shared_ptr<System::Context>& context)
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

            void HeaderButton::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void HeaderButton::_layoutEvent(System::Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void HeaderButton::_paintEvent(System::Event::Paint & event)
            {
                Widget::_paintEvent(event);
                const Math::BBox2f& g = getGeometry();
                const auto& render = _getRender();
                const auto& style = _getStyle();
                if (_isPressed())
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Pressed));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Hovered));
                    render->drawRect(g);
                }
            }

        } // namespace

        struct ListViewHeader::Private
        {
            size_t sort = 0;
            bool reverseSort = false;
            std::function<void(size_t, bool)> sortCallback;
            std::shared_ptr<UI::ButtonGroup> buttonGroup;
            std::shared_ptr<UI::Layout::Splitter> splitter;
        };

        void ListViewHeader::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            
            setClassName("djv::UIComponents::ListViewHeader");
            setBackgroundRole(UI::ColorRole::BackgroundBellows);
            
            p.buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);

            p.splitter = UI::Layout::Splitter::create(UI::Orientation::Horizontal, context);
            p.splitter->setHandleColorRole(UI::ColorRole::BackgroundBellows);
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

        std::shared_ptr<ListViewHeader> ListViewHeader::create(const std::shared_ptr<System::Context>& context)
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
                std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
                auto split = p.splitter->getSplit();
                p.splitter->clearChildren();
                for (const auto& i : value)
                {
                    auto button = HeaderButton::create(context);
                    button->setText(i);
                    buttons.push_back(button);
                    p.splitter->addChild(button);
                }
                p.buttonGroup->setButtons(buttons);
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

        void ListViewHeader::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->splitter->getMinimumSize());
        }

        void ListViewHeader::_layoutEvent(System::Event::Layout& event)
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

    } // namespace UIComponents
} // namespace djv
