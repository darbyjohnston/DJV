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

#include <djvUI/ListButton.h>

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            namespace
            {
                class CheckWidget : public Widget
                {
                    DJV_NON_COPYABLE(CheckWidget);

                protected:
                    CheckWidget();

                public:
                    static std::shared_ptr<CheckWidget> create(Context *);

                    void setButtonType(ButtonType);
                    void setChecked(bool);

                protected:
                    void _preLayoutEvent(Event::PreLayout &) override;
                    void _paintEvent(Event::Paint &) override;

                private:
                    ButtonType _buttonType = ButtonType::First;
                    bool _checked = false;
                };

                CheckWidget::CheckWidget()
                {}

                std::shared_ptr<CheckWidget> CheckWidget::create(Context * context)
                {
                    auto out = std::shared_ptr<CheckWidget>(new CheckWidget);
                    out->_init(context);
                    return out;
                }

                void CheckWidget::setButtonType(ButtonType value)
                {
                    if (value == _buttonType)
                        return;
                    _buttonType = value;
                    _redraw();
                }

                void CheckWidget::setChecked(bool value)
                {
                    if (value == _checked)
                        return;
                    _checked = value;
                    _redraw();
                }

                void CheckWidget::_preLayoutEvent(Event::PreLayout &)
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float iconSize = style->getMetric(Style::MetricsRole::Icon);
                        _setMinimumSize(glm::vec2(iconSize, iconSize));
                    }
                }

                void CheckWidget::_paintEvent(Event::Paint & event)
                {
                    Widget::_paintEvent(event);
                    if (auto render = _getRender().lock())
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const BBox2f & g = getGeometry();
                            const float m = style->getMetric(Style::MetricsRole::MarginSmall);

                            if (_checked)
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Foreground)));
                                render->drawCircle(g.getCenter(), m, 16);
                            }
                        }
                    }
                }

            } // namespace

            struct List::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<CheckWidget> checkWidget;
                std::shared_ptr<Label> label;
                std::shared_ptr<Layout::Horizontal> layout;
            };

            void List::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::Button::List");

                DJV_PRIVATE_PTR();
                p.icon = Icon::create(context);
                p.icon->setVAlign(VAlign::Center);

                p.checkWidget = CheckWidget::create(context);

                p.label = Label::create(context);
                p.label->setTextHAlign(TextHAlign::Left);

                p.layout = Layout::Horizontal::create(context);
                p.layout->setMargin(Layout::Margin(Style::MetricsRole::Margin, Style::MetricsRole::Margin, Style::MetricsRole::MarginSmall, Style::MetricsRole::MarginSmall));
                p.layout->addWidget(p.icon);
                p.layout->addWidget(p.checkWidget);
                p.layout->addWidget(p.label, Layout::RowStretch::Expand);
                p.layout->setParent(shared_from_this());

                _widgetUpdate();
            }

            List::List() :
                _p(new Private)
            {}

            List::~List()
            {}

            std::shared_ptr<List> List::create(Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                return out;
            }

            std::shared_ptr<List> List::create(const std::string& text, Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<List> List::create(const std::string& text, const std::string& icon, Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
                return out;
            }

            const std::string& List::getIcon() const
            {
                return _p->icon->getIcon();
            }

            void List::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.icon->setIcon(value);
                _widgetUpdate();
            }

            const std::string& List::getText() const
            {
                return _p->label->getText();
            }

            void List::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.label->setText(value);
                _widgetUpdate();
            }

            TextHAlign List::getTextHAlign() const
            {
                return _p->label->getTextHAlign();
            }

            TextVAlign List::getTextVAlign() const
            {
                return _p->label->getTextVAlign();
            }

            void List::setTextHAlign(TextHAlign value)
            {
                _p->label->setTextHAlign(value);
            }

            void List::setTextVAlign(TextVAlign value)
            {
                _p->label->setTextVAlign(value);
            }

            Style::ColorRole List::getTextColorRole() const
            {
                return _p->label->getTextColorRole();
            }

            void List::setTextColorRole(Style::ColorRole value)
            {
                _p->label->setTextColorRole(value);
            }

            const std::string & List::getFont() const
            {
                return _p->label->getFont();
            }

            const std::string & List::getFontFace() const
            {
                return _p->label->getFontFace();
            }

            Style::MetricsRole List::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void List::setFont(const std::string & value)
            {
                _p->label->setFont(value);
            }

            void List::setFontFace(const std::string & value)
            {
                _p->label->setFontFace(value);
            }

            void List::setFontSizeRole(Style::MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            const Layout::Margin& List::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void List::setInsideMargin(const Layout::Margin& value)
            {
                _p->layout->setMargin(value);
            }

            void List::setButtonType(ButtonType value)
            {
                IButton::setButtonType(value);
                _widgetUpdate();
            }

            void List::setChecked(bool value)
            {
                IButton::setChecked(value);
                _p->checkWidget->setChecked(value);
            }

            void List::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void List::_layoutEvent(Event::Layout&)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void List::_widgetUpdate()
            {
                bool iconVisible = !_p->icon->getIcon().empty();
                _p->icon->setVisible(iconVisible);
                switch (getButtonType())
                {
                case ButtonType::Toggle:
                case ButtonType::Radio:
                    if (!iconVisible)
                    {
                        _p->checkWidget->show();
                    }
                    break;
                default:
                    _p->checkWidget->hide();
                    break;
                }
                _p->label->setVisible(!_p->label->getText().empty());
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
