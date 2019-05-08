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

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            Enum::ANNOTATE_PRIMITIVE primitiveDefault       = Enum::ANNOTATE_FREEHAND_LINE;
            AV::Color                colorDefault           = AV::Color(1.f, 0.f, 0.f);
            size_t                   lineWidthDefault       = 5;
            bool                     listVisibleDefault     = false;
            QString                  exportExtensionDefault = ".jpg";
        
        } // namespace

        AnnotatePrefs::AnnotatePrefs(const QPointer<ViewContext> & context, QObject * parent) :
            AbstractPrefs(context, parent),
            _primitive(primitiveDefault),
            _color(colorDefault),
            _lineWidth(lineWidthDefault),
            _listVisible(listVisibleDefault),
            _exportExtension(exportExtensionDefault)
        {
            UI::Prefs prefs("djv::ViewLib::AnnotatePrefs");
            prefs.get("primitive", _primitive);
            prefs.get("color", _color);
            int lineWidth = 0;
            prefs.get("lineWidth", lineWidth);
            _lineWidth = Math::clamp(lineWidth, 1, 100);
            prefs.get("listVisible", _listVisible);
            prefs.get("exportExtension", _exportExtension);
            prefs.get("exportScript", _exportScript);
            prefs.get("exportScriptOptions", _exportScriptOptions);
            prefs.get("exportScriptInterpreter", _exportScriptInterpreter);
        }

        AnnotatePrefs::~AnnotatePrefs()
        {
            UI::Prefs prefs("djv::ViewLib::AnnotatePrefs");
            prefs.set("primitive", _primitive);
            prefs.set("color", _color);
            prefs.set("lineWidth", static_cast<int>(_lineWidth));
            prefs.set("listVisible", _listVisible);
            prefs.set("exportExtension", _exportExtension);
            prefs.set("exportScript", _exportScript);
            prefs.set("exportScriptOptions", _exportScriptOptions);
            prefs.set("exportScriptInterpreter", _exportScriptInterpreter);
        }

        Enum::ANNOTATE_PRIMITIVE AnnotatePrefs::primitive() const
        {
            return _primitive;
        }

        const AV::Color & AnnotatePrefs::color() const
        {
            return _color;
        }

        size_t AnnotatePrefs::lineWidth() const
        {
            return _lineWidth;
        }

        bool AnnotatePrefs::isListVisible() const
        {
            return _listVisible;
        }

        const QString& AnnotatePrefs::exportExtension() const
        {
            return _exportExtension;
        }

        const FileInfo& AnnotatePrefs::exportScript() const
        {
            return _exportScript;
        }

        const QString& AnnotatePrefs::exportScriptOptions() const
        {
            return _exportScriptOptions;
        }

        const QString& AnnotatePrefs::exportScriptInterpreter() const
        {
            return _exportScriptInterpreter;
        }

        void AnnotatePrefs::reset()
        {
            setPrimitive(primitiveDefault);
            setColor(colorDefault);
            setLineWidth(lineWidthDefault);
            setListVisible(listVisibleDefault);
            setExportExtension(exportExtensionDefault);
            setExportScript(QString());
            setExportScriptOptions(QString());
            setExportScriptInterpreter(QString());
        }

        void AnnotatePrefs::setPrimitive(Enum::ANNOTATE_PRIMITIVE value)
        {
            if (value == _primitive)
                return;
            _primitive = value;
            Q_EMIT primitiveChanged(_primitive);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setColor(const AV::Color & value)
        {
            if (value == _color)
                return;
            _color = value;
            Q_EMIT colorChanged(_color);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setLineWidth(size_t value)
        {
            if (value == _lineWidth)
                return;
            _lineWidth = value;
            Q_EMIT lineWidthChanged(_lineWidth);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setListVisible(bool value)
        {
            if (value == _listVisible)
                return;
            _listVisible = value;
            Q_EMIT listVisibleChanged(_listVisible);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setExportExtension(const QString& value)
        {
            if (value == _exportExtension)
                return;
            _exportExtension = value;
            Q_EMIT exportExtensionChanged(_exportExtension);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setExportScript(const FileInfo& value)
        {
            if (value == _exportScript)
                return;
            _exportScript = value;
            Q_EMIT exportScriptChanged(_exportScript);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setExportScriptOptions(const QString& value)
        {
            if (value == _exportScriptOptions)
                return;
            _exportScriptOptions = value;
            Q_EMIT exportScriptOptionsChanged(_exportScriptOptions);
            Q_EMIT prefChanged();
        }

        void AnnotatePrefs::setExportScriptInterpreter(const QString& value)
        {
            if (value == _exportScriptInterpreter)
                return;
            _exportScriptInterpreter = value;
            Q_EMIT exportScriptInterpreterChanged(_exportScriptInterpreter);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
