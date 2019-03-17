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

#include <djvUI/LineEditBase.h>

#include <djvAV/Render2D.h>

#include <djvCore/Timer.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            const size_t cursorTimeout = 500;

        } // namespace

        struct LineEditBase::Private
        {
            std::string text;
            ColorRole textColorRole = ColorRole::Foreground;
            std::string font;
            std::string fontFace = AV::Font::Info::faceDefault;
            MetricsRole fontSizeRole = MetricsRole::FontMedium;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            glm::vec2 textSize = glm::vec2(0.f, 0.f);
            std::future<glm::vec2> textSizeFuture;
            size_t cursorPos = 0;
            std::future<glm::vec2> cursorTextSizeFuture;
            glm::vec2 cursorTextSize = glm::vec2(0.f, 0.f);
            bool cursorBlink = false;
            std::shared_ptr<Time::Timer> cursorBlinkTimer;
            std::function<void(std::string)> textChangedCallback;
            std::function<void(std::string)> textFinishedCallback;
            std::function<void(bool)> focusCallback;
        };

        void LineEditBase::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::LineEditBase");
            setBackgroundRole(ColorRole::Trough);
            setVAlign(VAlign::Center);
            setPointerEnabled(true);

            p.cursorBlinkTimer = Time::Timer::create(context);
            p.cursorBlinkTimer->setRepeating(true);

            _textUpdate();
        }

        LineEditBase::LineEditBase() :
            _p(new Private)
        {}

        LineEditBase::~LineEditBase()
        {}

        std::shared_ptr<LineEditBase> LineEditBase::create(Context * context)
        {
            auto out = std::shared_ptr<LineEditBase>(new LineEditBase);
            out->_init(context);
            return out;
        }

        const std::string& LineEditBase::getText() const
        {
            return _p->text;
        }

        void LineEditBase::setText(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.text)
                return;
            p.text = value;
            p.cursorPos = value.size();
            _textUpdate();
            _cursorUpdate();
            if (p.textChangedCallback)
            {
                p.textChangedCallback(p.text);
            }
        }

        ColorRole LineEditBase::getTextColorRole() const
        {
            return _p->textColorRole;
        }

        void LineEditBase::setTextColorRole(ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textColorRole)
                return;
            p.textColorRole = value;
            _redraw();
        }

        const std::string & LineEditBase::getFont() const
        {
            return _p->font;
        }

        const std::string & LineEditBase::getFontFace() const
        {
            return _p->fontFace;
        }

        MetricsRole LineEditBase::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void LineEditBase::setFont(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.font)
                return;
            p.font = value;
            _textUpdate();
            _cursorUpdate();
        }

        void LineEditBase::setFontFace(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFace)
                return;
            p.fontFace = value;
            _textUpdate();
            _cursorUpdate();
        }

        void LineEditBase::setFontSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontSizeRole)
                return;
            p.fontSizeRole = value;
            _textUpdate();
            _cursorUpdate();
        }

        void LineEditBase::setTextChangedCallback(const std::function<void(const std::string &)> & value)
        {
            _p->textChangedCallback = value;
        }

        void LineEditBase::setTextFinishedCallback(const std::function<void(const std::string &)> & value)
        {
            _p->textFinishedCallback = value;
        }

        void LineEditBase::setFocusCallback(const std::function<void(bool)> & value)
        {
            _p->focusCallback = value;
        }

        void LineEditBase::_styleEvent(Event::Style& event)
        {
            _textUpdate();
            _cursorUpdate();
        }
        
        void LineEditBase::_preLayoutEvent(Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            if (auto style = _getStyle().lock())
            {
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float tc = style->getMetric(MetricsRole::TextColumn);

                if (p.fontMetricsFuture.valid())
                {
                    try
                    {
                        p.fontMetrics = p.fontMetricsFuture.get();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
                if (p.textSizeFuture.valid())
                {
                    try
                    {
                        p.textSize = p.textSizeFuture.get();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
                if (p.cursorTextSizeFuture.valid())
                {
                    try
                    {
                        p.cursorTextSize = p.cursorTextSizeFuture.get();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }

                const glm::vec2 size(tc, p.fontMetrics.lineHeight);
                _setMinimumSize(size + m * 2.f + getMargin().getSize(style));
            }
        }

        void LineEditBase::_clipEvent(Event::Clip& event)
        {
            if (isClipped())
            {
                releaseTextFocus();
            }
        }

        void LineEditBase::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (auto render = _getRender().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const glm::vec2 c = g.getCenter();
                    const float m = style->getMetric(MetricsRole::MarginSmall);
                    const float b = style->getMetric(MetricsRole::Border);

                    auto fontInfo = p.font.empty() ?
                        style->getFontInfo(p.fontFace, p.fontSizeRole) :
                        style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
                    render->setCurrentFont(fontInfo);

                    glm::vec2 pos = g.min;
                    pos += m;
                    pos.y = c.y - p.textSize.y / 2.f;

                    render->setFillColor(_getColorWithOpacity(style->getColor(p.textColorRole)));
                    //! \bug Why the extra subtract by one here?
                    render->drawText(p.text, glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.f)));

                    if (p.cursorBlink)
                    {
                        render->drawRect(BBox2f(
                            g.min.x + m + p.cursorTextSize.x,
                            g.min.y + m,
                            b,
                            g.h() - m * 2.f));
                    }
                }
            }
        }

        void LineEditBase::_pointerEnterEvent(Event::PointerEnter& event)
        {
            event.accept();
        }

        void LineEditBase::_pointerLeaveEvent(Event::PointerLeave& event)
        {
            event.accept();
        }

        void LineEditBase::_pointerMoveEvent(Event::PointerMove& event)
        {
            event.accept();
        }

        void LineEditBase::_buttonPressEvent(Event::ButtonPress& event)
        {
            if (!isEnabled(true))
                return;
            event.accept();
            takeTextFocus();
        }

        void LineEditBase::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            event.accept();
        }

        void LineEditBase::_keyPressEvent(Event::KeyPress& event)
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.text.size();
            switch (event.getKey())
            {
            case GLFW_KEY_BACKSPACE:
                event.accept();
                if (size > 0 && p.cursorPos > 0)
                {
                    p.text.erase(p.cursorPos - 1, 1);
                    _textUpdate();
                    _cursorUpdate();
                    if (p.textChangedCallback)
                    {
                        p.textChangedCallback(p.text);
                    }
                }
                break;
            case GLFW_KEY_DELETE:
                event.accept();
                if (size > 0 && p.cursorPos < size)
                {
                    p.text.erase(p.cursorPos, 1);
                    _textUpdate();
                    _cursorUpdate();
                    if (p.textChangedCallback)
                    {
                        p.textChangedCallback(p.text);
                    }
                }
                break;
            case GLFW_KEY_ENTER:
                event.accept();
                if (p.textFinishedCallback)
                {
                    p.textFinishedCallback(p.text);
                }
                break;
            case GLFW_KEY_LEFT:
                event.accept();
                if (p.cursorPos > 0)
                {
                    --p.cursorPos;
                    _cursorUpdate();
                }
                break;
            case GLFW_KEY_RIGHT:
                event.accept();
                if (p.cursorPos < size)
                {
                    ++p.cursorPos;
                    _cursorUpdate();
                }
                break;
            case GLFW_KEY_HOME:
                event.accept();
                if (p.cursorPos > 0)
                {
                    p.cursorPos = 0;
                    _cursorUpdate();
                }
                break;
            case GLFW_KEY_END:
                event.accept();
                if (size > 0 && p.cursorPos != size)
                {
                    p.cursorPos = size;
                    _cursorUpdate();
                }
                break;
            case GLFW_KEY_ESCAPE:
                event.accept();
                releaseTextFocus();
                break;
            default:
                if (!event.getKeyModifiers())
                {
                    event.accept();
                }
                break;
            }
        }

        void LineEditBase::_textFocusEvent(Event::TextFocus& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            _cursorUpdate();
            if (p.focusCallback)
            {
                p.focusCallback(true);
            }
        }

        void LineEditBase::_textFocusLostEvent(Event::TextFocusLost& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            p.cursorBlinkTimer->stop();
            p.cursorBlink = false;
            _redraw();
            if (p.textFinishedCallback)
            {
                p.textFinishedCallback(p.text);
            }
            if (p.focusCallback)
            {
                p.focusCallback(false);
            }
        }

        void LineEditBase::_textEvent(Event::Text& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto & text = event.getText();
            p.text.insert(p.cursorPos, text);
            p.cursorPos += text.size();
            _textUpdate();
            _cursorUpdate();
            if (p.textChangedCallback)
            {
                p.textChangedCallback(p.text);
            }
        }

        void LineEditBase::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const auto fontInfo = p.font.empty() ?
                        style->getFontInfo(p.fontFace, p.fontSizeRole) :
                        style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
                    p.fontMetricsFuture = fontSystem->getMetrics(fontInfo);
                    p.textSizeFuture = fontSystem->measure(p.text, fontInfo);
                    _resize();
                }
            }
        }

        void LineEditBase::_cursorUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const size_t size = p.text.size();
                    if (size)
                    {
                        p.cursorPos = Math::clamp(p.cursorPos, size_t(0), size);
                    }
                    else
                    {
                        p.cursorPos = 0;
                    }

                    const auto fontInfo = p.font.empty() ?
                        style->getFontInfo(p.fontFace, p.fontSizeRole) :
                        style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
                    const std::string cursorText = p.cursorPos < size ? p.text.substr(0, p.cursorPos) : p.text;
                    p.cursorTextSizeFuture = fontSystem->measure(cursorText, fontInfo);

                    if (hasTextFocus())
                    {
                        p.cursorBlink = true;
                        auto weak = std::weak_ptr<LineEditBase>(std::dynamic_pointer_cast<LineEditBase>(shared_from_this()));
                        p.cursorBlinkTimer->start(
                            std::chrono::milliseconds(cursorTimeout),
                            [weak](float)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->cursorBlink = !widget->_p->cursorBlink;
                                widget->_redraw();
                            }
                        });
                    }

                    _resize();
                }
            }
        }

    } // namespace UI
} // namespace djv
