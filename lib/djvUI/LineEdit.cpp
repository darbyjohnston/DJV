// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/LineEdit.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Text
        {
            struct LineEdit::Private
            {
                std::shared_ptr<LineEditBase> lineEditBase;
            };

            void LineEdit::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::LineEdit");
                setVAlign(VAlign::Center);

                p.lineEditBase = LineEditBase::create(context);
                p.lineEditBase->setBackgroundColorRole(ColorRole::Trough);
                addChild(p.lineEditBase);

                auto weak = std::weak_ptr<LineEdit>(std::dynamic_pointer_cast<LineEdit>(shared_from_this()));
                p.lineEditBase->setFocusCallback(
                    [weak](bool)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_redraw();
                        }
                    });
            }

            LineEdit::LineEdit() :
                _p(new Private)
            {}

            LineEdit::~LineEdit()
            {}

            std::shared_ptr<LineEdit> LineEdit::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<LineEdit>(new LineEdit);
                out->_init(context);
                return out;
            }

            const std::string& LineEdit::getText() const
            {
                return _p->lineEditBase->getText();
            }

            void LineEdit::setText(const std::string& value)
            {
                _p->lineEditBase->setText(value);
            }

            const std::string& LineEdit::getFont() const
            {
                return _p->lineEditBase->getFont();
            }

            const std::string& LineEdit::getFontFace() const
            {
                return _p->lineEditBase->getFontFace();
            }

            MetricsRole LineEdit::getFontSizeRole() const
            {
                return _p->lineEditBase->getFontSizeRole();
            }

            void LineEdit::setFont(const std::string& value)
            {
                _p->lineEditBase->setFont(value);
            }

            void LineEdit::setFontFace(const std::string& value)
            {
                _p->lineEditBase->setFontFace(value);
            }

            void LineEdit::setFontSizeRole(MetricsRole value)
            {
                _p->lineEditBase->setFontSizeRole(value);
            }

            ColorRole LineEdit::getTextColorRole() const
            {
                return _p->lineEditBase->getTextColorRole();
            }

            void LineEdit::setTextColorRole(ColorRole value)
            {
                _p->lineEditBase->setTextColorRole(value);
            }

            MetricsRole LineEdit::getTextSizeRole() const
            {
                return _p->lineEditBase->getTextSizeRole();
            }

            const std::string& LineEdit::getSizeString() const
            {
                return _p->lineEditBase->getSizeString();
            }

            void LineEdit::setTextSizeRole(MetricsRole value)
            {
                _p->lineEditBase->setTextSizeRole(value);
            }

            void LineEdit::setSizeString(const std::string& value)
            {
                _p->lineEditBase->setSizeString(value);
            }

            void LineEdit::setTextChangedCallback(const std::function<void(const std::string&)>& callback)
            {
                _p->lineEditBase->setTextChangedCallback(callback);
            }

            void LineEdit::setTextEditCallback(const std::function<void(const std::string&, TextEditReason)>& callback)
            {
                _p->lineEditBase->setTextEditCallback(callback);
            }

            void LineEdit::setFocusCallback(const std::function<void(bool)>& callback)
            {
                _p->lineEditBase->setFocusCallback(callback);
            }

            void LineEdit::setBackgroundColorRole(ColorRole value)
            {
                DJV_PRIVATE_PTR();
                p.lineEditBase->setBackgroundColorRole(value);
            }

            std::shared_ptr<Widget> LineEdit::getFocusWidget()
            {
                return _p->lineEditBase->getFocusWidget();
            }

            void LineEdit::_preLayoutEvent(System::Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float b = style->getMetric(UI::MetricsRole::Border);
                const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
                glm::vec2 size = p.lineEditBase->getMinimumSize();
                _setMinimumSize(size + b * 2.F + btf * 2.F + getMargin().getSize(style));
            }

            void LineEdit::_layoutEvent(System::Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float b = style->getMetric(UI::MetricsRole::Border);
                const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
                const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
                p.lineEditBase->setGeometry(g.margin(-b - btf));
            }

            void LineEdit::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float b = style->getMetric(UI::MetricsRole::Border);
                const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const auto& render = _getRender();
                if (p.lineEditBase->hasTextFocus())
                {
                    render->setFillColor(style->getColor(UI::ColorRole::TextFocus));
                    drawBorder(render, g, btf);
                }
                Math::BBox2f g2 = g.margin(-btf);
                render->setFillColor(style->getColor(UI::ColorRole::Border));
                drawBorder(render, g2, b);
            }

        } // namespace Text
    } // namespace UI
} // namespace djv
