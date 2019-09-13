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

#include <djvUI/Bellows.h>

#include <djvUI/IButton.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Spacer.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            namespace
            {
                class Button : public UI::Button::IButton
                {
                    DJV_NON_COPYABLE(Button);

                protected:
                    void _init(const std::shared_ptr<Context>&);
                    Button();

                public:
                    virtual ~Button();

                    static std::shared_ptr<Button> create(const std::shared_ptr<Context>&);

                    void setIcon(const std::string &);

                    const std::string & getText() const { return _label->getText(); }
                    void setText(const std::string &);

                    float getHeightForWidth(float) const override;

                protected:
                    void _preLayoutEvent(Event::PreLayout &) override;
                    void _layoutEvent(Event::Layout &) override;
                    void _paintEvent(Event::Paint &) override;

                private:
                    std::shared_ptr<Icon> _icon;
                    std::shared_ptr<Label> _label;
                    std::shared_ptr<HorizontalLayout> _layout;
                };

                void Button::_init(const std::shared_ptr<Context>& context)
                {
                    Widget::_init(context);

                    setClassName("djv::UI::Layout::Bellows::Button");
                    setButtonType(ButtonType::Toggle);
                    setBackgroundRole(ColorRole::BackgroundBellows);

                    _icon = Icon::create(context);
                    _icon->setIcon("djvIconArrowSmallRight");
                    _icon->setIconSizeRole(MetricsRole::IconSmall);
                    _icon->setVAlign(VAlign::Center);

                    _label = Label::create(context);
                    _label->setTextHAlign(TextHAlign::Left);

                    _layout = HorizontalLayout::create(context);
                    _layout->setMargin(MetricsRole::MarginSmall);
                    _layout->setSpacing(MetricsRole::SpacingSmall);
                    _layout->addChild(_icon);
                    _layout->addChild(_label);
                    _layout->setStretch(_label, RowStretch::Expand);
                    addChild(_layout);
                }

                Button::Button()
                {}

                Button::~Button()
                {}

                std::shared_ptr<Button> Button::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<Button>(new Button);
                    out->_init(context);
                    return out;
                }

                void Button::setIcon(const std::string & value)
                {
                    _icon->setIcon(value);
                }

                void Button::setText(const std::string & value)
                {
                    _label->setText(value);
                }

                float Button::getHeightForWidth(float value) const
                {
                    return _layout->getHeightForWidth(value);
                }

                void Button::_preLayoutEvent(Event::PreLayout &)
                {
                    _setMinimumSize(_layout->getMinimumSize());
                }

                void Button::_layoutEvent(Event::Layout &)
                {
                    _layout->setGeometry(getGeometry());
                }

                void Button::_paintEvent(Event::Paint & event)
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

            struct Bellows::Private
            {
                std::shared_ptr<Button> button;
                std::shared_ptr<Layout::Spacer> spacer;
                std::shared_ptr<StackLayout> childLayout;
                std::shared_ptr<VerticalLayout> layout;
                std::function<void(bool)> openCallback;
            };

            void Bellows::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Layout::Bellows");
                setVAlign(VAlign::Top);

                p.button = Button::create(context);

                p.spacer = Layout::Spacer::create(Orientation::Vertical, context);
                
                p.childLayout = StackLayout::create(context);
                p.childLayout->setShadowOverlay({ Side::Top });
                p.childLayout->addChild(p.spacer);

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addChild(p.button);
                p.layout->addSeparator();
                p.layout->addChild(p.childLayout);
                p.layout->setStretch(p.childLayout, RowStretch::Expand);
                Widget::addChild(p.layout);

                setOpen(true);

                auto weak = std::weak_ptr<Bellows>(std::dynamic_pointer_cast<Bellows>(shared_from_this()));
                p.button->setCheckedCallback(
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->setOpen(value);
                        if (widget->_p->openCallback)
                        {
                            widget->_p->openCallback(value);
                        }
                    }
                });
            }

            Bellows::Bellows() :
                _p(new Private)
            {}

            Bellows::~Bellows()
            {}

            std::shared_ptr<Bellows> Bellows::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Bellows>(new Bellows);
                out->_init(context);
                return out;
            }

            const std::string & Bellows::getText() const
            {
                return _p->button->getText();
            }

            void Bellows::setText(const std::string & text)
            {
                _p->button->setText(text);
            }

            bool Bellows::isOpen() const
            {
                return _p->button->isChecked();
            }

            void Bellows::setOpen(bool value)
            {
                DJV_PRIVATE_PTR();
                p.button->setChecked(value);
                p.button->setIcon(value ? "djvIconArrowSmallDown" : "djvIconArrowSmallRight");
                _childrenUpdate();
            }

            void Bellows::open()
            {
                setOpen(true);
            }

            void Bellows::close()
            {
                setOpen(false);
            }

            void Bellows::setOpenCallback(const std::function<void(bool)> & callback)
            {
                _p->openCallback = callback;
            }

            void Bellows::addChild(const std::shared_ptr<IObject> & value)
            {
                _p->childLayout->addChild(value);
                _childrenUpdate();
            }

            void Bellows::removeChild(const std::shared_ptr<IObject> & value)
            {
                _p->childLayout->removeChild(value);
                _childrenUpdate();
            }

            void Bellows::clearChildren()
            {
                _p->childLayout->clearChildren();
            }

            float Bellows::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void Bellows::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void Bellows::_layoutEvent(Event::Layout & event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void Bellows::_childrenUpdate()
            {
                DJV_PRIVATE_PTR();
                p.spacer->setVisible(p.childLayout->getChildWidgets().size() == 1);
                p.childLayout->setVisible(p.button->isChecked());
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
