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

#include <djvViewLib/FileActions.h>

#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/Util.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/StylePrefs.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDir>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        struct FileActions::Private
        {
            int layer = 0;
            QStringList layers;
        };

        FileActions::FileActions(
            const QPointer<ViewContext> & context,
            QObject * parent) :
            AbstractActions(context, parent),
            _p(new Private)
        {
            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                _actions[i] = new QAction(this);
            }
            _actions[OPEN]->setText(qApp->translate("djv::ViewLib::FileActions", "&Open"));
            _actions[RELOAD]->setText(qApp->translate("djv::ViewLib::FileActions", "Re&load"));
            _actions[RELOAD_FRAME]->setText(qApp->translate("djv::ViewLib::FileActions", "Reload Frame"));
            _actions[CLOSE]->setText(qApp->translate("djv::ViewLib::FileActions", "Clos&e"));
            _actions[EXPORT_SEQUENCE]->setText(qApp->translate("djv::ViewLib::FileActions", "E&xport Sequence"));
            _actions[EXPORT_FRAME]->setText(qApp->translate("djv::ViewLib::FileActions", "Export &Frame"));
            _actions[LAYER_PREV]->setText(qApp->translate("djv::ViewLib::FileActions", "Previous Layer"));
            _actions[LAYER_NEXT]->setText(qApp->translate("djv::ViewLib::FileActions", "Next Layer"));
            _actions[U8_CONVERSION]->setText(qApp->translate("djv::ViewLib::FileActions", "&8-bit Conversion"));
            _actions[U8_CONVERSION]->setCheckable(true);
            _actions[CACHE]->setText(qApp->translate("djv::ViewLib::FileActions", "&Memory Cache"));
            _actions[CACHE]->setCheckable(true);
            _actions[PRELOAD]->setText(qApp->translate("djv::ViewLib::FileActions", "Preloa&d Cache"));
            _actions[PRELOAD]->setCheckable(true);
            _actions[CLEAR_CACHE]->setText(qApp->translate("djv::ViewLib::FileActions", "Clear Cac&he"));
            _actions[MESSAGES]->setText(qApp->translate("djv::ViewLib::FileActions", "Messa&ges"));
            _actions[PREFS]->setText(qApp->translate("djv::ViewLib::FileActions", "&Preferences"));
            _actions[DEBUG_LOG]->setText(qApp->translate("djv::ViewLib::FileActions", "Debugging Log"));
            _actions[EXIT]->setText(qApp->translate("djv::ViewLib::FileActions", "E&xit"));

            for (int i = 0; i < GROUP_COUNT; ++i)
            {
                _groups[i] = new QActionGroup(this);
            }
            _groups[LAYER_GROUP]->setExclusive(true);
            _groups[PROXY_GROUP]->setExclusive(true);
            for (int i = 0; i < AV::PixelDataInfo::proxyLabels().count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(AV::PixelDataInfo::proxyLabels()[i]);
                action->setCheckable(true);
                action->setData(i);
                _groups[PROXY_GROUP]->addAction(action);
            }

            update();

            connect(
                context->filePrefs(),
                SIGNAL(recentChanged(const djv::Core::FileInfoList &)),
                SLOT(update()));
            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(update()));
            connect(
                context->stylePrefs(),
                SIGNAL(prefChanged()),
                SLOT(update()));
        }

        FileActions::~FileActions()
        {}

        void FileActions::setLayers(const QStringList & layers)
        {
            if (layers == _p->layers)
                return;
            _p->layers = layers;
            update();
        }

        void FileActions::setLayer(int layer)
        {
            if (layer == _p->layer)
                return;
            _p->layer = layer;
            update();
        }

        void FileActions::update()
        {
            const QVector<UI::Shortcut> & shortcuts = context()->shortcutPrefs()->shortcuts();

            _actions[OPEN]->setIcon(context()->iconLibrary()->icon("djv/UI/FileOpenIcon"));
            _actions[OPEN]->setShortcut(shortcuts[Enum::SHORTCUT_FILE_OPEN].value);
            _actions[OPEN]->setToolTip(
                qApp->translate("djv::ViewLib::FileActions",
                "Open a new file<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_FILE_OPEN].value.toString()));
            _actions[OPEN]->setWhatsThis(
                qApp->translate("djv::ViewLib::FileActions",
                "Open a new file<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"djv_view.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_FILE_OPEN].value.toString()));
            
            _actions[RELOAD]->setIcon(context()->iconLibrary()->icon("djv/UI/FileReloadIcon"));
            _actions[RELOAD]->setShortcut(shortcuts[Enum::SHORTCUT_FILE_RELOAD].value);
            _actions[RELOAD]->setToolTip(
                qApp->translate("djv::ViewLib::FileActions",
                "Reload the current file<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_FILE_RELOAD].value.toString()));
            _actions[RELOAD]->setWhatsThis(
                qApp->translate("djv::ViewLib::FileActions",
                "Reload the current file<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"djv_view.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_FILE_RELOAD].value.toString()));

            _actions[RELOAD_FRAME]->setShortcut(shortcuts[Enum::SHORTCUT_FILE_RELOAD_FRAME].value);
            _actions[RELOAD_FRAME]->setToolTip(
                qApp->translate("djv::ViewLib::FileActions",
                "Reload the current frame<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_FILE_RELOAD_FRAME].value.toString()));
            _actions[RELOAD_FRAME]->setWhatsThis(
                qApp->translate("djv::ViewLib::FileActions",
                "Reload the current frame<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"djv_view.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_FILE_RELOAD_FRAME].value.toString()));
            
            _actions[CLOSE]->setIcon(context()->iconLibrary()->icon("djv/UI/FileCloseIcon"));
            _actions[CLOSE]->setShortcut(shortcuts[Enum::SHORTCUT_FILE_CLOSE].value);
            _actions[CLOSE]->setToolTip(
                qApp->translate("djv::ViewLib::FileActions",
                "Close the current file<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_FILE_CLOSE].value.toString()));
            _actions[CLOSE]->setWhatsThis(
                qApp->translate("djv::ViewLib::FileActions",
                "Close the current file<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"djv_view.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_FILE_CLOSE].value.toString()));
            
            _actions[EXPORT_SEQUENCE]->setShortcut(shortcuts[Enum::SHORTCUT_FILE_EXPORT_SEQUENCE].value);
            _actions[EXPORT_FRAME]->setShortcut(shortcuts[Enum::SHORTCUT_FILE_EXPORT_FRAME].value);
            _actions[LAYER_NEXT]->setShortcut(shortcuts[Enum::SHORTCUT_FILE_LAYER_NEXT].value);
            _actions[LAYER_PREV]->setShortcut(shortcuts[Enum::SHORTCUT_FILE_LAYER_PREV].value);
            _actions[EXIT]->setShortcut(shortcuts[Enum::SHORTCUT_EXIT].value);

            Q_FOREACH(QAction * action, _groups[RECENT_GROUP]->actions())
                delete action;
            const Core::FileInfoList & recent = context()->filePrefs()->recentFiles();
            for (int i = 0; i < recent.count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(QDir::toNativeSeparators(recent[i]));
                action->setData(i);
                _groups[RECENT_GROUP]->addAction(action);
            }

            Q_FOREACH(QAction * action, _groups[LAYER_GROUP]->actions())
                delete action;
            const QVector<QKeySequence> layerShortcuts = QVector<QKeySequence>() <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_DEFAULT].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_1].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_2].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_3].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_4].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_5].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_6].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_7].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_8].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_9].value <<
                shortcuts[Enum::SHORTCUT_FILE_LAYER_10].value;
            for (int i = 0; i < _p->layers.count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(_p->layers[i]);
                action->setCheckable(true);
                action->setChecked(i == _p->layer);
                action->setShortcut(i < layerShortcuts.count() ? layerShortcuts[i] : QKeySequence());
                action->setData(i);
                _groups[LAYER_GROUP]->addAction(action);
            }

            const QVector<QKeySequence> proxyShortcuts = QVector<QKeySequence>() <<
                shortcuts[Enum::SHORTCUT_FILE_PROXY_NONE].value <<
                shortcuts[Enum::SHORTCUT_FILE_PROXY_1_2].value <<
                shortcuts[Enum::SHORTCUT_FILE_PROXY_1_4].value <<
                shortcuts[Enum::SHORTCUT_FILE_PROXY_1_8].value;
            for (int i = 0; i < _groups[PROXY_GROUP]->actions().count(); ++i)
            {
                _groups[PROXY_GROUP]->actions()[i]->setShortcut(proxyShortcuts[i]);
            }

            Q_EMIT changed();
        }

    } // namespace ViewLib
} // namespace djv
