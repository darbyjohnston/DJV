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

#include <djvUI/Shortcut.h>
#include <djvUI/Style.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <codecvt>
#include <locale>

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
            std::shared_ptr<AV::Font::System> fontSystem;

            std::string text;
            std::wstring_convert<std::codecvt_utf8<djv_char_t>, djv_char_t> utf32Convert;
            std::basic_string<djv_char_t> utf32;

            ColorRole textColorRole = ColorRole::Foreground;
            MetricsRole textSizeRole = MetricsRole::TextColumn;
            std::string font;
            std::string fontFace = AV::Font::faceDefault;
            MetricsRole fontSizeRole = MetricsRole::FontMedium;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            
            glm::vec2 textSize = glm::vec2(0.f, 0.f);
            std::future<glm::vec2> textSizeFuture;
            std::string sizeString;
            glm::vec2 sizeStringSize = glm::vec2(0.f, 0.f);
            std::future<glm::vec2> sizeStringFuture;
            size_t cursorPos = 0;
            size_t selectionAnchor = std::string::npos;
            std::future<std::vector<BBox2f> > glyphGeomFuture;
            std::vector<BBox2f> glyphGeom;
            bool cursorBlink = false;
            Event::PointerID pressedID = Event::InvalidID;

            std::shared_ptr<Time::Timer> cursorBlinkTimer;
            
            std::function<void(std::string)> textChangedCallback;
            std::function<void(std::string)> textFinishedCallback;
            std::function<void(bool)> focusCallback;
        };

        void LineEditBase::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::LineEditBase");
            setVAlign(VAlign::Center);
            setPointerEnabled(true);

            p.fontSystem = context->getSystemT<AV::Font::System>();

            p.cursorBlinkTimer = Time::Timer::create(context);
            p.cursorBlinkTimer->setRepeating(true);

            _textUpdate();
        }

        LineEditBase::LineEditBase() :
            _p(new Private)
        {}

        LineEditBase::~LineEditBase()
        {}

        std::shared_ptr<LineEditBase> LineEditBase::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<LineEditBase>(new LineEditBase);
            out->_init(context);
            return out;
        }

        const std::string & LineEditBase::getText() const
        {
            return _p->text;
        }

        void LineEditBase::setText(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.text)
                return;
            p.text = value;
            p.utf32 = _toUtf32(p.text);
            p.cursorPos = p.utf32.size();
            p.selectionAnchor = p.cursorPos;
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

        MetricsRole LineEditBase::getTextSizeRole() const
        {
            return _p->textSizeRole;
        }

        void LineEditBase::setTextSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textSizeRole)
                return;
            p.textSizeRole = value;
            _resize();
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

        const std::string & LineEditBase::getSizeString() const
        {
            return _p->sizeString;
        }

        void LineEditBase::setSizeString(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sizeString)
                return;
            p.sizeString = value;
            _textUpdate();
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

        bool LineEditBase::acceptFocus(TextFocusDirection)
        {
            bool out = false;
            if (isEnabled(true) && isVisible(true) && !isClipped())
            {
                takeTextFocus();
                out = true;
            }
            return out;
        }

        void LineEditBase::_styleEvent(Event::Style & event)
        {
            _textUpdate();
            _cursorUpdate();
        }

        void LineEditBase::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float m = style->getMetric(MetricsRole::MarginSmall);
            const float tc = style->getMetric(p.textSizeRole);

            if (p.fontMetricsFuture.valid())
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            if (p.textSizeFuture.valid())
            {
                try
                {
                    p.textSize = p.textSizeFuture.get();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            if (p.sizeStringFuture.valid())
            {
                try
                {
                    p.sizeStringSize = p.sizeStringFuture.get();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            if (p.glyphGeomFuture.valid())
            {
                try
                {
                    p.glyphGeom = p.glyphGeomFuture.get();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }

            const glm::vec2 size(p.sizeString.empty() ? tc : p.sizeStringSize.x, p.fontMetrics.lineHeight);
            _setMinimumSize(size + m * 2.f + getMargin().getSize(style));
        }

        void LineEditBase::_clipEvent(Event::Clip & event)
        {
            if (isClipped())
            {
                releaseTextFocus();
            }
        }

        void LineEditBase::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f & g = getMargin().bbox(getGeometry(), style);
            const glm::vec2 c = g.getCenter();
            const float m = style->getMetric(MetricsRole::MarginSmall);
            const float b = style->getMetric(MetricsRole::Border);

            auto render = _getRender();
            render->setFillColor(style->getColor(ColorRole::Checked));
            if (p.cursorPos != p.selectionAnchor)
            {
                float x0 = 0.f;
                float x1 = 0.f;
                const size_t glyphGeomSize = p.glyphGeom.size();
                if (glyphGeomSize)
                {
                    if (p.cursorPos < glyphGeomSize)
                    {
                        x0 = p.glyphGeom[p.cursorPos].min.x;
                    }
                    else
                    {
                        const BBox2f& geom = p.glyphGeom[glyphGeomSize - 1];
                        x0 = geom.min.x + geom.w();
                    }
                    if (p.selectionAnchor < glyphGeomSize)
                    {
                        x1 = p.glyphGeom[p.selectionAnchor].min.x;
                    }
                    else
                    {
                        const BBox2f& geom = p.glyphGeom[glyphGeomSize - 1];
                        x1 = geom.min.x + geom.w();
                    }
                }
                if (x1 < x0)
                {
                    const float tmp = x0;
                    x0 = x1;
                    x1 = tmp;
                }
                render->drawRect(BBox2f(
                    g.min.x + m + x0,
                    g.min.y + m,
                    x1 - x0,
                    g.h() - m * 2.f));
            }

            auto fontInfo = p.font.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
            render->setCurrentFont(fontInfo);
            render->setFillColor(style->getColor(p.textColorRole));
            //! \bug Why the extra subtract by one here?
            glm::vec2 pos = g.min;
            pos += m;
            pos.y = c.y - p.textSize.y / 2.f;
            render->drawText(p.text, glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.f)));

            if (p.cursorBlink)
            {
                float x = 0.f;
                const size_t glyphGeomSize = p.glyphGeom.size();
                if (glyphGeomSize)
                {
                    if (p.cursorPos < glyphGeomSize)
                    {
                        x = p.glyphGeom[p.cursorPos].min.x;
                    }
                    else
                    {
                        const BBox2f& geom = p.glyphGeom[glyphGeomSize - 1];
                        x = geom.min.x + geom.w();
                    }
                }
                render->drawRect(BBox2f(
                    g.min.x + m + x,
                    g.min.y + m,
                    b,
                    g.h() - m * 2.f));
            }
        }

        void LineEditBase::_pointerEnterEvent(Event::PointerEnter & event)
        {
            if (!event.isRejected())
            {
                event.accept();
            }
        }

        void LineEditBase::_pointerLeaveEvent(Event::PointerLeave & event)
        {
            event.accept();
        }

        void LineEditBase::_pointerMoveEvent(Event::PointerMove& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto& pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                const auto& style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(MetricsRole::MarginSmall);
                float x = event.getPointerInfo().projectedPos.x - g.min.x - m;
                size_t cursorPos = 0;
                if (x >= 0.f)
                {
                    for (const auto& i : p.glyphGeom)
                    {
                        if (x >= i.min.x && x <= i.max.x)
                        {
                            break;
                        }
                        ++cursorPos;
                    }
                }
                if (cursorPos != p.cursorPos)
                {
                    p.cursorPos = cursorPos;
                    _cursorUpdate();
                }
            }
        }

        void LineEditBase::_buttonPressEvent(Event::ButtonPress & event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID || !isEnabled(true))
                return;
            event.accept();
            takeTextFocus();
            const auto& pointerInfo = event.getPointerInfo();
            p.pressedID = pointerInfo.id;
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            const float m = style->getMetric(MetricsRole::MarginSmall);
            float x = event.getPointerInfo().projectedPos.x - g.min.x - m;
            size_t cursorPos = 0;
            for (const auto& i : p.glyphGeom)
            {
                if (x >= i.min.x && x <= i.max.x)
                {
                    break;
                }
                ++cursorPos;
            }
            p.cursorPos = cursorPos;
            p.selectionAnchor = cursorPos;
            _cursorUpdate();
        }

        void LineEditBase::_buttonReleaseEvent(Event::ButtonRelease & event)
        {
            DJV_PRIVATE_PTR();
            const auto& pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                event.accept();
                p.pressedID = Event::InvalidID;
            }
        }

        void LineEditBase::_keyPressEvent(Event::KeyPress & event)
        {
            Widget::_keyPressEvent(event);
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (!event.isAccepted())
                {
                    const size_t size = p.utf32.size();
                    const int modifiers = event.getKeyModifiers();
                    switch (event.getKey())
                    {
                    case GLFW_KEY_BACKSPACE:
                    {
                        event.accept();
                        const auto& selection = _getSelection();
                        if (size > 0 && (p.cursorPos > 0 || selection.min != selection.max))
                        {
                            if (selection.min != selection.max)
                            {
                                p.utf32.erase(selection.min, std::min(selection.max - selection.min, size));
                                p.cursorPos = selection.min;
                            }
                            else
                            {
                                p.utf32.erase(selection.min - 1, 1);
                                --p.cursorPos;
                            }
                            p.text = _fromUtf32(p.utf32);
                            p.selectionAnchor = p.cursorPos;
                            _textUpdate();
                            _cursorUpdate();
                            if (p.textChangedCallback)
                            {
                                p.textChangedCallback(p.text);
                            }
                        }
                        break;
                    }
                    case GLFW_KEY_DELETE:
                    {
                        event.accept();
                        const auto& selection = _getSelection();
                        if (size > 0 && (p.cursorPos < size || selection.min != selection.max))
                        {
                            if (selection.min != selection.max)
                            {
                                p.utf32.erase(selection.min, std::min(selection.max - selection.min, size));
                                p.cursorPos = selection.min;
                            }
                            else
                            {
                                p.utf32.erase(p.cursorPos, 1);
                            }
                            p.text = _fromUtf32(p.utf32);
                            p.selectionAnchor = p.cursorPos;
                            _textUpdate();
                            _cursorUpdate();
                            if (p.textChangedCallback)
                            {
                                p.textChangedCallback(p.text);
                            }
                        }
                        break;
                    }
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
                            if (!(modifiers & GLFW_MOD_SHIFT))
                            {
                                p.selectionAnchor = p.cursorPos;
                            }
                            _cursorUpdate();
                        }
                        break;
                    case GLFW_KEY_RIGHT:
                        event.accept();
                        if (p.cursorPos < size)
                        {
                            ++p.cursorPos;
                            if (!(modifiers & GLFW_MOD_SHIFT))
                            {
                                p.selectionAnchor = p.cursorPos;
                            }
                            _cursorUpdate();
                        }
                        break;
                    case GLFW_KEY_HOME:
                        event.accept();
                        if (p.cursorPos > 0)
                        {
                            p.cursorPos = 0;
                            if (!(modifiers & GLFW_MOD_SHIFT))
                            {
                                p.selectionAnchor = p.cursorPos;
                            }
                            _cursorUpdate();
                        }
                        break;
                    case GLFW_KEY_END:
                        event.accept();
                        if (size > 0 && p.cursorPos != size)
                        {
                            p.cursorPos = size;
                            if (!(modifiers & GLFW_MOD_SHIFT))
                            {
                                p.selectionAnchor = p.cursorPos;
                            }
                            _cursorUpdate();
                        }
                        break;
                    case GLFW_KEY_ESCAPE:
                        if (hasTextFocus())
                        {
                            event.accept();
                            releaseTextFocus();
                        }
                        break;
                    case GLFW_KEY_UP:
                    case GLFW_KEY_DOWN:
                    case GLFW_KEY_PAGE_UP:
                    case GLFW_KEY_PAGE_DOWN:
                        break;
                    case GLFW_KEY_A:
                        if (modifiers & UI::Shortcut::getSystemModifier())
                        {
                            event.accept();
                            p.cursorPos = 0;
                            p.selectionAnchor = p.utf32.size();
                            _textUpdate();
                            _cursorUpdate();
                        }
                        break;
                    case GLFW_KEY_X:
                        if (modifiers & UI::Shortcut::getSystemModifier())
                        {
                            event.accept();
                            const auto& selection = _getSelection();
                            if (selection.min != selection.max)
                            {
                                const auto utf32 = p.utf32.substr(selection.min, selection.max - selection.min);
                                p.utf32.erase(selection.min, selection.max - selection.min);
                                auto eventSystem = context->getSystemT<Event::IEventSystem>();
                                eventSystem->setClipboard(_fromUtf32(utf32));
                                p.text = _fromUtf32(p.utf32);
                                p.cursorPos = selection.min;
                                p.selectionAnchor = p.cursorPos;
                                _textUpdate();
                                _cursorUpdate();
                                if (p.textChangedCallback)
                                {
                                    p.textChangedCallback(p.text);
                                }
                            }
                        }
                        break;
                    case GLFW_KEY_C:
                        if (modifiers & UI::Shortcut::getSystemModifier())
                        {
                            event.accept();
                            const auto& selection = _getSelection();
                            if (selection.min != selection.max)
                            {
                                const auto utf32 = p.utf32.substr(selection.min, selection.max - selection.min);
                                auto eventSystem = context->getSystemT<Event::IEventSystem>();
                                eventSystem->setClipboard(_fromUtf32(utf32));
                            }
                        }
                        break;
                    case GLFW_KEY_V:
                        if (modifiers & UI::Shortcut::getSystemModifier())
                        {
                            event.accept();
                            auto eventSystem = context->getSystemT<Event::IEventSystem>();
                            const auto utf32 = _toUtf32(eventSystem->getClipboard());
                            const auto& selection = _getSelection();
                            if (selection.min != selection.max)
                            {
                                p.utf32.replace(selection.min, selection.max - selection.min, utf32);
                                p.cursorPos = selection.min + utf32.size();
                            }
                            else
                            {
                                p.utf32.insert(p.cursorPos, utf32);
                                p.cursorPos += utf32.size();
                            }
                            p.text = _fromUtf32(p.utf32);
                            p.selectionAnchor = p.cursorPos;
                            _textUpdate();
                            _cursorUpdate();
                            if (p.textChangedCallback)
                            {
                                p.textChangedCallback(p.text);
                            }
                        }
                        break;
                    default:
                        if (!event.getKeyModifiers())
                        {
                            event.accept();
                        }
                        break;
                    }
                }
            }
        }

        void LineEditBase::_textFocusEvent(Event::TextFocus& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            _cursorUpdate();
            _redraw();
            if (p.focusCallback)
            {
                p.focusCallback(true);
            }
        }

        void LineEditBase::_textFocusLostEvent(Event::TextFocusLost & event)
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

        void LineEditBase::_textEvent(Event::Text & event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const size_t size = p.utf32.size();
            const auto& selection = _getSelection();
            const auto& utf32 = event.getUtf32();
            if (selection.min != selection.max)
            {
                p.utf32.replace(selection.min, selection.max - selection.min, utf32);
                p.cursorPos = selection.min + utf32.size();
            }
            else
            {
                p.utf32.insert(p.cursorPos, utf32);
                p.cursorPos += utf32.size();
            }
            p.text = _fromUtf32(p.utf32);
            p.selectionAnchor = p.cursorPos;
            _textUpdate();
            _cursorUpdate();
            if (p.textChangedCallback)
            {
                p.textChangedCallback(p.text);
            }
        }

        std::string LineEditBase::_fromUtf32(const std::basic_string<djv_char_t>& value)
        {
            std::string out;
            try
            {
                out = _p->utf32Convert.to_bytes(value);
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                ss << "Error converting string: " << e.what();
                _log(ss.str(), LogLevel::Error);
            }
            return out;
        }

        std::basic_string<djv_char_t> LineEditBase::_toUtf32(const std::string& value)
        {
            std::basic_string<djv_char_t> out;
            try
            {
                std::string tmp = value;
                for (auto i = tmp.begin(); i != tmp.end(); ++i)
                {
                    if ('\n' == *i)
                    {
                        *i = ' ';
                    }
                    else if('\r' == *i)
                    {
                        *i = ' ';
                    }
                }
                out = _p->utf32Convert.from_bytes(tmp);
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                ss << "Error converting string" << " '" << value << "': " << e.what();
                _log(ss.str(), LogLevel::Error);
            }
            return out;
        }

        SizeTRange LineEditBase::_getSelection() const
        {
            DJV_PRIVATE_PTR();
            SizeTRange out(p.cursorPos, p.selectionAnchor);
            out.sort();
            return out;
        }

        void LineEditBase::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const auto fontInfo = p.font.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
            p.fontMetricsFuture = p.fontSystem->getMetrics(fontInfo);
            p.textSizeFuture = p.fontSystem->measure(p.text, fontInfo);
            if (!p.sizeString.empty())
            {
                p.sizeStringFuture = p.fontSystem->measure(p.sizeString, fontInfo);
            }
            _resize();
        }

        void LineEditBase::_cursorUpdate()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.utf32.size();
            if (size)
            {
                p.cursorPos = Math::clamp(p.cursorPos, size_t(0), size);
                p.selectionAnchor = Math::clamp(p.selectionAnchor, size_t(0), size);
            }
            else
            {
                p.cursorPos = 0;
                p.selectionAnchor = 0;
            }

            const auto& style = _getStyle();
            const auto fontInfo = p.font.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.font, p.fontFace, p.fontSizeRole);
            p.glyphGeomFuture = p.fontSystem->measureGlyphs(p.text, fontInfo);

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

    } // namespace UI
} // namespace djv
