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

#include <djvViewLib/AnnotationsPrefs.h>

#include <djvViewLib/ViewContext.h>

#include <djvUI/Prefs.h>

#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        AnnotationsPrefs::AnnotationsPrefs(const QPointer<ViewContext> & context, QObject * parent) :
            AbstractPrefs(context, parent),
            _primitive(Enum::ANNOTATIONS_POLYLINE),
            _color(Enum::ANNOTATIONS_RED),
            _lineWidth(5)
        {
            UI::Prefs prefs("djv::ViewLib::AnnotationsPrefs");
            prefs.get("primitive", _primitive);
            prefs.get("color", _color);
            prefs.get("lineWidth", _lineWidth);
        }

        AnnotationsPrefs::~AnnotationsPrefs()
        {
            UI::Prefs prefs("djv::ViewLib::AnnotationsPrefs");
            prefs.set("primitive", _primitive);
            prefs.set("color", _color);
            prefs.set("lineWidth", _lineWidth);
        }

        Enum::ANNOTATIONS_PRIMITIVE AnnotationsPrefs::primitive() const
        {
            return _primitive;
        }

        Enum::ANNOTATIONS_COLOR AnnotationsPrefs::color() const
        {
            return _color;
        }

        size_t AnnotationsPrefs::lineWidth() const
        {
            return _lineWidth;
        }

        void AnnotationsPrefs::setPrimitive(Enum::ANNOTATIONS_PRIMITIVE value)
        {
            if (value == _primitive)
                return;
            _primitive = value;
            Q_EMIT primitiveChanged(_primitive);
            Q_EMIT prefChanged();
        }

        void AnnotationsPrefs::setColor(Enum::ANNOTATIONS_COLOR value)
        {
            if (value == _color)
                return;
            _color = value;
            Q_EMIT colorChanged(_color);
            Q_EMIT prefChanged();
        }

        void AnnotationsPrefs::setLineWidth(size_t value)
        {
            if (value == _lineWidth)
                return;
            _lineWidth = value;
            Q_EMIT lineWidthChanged(_lineWidth);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
