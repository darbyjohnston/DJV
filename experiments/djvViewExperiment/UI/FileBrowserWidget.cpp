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

#include <UI/FileBrowserPrivate.h>

#include <UI/Context.h>

#include <Core/FileUtil.h>

#include <QAction>
#include <QButtonGroup>
#include <QComboBox>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPointer>
#include <QStackedWidget>
#include <QStyle>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include <iostream>

namespace djv
{
    namespace UI
    {
        struct PathWidget::Private
        {
            QString path;
            std::vector<QPointer<QAction> > actions;
            QPointer<QToolBar> toolBar;
        };

        PathWidget::PathWidget(const QPointer<Context> &, QWidget * parent) :
            QFrame(parent),
            _p(new Private)
        {
            setFrameShape(QFrame::StyledPanel);
            setBackgroundRole(QPalette::Base);
            setAutoFillBackground(true);

            _p->toolBar = new QToolBar;

            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->toolBar);

            _widgetUpdate();
        }

        PathWidget::~PathWidget()
        {}

        const QString & PathWidget::getPath() const
        {
            return _p->path;
        }

        void PathWidget::setPath(const QString & value)
        {
            if (value == _p->path)
                return;
            _p->path = value;
            _widgetUpdate();
            Q_EMIT pathChanged(_p->path);
        }

        void PathWidget::_widgetUpdate()
        {
            _p->toolBar->clear();
            for (auto i : _p->actions)
            {
                delete i.data();
            }
            _p->actions.clear();
            const auto splitPath = Core::FileUtil::splitPath(_p->path);
            QStringList subPath;
            for (int i = 0; i < splitPath.size(); ++i)
            {
                auto action = new QAction(splitPath[i], this);
                _p->actions.push_back(action);
                _p->toolBar->addAction(action);
                subPath.push_back(splitPath[i]);
                connect(
                    action,
                    &QAction::triggered,
                    [this, subPath]
                {
                    Q_EMIT pathChanged(Core::FileUtil::joinPath(subPath));
                });
            }
        }

        struct FileBrowserHeader::Private
        {
            QString path;
            bool pathEdit = false;
            QPointer<PathWidget> pathWidget;
            QPointer<QLineEdit> pathLineEdit;
            QPointer<QStackedWidget> pathStackedWidget;
            QPointer<QToolButton> upButton;
            QPointer<QToolButton> backButton;
            QPointer<QToolButton> forwardButton;
            QPointer<QToolButton> pathEditButton;
        };

        FileBrowserHeader::FileBrowserHeader(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            _p->pathWidget = new PathWidget(context);
            _p->pathLineEdit = new QLineEdit;
            _p->pathStackedWidget = new QStackedWidget;
            _p->pathStackedWidget->addWidget(_p->pathWidget);
            _p->pathStackedWidget->addWidget(_p->pathLineEdit);
            _p->pathStackedWidget->setCurrentWidget(_p->pathWidget);

            _p->upButton = new QToolButton;
            _p->upButton->setAutoRaise(true);
            _p->upButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
            _p->backButton = new QToolButton;
            _p->backButton->setAutoRaise(true);
            _p->backButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
            _p->forwardButton = new QToolButton;
            _p->forwardButton->setAutoRaise(true);
            _p->forwardButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));

            _p->pathEditButton = new QToolButton;
            _p->pathEditButton->setText("Edit");
            _p->pathEditButton->setCheckable(true);
            _p->pathEditButton->setAutoRaise(true);

            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->pathStackedWidget);
            auto hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            auto hLayout2 = new QHBoxLayout;
            hLayout2->setMargin(0);
            hLayout2->setSpacing(0);
            hLayout2->addWidget(_p->upButton);
            hLayout2->addWidget(_p->backButton);
            hLayout2->addWidget(_p->forwardButton);
            hLayout->addLayout(hLayout2);
            hLayout->addStretch(1);
            hLayout->addWidget(_p->pathEditButton);
            layout->addLayout(hLayout);

            _widgetUpdate();

            connect(
                _p->pathWidget,
                &PathWidget::pathChanged,
                [this](const QString & value)
            {
                Q_EMIT pathChanged(value);
            });
            connect(
                _p->pathLineEdit,
                &QLineEdit::editingFinished,
                [this]
            {
                Q_EMIT pathChanged(_p->pathLineEdit->text());
            });

            connect(
                _p->upButton,
                SIGNAL(clicked()),
                SIGNAL(up()));
            connect(
                _p->backButton,
                SIGNAL(clicked()),
                SIGNAL(back()));
            connect(
                _p->forwardButton,
                SIGNAL(clicked()),
                SIGNAL(forward()));

            connect(
                _p->pathEditButton,
                &QToolButton::toggled,
                [this](bool value)
            {
                _p->pathEdit = value;
                if (value)
                {
                    _p->pathLineEdit->setFocus();
                }
                _widgetUpdate();
            });
        }

        FileBrowserHeader::~FileBrowserHeader()
        {}

        const QString & FileBrowserHeader::getPath() const
        {
            return _p->path;
        }

        void FileBrowserHeader::setPath(const QString & value)
        {
            if (value == _p->path)
                return;
            _p->path = value;
            _widgetUpdate();
            Q_EMIT pathChanged(_p->path);
        }

        void FileBrowserHeader::setUpEnabled(bool value)
        {
            _p->upButton->setEnabled(value);
        }

        void FileBrowserHeader::setBackEnabled(bool value)
        {
            _p->backButton->setEnabled(value);
        }

        void FileBrowserHeader::setForwardEnabled(bool value)
        {
            _p->forwardButton->setEnabled(value);
        }

        void FileBrowserHeader::_widgetUpdate()
        {
            _p->pathWidget->setPath(_p->path);
            _p->pathLineEdit->setText(_p->path);
            if (_p->pathEdit)
            {
                _p->pathStackedWidget->setCurrentWidget(_p->pathLineEdit);
            }
            else
            {
                _p->pathStackedWidget->setCurrentWidget(_p->pathWidget);
            }
        }

        struct FileBrowserFooter::Private
        {
            QPointer<QButtonGroup> viewModeButtonGroup;
            QMap<int, QListView::ViewMode> buttonToViewMode;
        };

        FileBrowserFooter::FileBrowserFooter(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            auto filterLineEdit = new QLineEdit;
            filterLineEdit->setClearButtonEnabled(true);

            auto listModeButton = new QToolButton;
            listModeButton->setCheckable(true);
            listModeButton->setAutoRaise(true);
            listModeButton->setText("List");
            auto iconModeButton = new QToolButton;
            iconModeButton->setCheckable(true);
            iconModeButton->setAutoRaise(true);
            iconModeButton->setText("Icon");
            _p->viewModeButtonGroup = new QButtonGroup(this);
            _p->viewModeButtonGroup->setExclusive(true);
            _p->viewModeButtonGroup->addButton(listModeButton);
            _p->viewModeButtonGroup->addButton(iconModeButton);
            _p->buttonToViewMode[_p->viewModeButtonGroup->id(listModeButton)] = QListView::ViewMode::ListMode;
            _p->buttonToViewMode[_p->viewModeButtonGroup->id(iconModeButton)] = QListView::ViewMode::IconMode;

            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            auto hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(filterLineEdit);
            auto hLayout2 = new QHBoxLayout;
            hLayout2->setMargin(0);
            hLayout2->setSpacing(0);
            hLayout2->addWidget(listModeButton);
            hLayout2->addWidget(iconModeButton);
            hLayout->addLayout(hLayout2);
            layout->addLayout(hLayout);

            listModeButton->setChecked(true);

            connect(
                filterLineEdit,
                SIGNAL(textChanged(const QString &)),
                SIGNAL(filterChanged(const QString &)));
            connect(
                _p->viewModeButtonGroup,
                SIGNAL(buttonClicked(int)),
                SLOT(_viewModeCallback(int)));
        }

        FileBrowserFooter::~FileBrowserFooter()
        {}

        void FileBrowserFooter::setViewMode(QListView::ViewMode value)
        {
            _p->viewModeButtonGroup->buttons()[value]->setChecked(true);
        }

        void FileBrowserFooter::_viewModeCallback(int value)
        {
            Q_EMIT viewModeChanged(_p->buttonToViewMode[value]);
        }

    } // namespace UI
} // namespace djv
