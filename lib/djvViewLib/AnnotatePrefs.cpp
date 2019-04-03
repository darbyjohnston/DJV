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

#include <djvViewLib/AnnotatePrefs.h>

#include <djvViewLib/ViewContext.h>

#include <djvUI/Prefs.h>

#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            Enum::ANNOTATE_PRIMITIVE  primitiveDefault = Enum::ANNOTATE_PEN;
            Enum::ANNOTATE_COLOR      colorDefault     = Enum::ANNOTATE_RED;
            Enum::ANNOTATE_LINE_WIDTH lineWidthDefault = Enum::ANNOTATE_LINE_WIDTH_5;
        
        } // namespace

        AnnotatePrefs::AnnotatePrefs(const QPointer<ViewContext> & context, QObject * parent) :
            AbstractPrefs(context, parent),
            _primitive(primitiveDefault),
            _color(colorDefault),
            _lineWidth(lineWidthDefault)
        {
            UI::Prefs prefs("djv::ViewLib::AnnotatePrefs");
            prefs.get("primitive", _primitive);
            prefs.get("color", _color);
            prefs.get("lineWidth", _lineWidth);
        }

        AnnotatePrefs::~AnnotatePrefs()
        {
            UI::Prefs prefs("djv::ViewLib::AnnotatePrefs");
            prefs.set("primitive", _primitive);
            prefs.set("color", _color);
            prefs.set("lineWidth", _lineWidth);
        }

        Enum::ANNOTATE_PRIMITIVE AnnotatePrefs::primitive() const
        {
            return _primitive;
        }

        Enum::ANNOTATE_COLOR AnnotatePrefs::color() const
        {
            return _color;
        }

        Enum::ANNOTATE_LINE_WIDTH AnnotatePrefs::lineWidth() const
        {
            return _lineWidth;
        }

        void AnnotatePrefs::reset()
        {}

        void AnnotatePrefs::setPrimitive(Enum::ANNOTATE_PRIMITIVE value)
        {
            if (value == _primitive)
                return;
            _primitive = value;
            Q_EMIT primitiveChanged(_primitive);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setColor(Enum::ANNOTATE_COLOR value)
        {
            if (value == _color)
                return;
            _color = value;
            Q_EMIT colorChanged(_color);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setLineWidth(Enum::ANNOTATE_LINE_WIDTH value)
        {
            if (value == _lineWidth)
                return;
            _lineWidth = value;
            Q_EMIT lineWidthChanged(_lineWidth);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
