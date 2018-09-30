//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvUI/FileBrowser.h>

#include <djvUI/FileBrowserModel.h>
#include <djvUI/FileBrowserPrefs.h>
#include <djvUI/UIContext.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/IndexShortcut.h>
#include <djvUI/MultiChoiceDialog.h>
#include <djvUI/Prefs.h>
#include <djvUI/QuestionDialog.h>
#include <djvUI/SearchBox.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Debug.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/ListUtil.h>
#include <djvCore/SignalBlocker.h>
#include <djvCore/System.h>

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QPointer>
#include <QPushButton>
#include <QResizeEvent>
#include <QShortcut>
#include <QTreeView>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        namespace
        {
            struct Menus
            {
                enum MENU
                {
                    RECENT,
                    DRIVES,
                    THUMBNAIL_MODE,
                    THUMBNAIL_SIZE,
                    SEQ,
                    SORT,
                    BOOKMARKS
                };

                QPointer<QMenuBar> menuBar;
                QMap<int, QPointer<QMenu> > menus;
            };

            struct Actions
            {
                enum ACTION
                {
                    UP,
                    PREV,
                    CURRENT,
                    HOME,
                    DESKTOP,
                    RELOAD,
                    SHOW_HIDDEN,
                    REVERSE_SORT,
                    SORT_DIRS_FIRST,
                    DELETE_CURRENT_BOOKMARK,
                    DELETE_ALL_BOOKMARKS
                };

                enum ACTION_GROUP
                {
                    RECENT_GROUP,
                    DRIVES_GROUP,
                    THUMBNAIL_MODE_GROUP,
                    THUMBNAIL_SIZE_GROUP,
                    SEQ_GROUP,
                    SORT_GROUP,
                    BOOKMARKS_GROUP
                };

                QMap<int, QPointer<QAction> > actions;
                QMap<int, QPointer<QActionGroup> > groups;
            };

            struct Widgets
            {
                QPointer<QLineEdit> file;
                QPointer<ToolButton> up;
                QPointer<ToolButton> prev;
                QPointer<ToolButton> reload;
                QPointer<QComboBox> seq;
                QPointer<SearchBox>  search;
                QPointer<QTreeView> browser;
                QPointer<QCheckBox> pinned;
            };

        } // namespace

        struct FileBrowser::Private
        {
            Private(const QPointer<UIContext> & context) :
                context(context)
            {}

            QPointer<UIContext> context;
            bool pinnable = false;
            bool pinned = false;
            bool shown = false;
            Core::FileInfo fileInfo;
            QString prev;
            QPointer<FileBrowserModel> model;
            Menus menus;
            Actions actions;
            Widgets widgets;
            QPointer<QHBoxLayout> buttonsLayout;
        };

        FileBrowser::FileBrowser(const QPointer<UIContext> & context, QWidget * parent) :
            QDialog(parent),
            _p(new Private(context))
        {
            //DJV_DEBUG("FileBrowser::FileBrowser");

            // Create the menus.
            _p->menus.menuBar = new QMenuBar(this);
            _p->menus.menuBar->setNativeMenuBar(false);

            auto directoryMenu = _p->menus.menuBar->addMenu(
                qApp->translate("djv::UI::FileBrowser", "&Directory"));

            _p->actions.actions[Actions::UP] = directoryMenu->addAction(
                qApp->translate("djv::UI::FileBrowser", "&Up"), this, SLOT(upCallback()));

            _p->actions.actions[Actions::PREV] = directoryMenu->addAction(
                qApp->translate("djv::UI::FileBrowser", "&Previous"), this, SLOT(prevCallback()));

            _p->menus.menus[Menus::RECENT] = directoryMenu->addMenu(
                qApp->translate("djv::UI::FileBrowser", "&Recent"));
            _p->actions.groups[Actions::RECENT_GROUP] = new QActionGroup(this);

            directoryMenu->addSeparator();

            _p->actions.actions[Actions::CURRENT] = directoryMenu->addAction(
                qApp->translate("djv::UI::FileBrowser", "&Current"), this, SLOT(currentCallback()));

            _p->actions.actions[Actions::HOME] = directoryMenu->addAction(
                qApp->translate("djv::UI::FileBrowser", "&Home"), this, SLOT(homeCallback()));

            _p->actions.actions[Actions::DESKTOP] = directoryMenu->addAction(
                qApp->translate("djv::UI::FileBrowser", "&Desktop"), this, SLOT(desktopCallback()));

            _p->menus.menus[Menus::DRIVES] = directoryMenu->addMenu(
                qApp->translate("djv::UI::FileBrowser", "&Drives"));
            _p->actions.groups[Actions::DRIVES_GROUP] = new QActionGroup(this);

            directoryMenu->addSeparator();

            _p->actions.actions[Actions::RELOAD] = directoryMenu->addAction(
                qApp->translate("djv::UI::FileBrowser", "Re&load"), this, SLOT(reloadCallback()));

            auto optionsMenu = _p->menus.menuBar->addMenu(
                qApp->translate("djv::UI::FileBrowser", "&Options"));

            _p->menus.menus[Menus::THUMBNAIL_MODE] = optionsMenu->addMenu(
                qApp->translate("djv::UI::FileBrowser", "Thu&mbnails"));
            _p->actions.groups[Actions::THUMBNAIL_MODE_GROUP] = new QActionGroup(this);
            _p->actions.groups[Actions::THUMBNAIL_MODE_GROUP]->setExclusive(true);

            _p->menus.menus[Menus::THUMBNAIL_SIZE] = optionsMenu->addMenu(
                qApp->translate("djv::UI::FileBrowser", "Thumbnail Size"));
            _p->actions.groups[Actions::THUMBNAIL_SIZE_GROUP] = new QActionGroup(this);
            _p->actions.groups[Actions::THUMBNAIL_SIZE_GROUP]->setExclusive(true);

            _p->menus.menus[Menus::SEQ] = optionsMenu->addMenu(
                qApp->translate("djv::UI::FileBrowser", "File Se&quences"));
            _p->actions.groups[Actions::SEQ_GROUP] = new QActionGroup(this);

            _p->actions.actions[Actions::SHOW_HIDDEN] =
                optionsMenu->addAction(
                    qApp->translate("djv::UI::FileBrowser", "Show &Hidden Files"),
                    this,
                    SLOT(showHiddenCallback(bool)));
            _p->actions.actions[Actions::SHOW_HIDDEN]->setCheckable(true);

            optionsMenu->addSeparator();

            _p->menus.menus[Menus::SORT] = optionsMenu->addMenu(
                qApp->translate("djv::UI::FileBrowser", "&Sort"));
            _p->actions.groups[Actions::SORT_GROUP] = new QActionGroup(this);

            _p->actions.actions[Actions::REVERSE_SORT] =
                optionsMenu->addAction(
                    qApp->translate("djv::UI::FileBrowser", "&Reverse Sort"),
                    this,
                    SLOT(reverseSortCallback(bool)));
            _p->actions.actions[Actions::REVERSE_SORT]->setCheckable(true);

            _p->actions.actions[Actions::SORT_DIRS_FIRST] =
                optionsMenu->addAction(
                    qApp->translate("djv::UI::FileBrowser", "Sort &Directories First"),
                    this,
                    SLOT(sortDirsFirstCallback(bool)));
            _p->actions.actions[Actions::SORT_DIRS_FIRST]->setCheckable(true);

            _p->menus.menus[Menus::BOOKMARKS] = _p->menus.menuBar->addMenu(
                qApp->translate("djv::UI::FileBrowser", "&Bookmarks"));

            _p->actions.groups[Actions::BOOKMARKS_GROUP] = new QActionGroup(this);

            // Create the widgets.
            _p->widgets.file = new QLineEdit;

            _p->widgets.up = new ToolButton(context);
            _p->widgets.up->setDefaultAction(_p->actions.actions[Actions::UP]);

            _p->widgets.prev = new ToolButton(context);
            _p->widgets.prev->setDefaultAction(_p->actions.actions[Actions::PREV]);

            _p->widgets.reload = new ToolButton(context);
            _p->widgets.reload->setDefaultAction(_p->actions.actions[Actions::RELOAD]);

            _p->widgets.seq = new QComboBox;
            _p->widgets.seq->addItems(Core::Sequence::compressLabels());
            QLabel * seqLabel = new QLabel(
                qApp->translate("djv::UI::FileBrowser", "File sequences:"));

            _p->widgets.search = new SearchBox(context);
            _p->widgets.search->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            _p->widgets.browser = new QTreeView;
            _p->widgets.browser->setSortingEnabled(true);
            _p->widgets.browser->sortByColumn(1, Qt::AscendingOrder);
            _p->widgets.browser->setRootIsDecorated(false);
            _p->widgets.browser->setItemsExpandable(false);
            _p->widgets.browser->setVerticalScrollMode(
                QAbstractItemView::ScrollPerPixel);
            _p->widgets.browser->setAlternatingRowColors(true);
            _p->widgets.browser->setDragDropMode(QAbstractItemView::DragOnly);
            _p->widgets.browser->setDragEnabled(true);

            _p->widgets.pinned = new QCheckBox(
                qApp->translate("djv::UI::FileBrowser", "Pin dialog"));

            QPushButton * okButton = new QPushButton(
                qApp->translate("djv::UI::FileBrowser", "Ok"));

            QPushButton * closeButton = new QPushButton(
                qApp->translate("djv::UI::FileBrowser", "Close"));
            closeButton->setAutoDefault(false);

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->setMenuBar(_p->menus.menuBar);

            auto hLayout = new QHBoxLayout;

            _p->buttonsLayout = new QHBoxLayout;
            _p->buttonsLayout->setMargin(0);
            _p->buttonsLayout->setSpacing(0);
            _p->buttonsLayout->addWidget(_p->widgets.up);
            _p->buttonsLayout->addWidget(_p->widgets.prev);
            _p->buttonsLayout->addWidget(_p->widgets.reload);
            hLayout->addLayout(_p->buttonsLayout);

            auto hLayout2 = new QHBoxLayout;
            hLayout2->setMargin(0);
            hLayout2->addWidget(seqLabel);
            hLayout2->addWidget(_p->widgets.seq);
            hLayout->addLayout(hLayout2);

            hLayout->addStretch();
            hLayout->addWidget(_p->widgets.search);

            layout->addLayout(hLayout);

            layout->addWidget(_p->widgets.browser);
            layout->addWidget(_p->widgets.file);

            hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->widgets.pinned);
            hLayout->addStretch();
            hLayout->addWidget(okButton);
            hLayout->addWidget(closeButton);
            layout->addLayout(hLayout);

            // Restore the last used path.
            _p->fileInfo = Core::FileInfoUtil::fixPath(FileBrowserPrefs::pathDefault());
            _p->prev = _p->fileInfo;
            //DJV_DEBUG_PRINT("file = " << _p->fileInfo);

            // Load the preferences.
            _p->model = new FileBrowserModel(context, this);
            _p->model->setSequence(context->fileBrowserPrefs()->sequence());
            _p->model->setShowHidden(context->fileBrowserPrefs()->hasShowHidden());
            _p->model->setColumnsSort(context->fileBrowserPrefs()->columnsSort());
            _p->model->setReverseSort(context->fileBrowserPrefs()->hasReverseSort());
            _p->model->setSortDirsFirst(context->fileBrowserPrefs()->hasSortDirsFirst());
            _p->model->setThumbnailMode(context->fileBrowserPrefs()->thumbnailMode());
            _p->model->setThumbnailSize(context->fileBrowserPrefs()->thumbnailSize());
            _p->model->setPath(_p->fileInfo);

            // Initialize.
            setWindowTitle(qApp->translate("djv::UI::FileBrowser", "File Browser"));
            setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
            _p->widgets.browser->setModel(_p->model);
            const QVector<int> sizes = columnSizes();
            for (int i = 0; i < sizes.count(); ++i)
            {
                _p->widgets.browser->header()->resizeSection(i, sizes[i]);
            }
            styleUpdate();
            widgetUpdate();
            menuUpdate();
            toolTipUpdate();
            resize(800, 600);

            // Setup the callbacks.
            connect(
                _p->model,
                SIGNAL(optionChanged()),
                SLOT(widgetUpdate()));
            connect(
                _p->model,
                SIGNAL(optionChanged()),
                SLOT(menuUpdate()));
            connect(
                _p->model,
                SIGNAL(optionChanged()),
                SLOT(widgetUpdate()));
            connect(
                _p->model,
                SIGNAL(optionChanged()),
                SLOT(menuUpdate()));
            _p->model->connect(
                context->fileBrowserPrefs(),
                SIGNAL(sequenceChanged(djv::Core::Sequence::COMPRESS)),
                SLOT(setSequence(djv::Core::Sequence::COMPRESS)));
            _p->model->connect(
                context->fileBrowserPrefs(),
                SIGNAL(showHiddenChanged(bool)),
                SLOT(setShowHidden(bool)));
            _p->model->connect(
                context->fileBrowserPrefs(),
                SIGNAL(columnsSortChanged(djv::UI::FileBrowserModel::COLUMNS)),
                SLOT(setColumnsSort(djv::UI::FileBrowserModel::COLUMNS)));
            _p->model->connect(
                context->fileBrowserPrefs(),
                SIGNAL(reverseSortChanged(bool)),
                SLOT(setReverseSort(bool)));
            _p->model->connect(
                context->fileBrowserPrefs(),
                SIGNAL(sortDirsFirstChanged(bool)),
                SLOT(setSortDirsFirst(bool)));
            connect(
                _p->actions.groups[Actions::RECENT_GROUP],
                SIGNAL(triggered(QAction *)),
                SLOT(recentCallback(QAction *)));
            connect(
                _p->actions.groups[Actions::DRIVES_GROUP],
                SIGNAL(triggered(QAction *)),
                SLOT(drivesCallback(QAction *)));
            connect(
                _p->actions.groups[Actions::THUMBNAIL_MODE_GROUP],
                SIGNAL(triggered(QAction *)),
                SLOT(thumbnailModeCallback(QAction *)));
            connect(
                _p->actions.groups[Actions::THUMBNAIL_SIZE_GROUP],
                SIGNAL(triggered(QAction *)),
                SLOT(thumbnailSizeCallback(QAction *)));
            connect(
                _p->actions.groups[Actions::SEQ_GROUP],
                SIGNAL(triggered(QAction *)),
                SLOT(seqCallback(QAction *)));
            connect(
                _p->actions.groups[Actions::SORT_GROUP],
                SIGNAL(triggered(QAction *)),
                SLOT(columnsSortCallback(QAction *)));
            connect(
                _p->actions.groups[Actions::BOOKMARKS_GROUP],
                SIGNAL(triggered(QAction *)),
                SLOT(bookmarksCallback(QAction *)));
            connect(
                _p->widgets.file,
                SIGNAL(editingFinished()),
                SLOT(fileCallback()));
            connect(
                _p->widgets.browser,
                SIGNAL(activated(const QModelIndex &)),
                SLOT(browserCallback(const QModelIndex &)));
            connect(
                _p->widgets.browser->selectionModel(),
                SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
                SLOT(browserCurrentCallback(const QModelIndex &, const QModelIndex &)));
            connect(
                _p->widgets.browser->header(),
                SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
                SLOT(columnsSortCallback(int, Qt::SortOrder)));
            connect(
                _p->widgets.seq,
                SIGNAL(activated(int)),
                SLOT(seqCallback(int)));
            connect(
                _p->widgets.search,
                SIGNAL(textChanged(const QString &)),
                SLOT(searchCallback(const QString &)));
            connect(
                _p->widgets.pinned,
                SIGNAL(toggled(bool)),
                SLOT(pinnedCallback(bool)));
            connect(okButton, SIGNAL(clicked()), SLOT(acceptedCallback()));
            connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
            connect(
                context->fileBrowserPrefs(),
                SIGNAL(thumbnailModeChanged(djv::UI::FileBrowserModel::THUMBNAIL_MODE)),
                SLOT(modelUpdate()));
            connect(
                context->fileBrowserPrefs(),
                SIGNAL(thumbnailSizeChanged(djv::UI::FileBrowserModel::THUMBNAIL_SIZE)),
                SLOT(modelUpdate()));
            connect(
                context->fileBrowserPrefs(),
                SIGNAL(recentChanged(const QStringList &)),
                SLOT(menuUpdate()));
            connect(
                context->fileBrowserPrefs(),
                SIGNAL(bookmarksChanged(const QStringList &)),
                SLOT(menuUpdate()));
            connect(
                context->fileBrowserPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(menuUpdate()));
            connect(
                context->fileBrowserPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(toolTipUpdate()));
        }

        FileBrowser::~FileBrowser()
        {
            //DJV_DEBUG("FileBrowser::~FileBrowser");

            // Remember the last used path.
            FileBrowserPrefs::setPathDefault(_p->model->path());

            // Save the preferences.
            _p->context->fileBrowserPrefs()->setSequence(_p->model->sequence());
            _p->context->fileBrowserPrefs()->setShowHidden(_p->model->hasShowHidden());
            _p->context->fileBrowserPrefs()->setColumnsSort(_p->model->columnsSort());
            _p->context->fileBrowserPrefs()->setReverseSort(_p->model->hasReverseSort());
            _p->context->fileBrowserPrefs()->setSortDirsFirst(_p->model->hasSortDirsFirst());
            _p->context->fileBrowserPrefs()->setThumbnailMode(_p->model->thumbnailMode());
            _p->context->fileBrowserPrefs()->setThumbnailSize(_p->model->thumbnailSize());
        }

        const Core::FileInfo & FileBrowser::fileInfo() const
        {
            return _p->fileInfo;
        }

        bool FileBrowser::isPinnable() const
        {
            return _p->pinnable;
        }

        bool FileBrowser::isPinned() const
        {
            return _p->pinned;
        }

        void FileBrowser::setFileInfo(const Core::FileInfo & fileInfo)
        {
            Core::FileInfo tmp = Core::FileInfoUtil::fixPath(fileInfo);
            tmp.setType(fileInfo.type());
            if (tmp == _p->fileInfo)
                return;
            //DJV_DEBUG("FileBrowser::setFileInfo");
            //DJV_DEBUG_PRINT("fileInfo = " << tmp);
            const QString oldDir = Core::FileInfoUtil::fixPath(
                _p->fileInfo.type() == Core::FileInfo::DIRECTORY ?
                _p->fileInfo.fileName() :
                _p->fileInfo.path());
            //DJV_DEBUG_PRINT("oldDir = " << oldDir);
            _p->fileInfo = tmp;
            //DJV_DEBUG_PRINT("file = " << _p->fileInfo);
            const QString newDir = Core::FileInfoUtil::fixPath(
                _p->fileInfo.type() == Core::FileInfo::DIRECTORY ?
                _p->fileInfo.fileName() :
                _p->fileInfo.path());
            //DJV_DEBUG_PRINT("newDir = " << newDir);
            if (newDir != oldDir)
            {
                _p->prev = oldDir;
                modelUpdate();
            }
            widgetUpdate();
        }

        void FileBrowser::setPinnable(bool pinnable)
        {
            if (pinnable == _p->pinnable)
                return;
            _p->pinnable = pinnable;
            widgetUpdate();
            Q_EMIT pinnableChanged(_p->pinnable);
        }

        void FileBrowser::setPinned(bool pinned)
        {
            if (pinned == _p->pinned)
                return;
            _p->pinned = pinned;
            Q_EMIT pinnedChanged(_p->pinned);
        }

        void FileBrowser::showEvent(QShowEvent *)
        {
            //DJV_DEBUG("FileBrowser::showEvent");
            if (!_p->shown)
            {
                modelUpdate();
                widgetUpdate();
                _p->shown = true;
            }
            _p->widgets.file->setFocus(Qt::OtherFocusReason);
        }

        void FileBrowser::fileCallback()
        {
            //DJV_DEBUG("FileBrowser::fileCallback");
            const QString fileName = Core::FileInfoUtil::fixPath(_p->widgets.file->text());
            //DJV_DEBUG_PRINT("fileName = " << fileName);
            Core::FileInfo fileInfo(fileName);
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            if (fileInfo.type() == Core::FileInfo::DIRECTORY)
            {
                //DJV_DEBUG_PRINT("valid");
                setFileInfo(fileInfo);
                _p->widgets.file->setText(QDir::toNativeSeparators(_p->fileInfo));
            }
        }

        void FileBrowser::browserCallback(const QModelIndex & index)
        {
            //DJV_DEBUG("FileBrowser::browserCallback");
            const Core::FileInfo fileInfo = _p->model->fileInfo(index);
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());
            _p->widgets.file->setText(QDir::toNativeSeparators(fileInfo));
            acceptedCallback();
        }

        void FileBrowser::browserCurrentCallback(
            const QModelIndex & index,
            const QModelIndex & previous)
        {
            //DJV_DEBUG("FileBrowser::browserCurrentCallback");
            const Core::FileInfo fileInfo = _p->model->fileInfo(index);
            //DJV_DEBUG_PRINT("file = " << fileInfo);
            _p->widgets.file->setText(QDir::toNativeSeparators(fileInfo));
        }

        void FileBrowser::recentCallback(QAction * action)
        {
            setFileInfo(Core::FileInfoUtil::fixPath(action->data().toString()));
        }

        void FileBrowser::upCallback()
        {
            QDir tmp(_p->fileInfo.path());
            tmp.cdUp();
            setFileInfo(Core::FileInfoUtil::fixPath(tmp.path()));
        }

        void FileBrowser::prevCallback()
        {
            setFileInfo(_p->prev);
        }

        void FileBrowser::drivesCallback(QAction * action)
        {
            setFileInfo(Core::FileInfoUtil::fixPath(action->data().toString()));
        }

        void FileBrowser::currentCallback()
        {
            setFileInfo(Core::FileInfoUtil::fixPath(QDir::currentPath()));
        }

        void FileBrowser::homeCallback()
        {
            setFileInfo(Core::FileInfoUtil::fixPath(QDir::homePath()));
        }

        void FileBrowser::desktopCallback()
        {
            setFileInfo(Core::FileInfoUtil::fixPath(QDir::homePath() + "/Desktop"));
        }

        void FileBrowser::reloadCallback()
        {
            _p->model->reload();
        }

        void FileBrowser::thumbnailModeCallback(QAction * action)
        {
            _p->context->fileBrowserPrefs()->setThumbnailMode(
                static_cast<FileBrowserModel::THUMBNAIL_MODE>(action->data().toInt()));
        }

        void FileBrowser::thumbnailSizeCallback(QAction * action)
        {
            _p->context->fileBrowserPrefs()->setThumbnailSize(
                static_cast<FileBrowserModel::THUMBNAIL_SIZE>(action->data().toInt()));
        }

        void FileBrowser::showHiddenCallback(bool value)
        {
            _p->model->setShowHidden(value);
        }

        void FileBrowser::showHiddenCallback()
        {
            _p->model->setShowHidden(!_p->model->hasShowHidden());
        }

        void FileBrowser::seqCallback(QAction * action)
        {
            _p->model->setSequence(
                static_cast<Core::Sequence::COMPRESS>(action->data().toInt()));
        }

        void FileBrowser::seqCallback(int in)
        {
            _p->model->setSequence(static_cast<Core::Sequence::COMPRESS>(in));
        }

        void FileBrowser::searchCallback(const QString & text)
        {
            _p->model->setFilterText(text);
        }

        void FileBrowser::columnsSortCallback(int section, Qt::SortOrder order)
        {
            _p->model->setColumnsSort(static_cast<FileBrowserModel::COLUMNS>(section));
            _p->model->setReverseSort(order);
        }

        void FileBrowser::columnsSortCallback(QAction * action)
        {
            const FileBrowserModel::COLUMNS columnsSort =
                static_cast<FileBrowserModel::COLUMNS>(action->data().toInt());
            if (columnsSort != _p->model->columnsSort())
            {
                _p->model->setColumnsSort(columnsSort);
            }
            else
            {
                _p->model->setReverseSort(!_p->model->hasReverseSort());
            }
        }

        void FileBrowser::columnsSortCallback()
        {
            if (IndexShortcut * shortcut = qobject_cast<IndexShortcut *>(sender()))
            {
                const int index = shortcut->index();
                bool reverseSort =
                    _p->widgets.browser->header()->sortIndicatorOrder();
                if (index == _p->widgets.browser->header()->sortIndicatorSection())
                {
                    reverseSort = !reverseSort;
                }
                _p->model->setColumnsSort(static_cast<FileBrowserModel::COLUMNS>(index));
                _p->model->setReverseSort(reverseSort);
            }
        }

        void FileBrowser::reverseSortCallback(bool value)
        {
            _p->model->setShowHidden(value);
        }

        void FileBrowser::reverseSortCallback()
        {
            _p->model->setShowHidden(!_p->model->hasShowHidden());
        }

        void FileBrowser::sortDirsFirstCallback(bool value)
        {
            _p->model->setSortDirsFirst(value);
        }

        void FileBrowser::sortDirsFirstCallback()
        {
            _p->model->setSortDirsFirst(!_p->model->hasSortDirsFirst());
        }

        void FileBrowser::pinnedCallback(bool pinned)
        {
            _p->pinned = pinned;
        }

        void FileBrowser::addBookmarkCallback()
        {
            _p->context->fileBrowserPrefs()->addBookmark(_p->fileInfo.path());
        }

        void FileBrowser::deleteBookmarkCallback()
        {
            const QStringList & bookmarks = _p->context->fileBrowserPrefs()->bookmarks();
            if (!bookmarks.count())
                return;
            //DJV_DEBUG("FileBrowser::deleteBookmarkCallback");
            //DJV_DEBUG_PRINT("bookmarks = " << bookmarks);
            MultiChoiceDialog dialog(qApp->translate("djv::UI::FileBrowser", "Choose which bookmarks to delete:"), bookmarks);
            if (QDialog::Accepted == dialog.exec())
            {
                QStringList tmp;
                const QVector<bool> & values = dialog.values();
                //DJV_DEBUG_PRINT("values = " << values);
                for (int i = 0; i < values.count(); ++i)
                {
                    if (!values[i])
                    {
                        tmp += bookmarks[i];
                    }
                }
                //DJV_DEBUG_PRINT("bookmarks = " << tmp);
                _p->context->fileBrowserPrefs()->setBookmarks(tmp);
            }
        }

        void FileBrowser::deleteAllBookmarksCallback()
        {
            const QStringList & bookmarks = _p->context->fileBrowserPrefs()->bookmarks();
            if (!bookmarks.count())
                return;
            QuestionDialog dialog(
                qApp->translate("djv::UI::FileBrowser", "Delete all bookmarks?"));
            if (QDialog::Accepted == dialog.exec())
            {
                _p->context->fileBrowserPrefs()->setBookmarks(QStringList());
            }
        }

        void FileBrowser::bookmarksCallback(QAction * action)
        {
            setFileInfo(Core::FileInfoUtil::fixPath(action->data().toString()));
        }

        void FileBrowser::bookmarksCallback()
        {
            if (IndexShortcut * shortcut = qobject_cast<IndexShortcut *>(sender()))
            {
                const QStringList & bookmarks = _p->context->fileBrowserPrefs()->bookmarks();
                setFileInfo(bookmarks[shortcut->index()]);
            }
        }

        void FileBrowser::bookmarkDelCallback(const QVector<int> & list)
        {
            const QStringList & bookmarks = _p->context->fileBrowserPrefs()->bookmarks();
            QStringList tmp;
            for (int i = 0; i < bookmarks.count(); ++i)
            {
                int j = 0;
                for (; j < list.count(); ++j)
                {
                    if (list[j] == i)
                        break;
                }
                if (j < list.count())
                    continue;
                tmp += bookmarks[i];
            }
            _p->context->fileBrowserPrefs()->setBookmarks(tmp);
            menuUpdate();
        }

        void FileBrowser::bookmarkDelAllCallback()
        {
            _p->context->fileBrowserPrefs()->setBookmarks(QStringList());
            menuUpdate();
        }

        void FileBrowser::acceptedCallback()
        {
            //DJV_DEBUG("FileBrowser::acceptedCallback");
            const QString fileName = Core::FileInfoUtil::fixPath(_p->widgets.file->text());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            Core::FileInfo fileInfo(fileName);
            if (_p->model->sequence() != Core::Sequence::COMPRESS_OFF &&
                fileInfo.isSequenceValid())
            {
                fileInfo.setType(Core::FileInfo::SEQUENCE);
            }
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());
            setFileInfo(fileInfo);
            _p->widgets.file->setText(QDir::toNativeSeparators(_p->fileInfo));
            if (fileInfo.type() != Core::FileInfo::DIRECTORY)
            {
                _p->context->fileBrowserPrefs()->addRecent(_p->fileInfo.path());
                if (!_p->pinnable || (_p->pinnable && !_p->pinned))
                {
                    accept();
                }
                Q_EMIT fileInfoChanged(_p->fileInfo);
            }
        }

        void FileBrowser::styleUpdate()
        {
            _p->buttonsLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
            menuUpdate();
        }

        void FileBrowser::modelUpdate()
        {
            //DJV_DEBUG("FileBrowser::modelUpdate");
            //DJV_DEBUG_PRINT("path = " << _p->fileInfo.path());
            setCursor(Qt::WaitCursor);
            _p->model->setThumbnailMode(_p->context->fileBrowserPrefs()->thumbnailMode());
            _p->model->setThumbnailSize(_p->context->fileBrowserPrefs()->thumbnailSize());
            _p->model->setPath(_p->fileInfo.path());
            setCursor(QCursor());
        }

        void FileBrowser::widgetUpdate()
        {
            //DJV_DEBUG("FileBrowser::widgetUpdate");
            //DJV_DEBUG_PRINT("file = " << _p->fileInfo);
            //DJV_DEBUG_PRINT("file seq = " << _p->fileInfo.seq());
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->widgets.file <<
                _p->widgets.seq <<
                _p->widgets.browser->header() <<
                _p->widgets.pinned);
            _p->widgets.file->setText(QDir::toNativeSeparators(_p->fileInfo));
            _p->widgets.seq->setCurrentIndex(_p->model->sequence());
            _p->widgets.browser->header()->setSortIndicator(
                _p->model->columnsSort(),
                static_cast<Qt::SortOrder>(_p->model->hasReverseSort()));
            _p->widgets.pinned->setVisible(_p->pinnable);
            _p->widgets.pinned->setChecked(_p->pinned);
        }

        void FileBrowser::menuUpdate()
        {
            //DJV_DEBUG("FileBrowser::menuUpdate");
            const QVector<Shortcut> & shortcuts = _p->context->fileBrowserPrefs()->shortcuts();

            _p->actions.actions[Actions::UP]->setIcon(_p->context->iconLibrary()->icon("djv/UI/DirUpIcon"));
            _p->actions.actions[Actions::UP]->setShortcut(shortcuts[FileBrowserPrefs::UP].value);

            _p->actions.actions[Actions::PREV]->setIcon(_p->context->iconLibrary()->icon("djv/UI/DirPrevIcon"));
            _p->actions.actions[Actions::PREV]->setShortcut(shortcuts[FileBrowserPrefs::PREV].value);

            _p->menus.menus[Menus::RECENT]->clear();
            const QStringList & recent = _p->context->fileBrowserPrefs()->recent();
            for (int i = 0; i < recent.count(); ++i)
            {
                QAction * action = _p->menus.menus[Menus::RECENT]->addAction(QDir::toNativeSeparators(recent[i]));
                action->setData(recent[i]);
                _p->actions.groups[Actions::RECENT_GROUP]->addAction(action);
            }

            _p->actions.actions[Actions::CURRENT]->setShortcut(shortcuts[FileBrowserPrefs::CURRENT].value);
            _p->actions.actions[Actions::HOME]->setShortcut(shortcuts[FileBrowserPrefs::HOME].value);
            _p->actions.actions[Actions::DESKTOP]->setShortcut(shortcuts[FileBrowserPrefs::DESKTOP].value);

            _p->menus.menus[Menus::DRIVES]->clear();
            const QStringList drives = Core::System::drives();
            for (int i = 0; i < drives.count(); ++i)
            {
                QAction * action = _p->menus.menus[Menus::DRIVES]->addAction(QDir::toNativeSeparators(drives[i]));
                action->setData(drives[i]);
                _p->actions.groups[Actions::DRIVES_GROUP]->addAction(action);
            }

            _p->actions.actions[Actions::RELOAD]->setIcon(_p->context->iconLibrary()->icon("djv/UI/DirReloadIcon"));
            _p->actions.actions[Actions::RELOAD]->setShortcut(shortcuts[FileBrowserPrefs::RELOAD].value);

            _p->menus.menus[Menus::THUMBNAIL_MODE]->clear();
            const QStringList & thumbnailModeLabels = FileBrowserModel::thumbnailModeLabels();
            for (int i = 0; i < thumbnailModeLabels.count(); ++i)
            {
                QAction * action = _p->menus.menus[Menus::THUMBNAIL_MODE]->addAction(thumbnailModeLabels[i]);
                action->setCheckable(true);
                action->setChecked(_p->model->thumbnailMode() == i);
                action->setData(i);
                _p->actions.groups[Actions::THUMBNAIL_MODE_GROUP]->addAction(action);
            }

            _p->menus.menus[Menus::THUMBNAIL_SIZE]->clear();
            const QStringList & thumbnailSizeLabels = FileBrowserModel::thumbnailSizeLabels();
            for (int i = 0; i < thumbnailSizeLabels.count(); ++i)
            {
                QAction * action = _p->menus.menus[Menus::THUMBNAIL_SIZE]->addAction(thumbnailSizeLabels[i]);
                action->setCheckable(true);
                action->setChecked(_p->model->thumbnailSize() == i);
                action->setData(i);
                _p->actions.groups[Actions::THUMBNAIL_SIZE_GROUP]->addAction(action);
            }

            _p->menus.menus[Menus::SEQ]->clear();
            const QStringList & seq = Core::Sequence::compressLabels();
            for (int i = 0; i < seq.count(); ++i)
            {
                QAction * action = _p->menus.menus[Menus::SEQ]->addAction(seq[i]);
                action->setCheckable(true);
                action->setChecked(_p->model->sequence() == i);
                action->setData(i);
                _p->actions.groups[Actions::SEQ_GROUP]->addAction(action);
            }

            _p->actions.actions[Actions::SHOW_HIDDEN]->setChecked(_p->model->hasShowHidden());
            _p->actions.actions[Actions::SHOW_HIDDEN]->setShortcut(shortcuts[FileBrowserPrefs::SHOW_HIDDEN].value);

            QVector<Shortcut> sortShortcuts = QVector<Shortcut>() <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::SORT_NAME] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::SORT_SIZE] <<
#if ! defined(DJV_WINDOWS)
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::SORT_USER] <<
#endif
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::SORT_PERMISSIONS] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::SORT_TIME];
            _p->menus.menus[Menus::SORT]->clear();
            const QStringList & sort = FileBrowserModel::columnsLabels();
            for (int i = 0; i < sort.count(); ++i)
            {
                QAction * action = _p->menus.menus[Menus::SORT]->addAction(sort[i]);
                action->setCheckable(true);
                action->setChecked(_p->widgets.browser->header()->sortIndicatorSection() == i);
                action->setData(i);
                action->setShortcut(sortShortcuts[i].value);
                _p->actions.groups[Actions::SORT_GROUP]->addAction(action);
            }

            _p->actions.actions[Actions::REVERSE_SORT]->setChecked(_p->widgets.browser->header()->sortIndicatorOrder());
            _p->actions.actions[Actions::REVERSE_SORT]->setShortcut(shortcuts[FileBrowserPrefs::REVERSE_SORT].value);

            _p->actions.actions[Actions::SORT_DIRS_FIRST]->setChecked(_p->model->hasSortDirsFirst());
            _p->actions.actions[Actions::SORT_DIRS_FIRST]->setShortcut(shortcuts[FileBrowserPrefs::SORT_DIRS_FIRST].value);

            _p->menus.menus[Menus::BOOKMARKS]->clear();
            QAction * action = _p->menus.menus[Menus::BOOKMARKS]->addAction(
                qApp->translate("djv::UI::FileBrowser", "&Add"),
                this,
                SLOT(addBookmarkCallback()));
            action->setShortcut(shortcuts[FileBrowserPrefs::ADD_BOOKMARK].value);
            action = _p->menus.menus[Menus::BOOKMARKS]->addAction(
                qApp->translate("djv::UI::FileBrowser", "&Delete"),
                this,
                SLOT(deleteBookmarkCallback()));
            const QStringList & bookmarks = _p->context->fileBrowserPrefs()->bookmarks();
            action->setEnabled(bookmarks.size());
            action = _p->menus.menus[Menus::BOOKMARKS]->addAction(
                qApp->translate("djv::UI::FileBrowser", "D&elete All"),
                this,
                SLOT(deleteAllBookmarksCallback()));
            action->setEnabled(bookmarks.size());
            _p->menus.menus[Menus::BOOKMARKS]->addSeparator();
            QVector<Shortcut> bookmarkShortcuts = QVector<Shortcut>() <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_1] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_2] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_3] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_4] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_5] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_6] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_7] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_8] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_9] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_10] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_11] <<
                _p->context->fileBrowserPrefs()->shortcuts()[FileBrowserPrefs::BOOKMARK_12];
            for (int i = 0; i < bookmarks.count(); ++i)
            {
                QAction * action = _p->menus.menus[Menus::BOOKMARKS]->addAction(bookmarks[i]);
                action->setData(bookmarks[i]);
                if (i < bookmarkShortcuts.count())
                    action->setShortcut(bookmarkShortcuts[i].value);
                _p->actions.groups[Actions::BOOKMARKS_GROUP]->addAction(action);
            }
        }

        void FileBrowser::toolTipUpdate()
        {
            //DJV_DEBUG("FileBrowser::toolTipUpdate");
            const QVector<Shortcut> & shortcuts = _p->context->fileBrowserPrefs()->shortcuts();
                
            _p->widgets.file->setToolTip(qApp->translate("djv::UI::FileBrowser", "File name"));
            _p->widgets.up->setToolTip(QString(
                qApp->translate("djv::UI::FileBrowser", "Go up a directory\n\nKeyboard shortcut: %1")).
                arg(shortcuts[FileBrowserPrefs::UP].value.toString()));
            _p->widgets.prev->setToolTip(QString(
                qApp->translate("djv::UI::FileBrowser", "Go to the previous directory\n\nKeyboard shortcut: %1")).
                arg(shortcuts[FileBrowserPrefs::PREV].value.toString()));
            _p->widgets.reload->setToolTip(QString(
                qApp->translate("djv::UI::FileBrowser", "Reload the current directory\n\nKeyboard shortcut: %1")).
                arg(shortcuts[FileBrowserPrefs::RELOAD].value.toString()));
            _p->widgets.seq->setToolTip(QString(
                qApp->translate("djv::UI::FileBrowser",
                "File sequence options:\n"
                "%1 - Show every file\n"
                "%2 - Show sequences of files including gaps between ranges; for example: 1-3,5,7-10\n"
                "%3 - Show sequences of files as a continuous range; for example: 1-10\n")).
                arg(Core::Sequence::compressLabels()[0]).
                arg(Core::Sequence::compressLabels()[1]).
                arg(Core::Sequence::compressLabels()[2]));
        }

        QVector<int> FileBrowser::columnSizes() const
        {
            const QFontMetrics fontMetrics(this->fontMetrics());
            static const int border = 20;
            return QVector<int>() <<
                300 <<
                (fontMetrics.width("000.0000") + border) <<
#if ! defined(DJV_WINDOWS)
                (fontMetrics.width("000000") + border) <<
#endif
                (fontMetrics.averageCharWidth() * 4 + border) <<
                fontMetrics.width("000 000 00 00:00:00 0000");
        }

        bool FileBrowser::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QDialog::event(event);
        }

    } // namespace UI
} // namespace djv
