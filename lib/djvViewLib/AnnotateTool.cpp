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

#include <djvViewLib/AnnotateTool.h>

#include <djvViewLib/AnnotateData.h>
#include <djvViewLib/AnnotateModel.h>
#include <djvViewLib/AnnotatePrefs.h>
#include <djvViewLib/FileGroup.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/ToolButton.h>

#include <djvAV/Color.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QBoxLayout>
#include <QTreeView>
#include <QMenu>
#include <QPointer>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct AnnotateTool::Private
        {
            Private(const QPointer<ViewContext> & context) :
                primitive(context->annotatePrefs()->primitive()),
                color(context->annotatePrefs()->color()),
                lineWidth(context->annotatePrefs()->lineWidth())
            {}

            Enum::ANNOTATE_PRIMITIVE primitive;
            Enum::ANNOTATE_COLOR color;
            Enum::ANNOTATE_LINE_WIDTH lineWidth;
            std::shared_ptr<Annotate::AbstractPrimitive> currentPrimitive;
            std::shared_ptr<Annotate::Collection> collection;
            std::shared_ptr<Annotate::Data> currentData;

            QPointer<QTabWidget> tabWidget;
            QPointer<AnnotateModel> model;
            QPointer<QTreeView> treeView;
            QPointer<UI::ToolButton> prevButton;
            QPointer<UI::ToolButton> nextButton;
            QPointer<UI::ToolButton> addButton;
            QPointer<UI::ToolButton> removeButton;
            QPointer<QTextEdit> textEdit;
            QPointer<QActionGroup> primitiveActionGroup;
            QPointer<QMenu> primitiveMenu;
            QPointer<UI::ToolButton> primitiveButton;
            QPointer<QActionGroup> colorActionGroup;
            QPointer<QMenu> colorMenu;
            QPointer<UI::ToolButton> colorButton;
            std::vector<QIcon> colorIcons;
            QPointer<QActionGroup> lineWidthActionGroup;
            QPointer<QMenu> lineWidthMenu;
            QPointer<UI::ToolButton> lineWidthButton;
            QPointer<UI::ToolButton> clearButton;
            QPointer<QHBoxLayout> prevNextLayout;
            QPointer<QHBoxLayout> addRemoveLayout;
            QPointer<QHBoxLayout> drawLayout;
            QPointer<QSplitter> splitter;
        };

        AnnotateTool::AnnotateTool(
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            AbstractTool(session, context, parent),
            _p(new Private(context))
        {
            // Create the models.
            _p->model = new AnnotateModel;

            // Create the widgets.
            _p->treeView = new QTreeView;
            _p->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
            _p->treeView->setModel(_p->model);

            _p->prevButton = new UI::ToolButton(context.data());
            _p->nextButton = new UI::ToolButton(context.data());
            _p->addButton = new UI::ToolButton(context.data());
            _p->removeButton = new UI::ToolButton(context.data());

            _p->textEdit = new QTextEdit;

            _p->primitiveActionGroup = new QActionGroup(this);
            _p->primitiveActionGroup->setExclusive(true);
            _p->primitiveMenu = new QMenu;
            for (int i = 0; i < Enum::ANNOTATE_PRIMITIVE_COUNT; ++i)
            {
                auto action = new QAction(this);
                action->setText(Enum::annotatePrimitiveLabels()[i]);
                action->setCheckable(true);
                _p->primitiveActionGroup->addAction(action);
                _p->primitiveMenu->addAction(action);
            }
            _p->primitiveButton = new UI::ToolButton(context.data());

            _p->colorActionGroup = new QActionGroup(this);
            _p->colorActionGroup->setExclusive(true);
            _p->colorMenu = new QMenu;
            for (int i = 0; i < Enum::ANNOTATE_COLOR_COUNT; ++i)
            {
                auto action = new QAction(this);
                action->setText(Enum::annotateColorLabels()[i]);
                action->setCheckable(true);
                _p->colorActionGroup->addAction(action);
                _p->colorMenu->addAction(action);
            }
            _p->colorButton = new UI::ToolButton(context.data());

            _p->lineWidthActionGroup = new QActionGroup(this);
            _p->lineWidthActionGroup->setExclusive(true);
            _p->lineWidthMenu = new QMenu;
            for (int i = 0; i < Enum::ANNOTATE_LINE_WIDTH_COUNT; ++i)
            {
                auto action = new QAction(this);
                action->setText(Enum::annotateLineWidthLabels()[i]);
                action->setCheckable(true);
                _p->lineWidthActionGroup->addAction(action);
                _p->lineWidthMenu->addAction(action);
            }
            _p->lineWidthButton = new UI::ToolButton(context.data());

            _p->clearButton = new UI::ToolButton(context.data());

            // Layout the widgets.
            auto annotationsTab = new QWidget;
            auto vLayout = new QVBoxLayout(annotationsTab);
            _p->splitter = new QSplitter(Qt::Vertical);
            vLayout->addWidget(_p->splitter);

            auto listWidget = new QWidget;
            vLayout = new QVBoxLayout(listWidget);
            vLayout->setMargin(0);
            vLayout->addWidget(_p->treeView);
            auto hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            _p->prevNextLayout = new QHBoxLayout;
            _p->prevNextLayout->setMargin(0);
            _p->prevNextLayout->addWidget(_p->prevButton);
            _p->prevNextLayout->addWidget(_p->nextButton);
            hLayout->addLayout(_p->prevNextLayout);
            hLayout->addStretch();
            _p->addRemoveLayout = new QHBoxLayout;
            _p->addRemoveLayout->setMargin(0);
            _p->addRemoveLayout->addWidget(_p->addButton);
            _p->addRemoveLayout->addWidget(_p->removeButton);
            hLayout->addLayout(_p->addRemoveLayout);
            vLayout->addLayout(hLayout);
            _p->splitter->addWidget(listWidget);

            auto editWidget = new QWidget;
            vLayout = new QVBoxLayout(editWidget);
            vLayout->setMargin(0);
            vLayout->addWidget(_p->textEdit);
            hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            _p->drawLayout = new QHBoxLayout;
            _p->drawLayout->setMargin(0);
            _p->drawLayout->addWidget(_p->primitiveButton);
            _p->drawLayout->addWidget(_p->colorButton);
            _p->drawLayout->addWidget(_p->lineWidthButton);
            _p->drawLayout->addWidget(_p->clearButton);
            hLayout->addLayout(_p->drawLayout);
            hLayout->addStretch();
            vLayout->addLayout(hLayout);
            _p->splitter->addWidget(editWidget);

            _p->tabWidget = new QTabWidget;
            _p->tabWidget->addTab(annotationsTab, "Annotations");
            _p->tabWidget->addTab(new QWidget, "Summary");
            _p->tabWidget->addTab(new QWidget, "Export");

            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->tabWidget);

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::AnnotateTool", "Annotate"));
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            _p->collection = Annotate::Collection::create(FileInfo());
            styleUpdate();
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->primitiveActionGroup,
                SIGNAL(triggered(QAction *)),
                SLOT(primitiveCallback(QAction *)));
            connect(
                _p->colorActionGroup,
                SIGNAL(triggered(QAction *)),
                SLOT(colorCallback(QAction *)));
            connect(
                _p->lineWidthActionGroup,
                SIGNAL(triggered(QAction *)),
                SLOT(lineWidthCallback(QAction *)));

            connect(
                _p->treeView->selectionModel(),
                &QItemSelectionModel::currentChanged,
                [this, session](const QModelIndex & current, const QModelIndex &)
            {
                if (auto p = current.internalPointer())
                {
                    _p->currentData = *reinterpret_cast<std::shared_ptr<Annotate::Data> *>(p);
                    _p->textEdit->setText(_p->currentData->text());
                    _p->textEdit->setEnabled(true);
                    auto playbackGroup = session->playbackGroup();
                    playbackGroup->setFrame(_p->currentData->frame());
                    viewUpdate();
                }
                else
                {
                    _p->currentData.reset();
                    _p->textEdit->clear();
                    _p->textEdit->setEnabled(false);
                }
            });

            connect(
                _p->prevButton,
                &UI::ToolButton::clicked,
                [this, session]
            {
                const auto & data = _p->collection->data();
                if (_p->collection && data.size() > 1)
                {
                    size_t row = 0;
                    if (_p->currentData)
                    {
                        row = _p->collection->index(_p->currentData);
                        if (row > 0)
                        {
                            --row;
                        }
                        else
                        {
                            row = data.size() - 1;
                        }
                    }
                    _p->currentData = data[row];
                    _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                    _p->treeView->selectionModel()->select(
                        QItemSelection(_p->model->index(static_cast<int>(row), 0), _p->model->index(static_cast<int>(row), 1)),
                        QItemSelectionModel::Select);
                    _p->textEdit->setText(_p->currentData->text());
                    _p->textEdit->setEnabled(true);
                    auto playbackGroup = session->playbackGroup();
                    playbackGroup->setFrame(_p->currentData->frame());
                }
            });

            connect(
                _p->nextButton,
                &UI::ToolButton::clicked,
                [this, session]
            {
                const auto & data = _p->collection->data();
                if (_p->collection && data.size() > 1)
                {
                    size_t row = 0;
                    if (_p->currentData)
                    {
                        row = _p->collection->index(_p->currentData);
                        if (row < data.size() - 1)
                        {
                            ++row;
                        }
                        else
                        {
                            row = 0;
                        }
                    }
                    _p->currentData = data[row];
                    _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                    _p->treeView->selectionModel()->select(
                        QItemSelection(_p->model->index(static_cast<int>(row), 0), _p->model->index(static_cast<int>(row), 1)),
                        QItemSelectionModel::Select);
                    _p->textEdit->setText(_p->currentData->text());
                    _p->textEdit->setEnabled(true);
                    auto playbackGroup = session->playbackGroup();
                    playbackGroup->setFrame(_p->currentData->frame());
                }
            });

            connect(
                _p->addButton,
                &UI::ToolButton::clicked,
                [this, session]
            {
                if (_p->collection)
                {
                    auto playbackGroup = session->playbackGroup();
                    const qint64 frame = playbackGroup->frame();
                    auto data = Annotate::Data::create(frame);
                    const size_t row = _p->collection->addData(data);
                    _p->currentData = data;
                    annotationsUpdate();
                    _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                    _p->treeView->selectionModel()->select(
                        QItemSelection(_p->model->index(static_cast<int>(row), 0), _p->model->index(static_cast<int>(row), 1)),
                        QItemSelectionModel::Select);
                    _p->textEdit->setText(data->text());
                    _p->textEdit->setEnabled(true);
                }
            });

            connect(
                _p->removeButton,
                &UI::ToolButton::clicked,
                [this, session]
            {
                if (_p->collection && _p->currentData)
                {
                    int selection = -1;
                    const auto & indexes = _p->treeView->selectionModel()->selection().indexes();
                    if (indexes.size())
                    {
                        selection = indexes[0].row();
                    }
                    _p->collection->removeData(_p->currentData);
                    _p->currentData.reset();
                    const auto & data = _p->collection->data();
                    selection = Math::min(selection, static_cast<int>(data.size()) - 1);
                    annotationsUpdate();
                    viewUpdate();
                    if (selection >= 0)
                    {
                        _p->currentData = data[selection];
                        _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                        _p->treeView->selectionModel()->select(
                            QItemSelection(_p->model->index(selection, 0), _p->model->index(selection, 1)),
                            QItemSelectionModel::Select);
                        _p->textEdit->setText(_p->currentData->text());
                        _p->textEdit->setEnabled(true);
                        auto playbackGroup = session->playbackGroup();
                        playbackGroup->setFrame(_p->currentData->frame());
                    }
                    else
                    {
                        _p->textEdit->clear();
                        _p->textEdit->setEnabled(false);
                    }
                }
            });

            connect(
                _p->textEdit,
                &QTextEdit::textChanged,
                [this]
            {
                if (_p->currentData)
                {
                    _p->currentData->setText(_p->textEdit->toPlainText());
                    int selection = -1;
                    const auto & indexes = _p->treeView->selectionModel()->selection().indexes();
                    if (indexes.size())
                    {
                        selection = indexes[0].row();
                    }
                    annotationsUpdate();
                    if (selection != -1)
                    {
                        _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                        _p->treeView->selectionModel()->select(
                            QItemSelection(_p->model->index(selection, 0), _p->model->index(selection, 1)),
                            QItemSelectionModel::Select);
                    }
                }
            });

            connect(
                _p->primitiveButton,
                &UI::ToolButton::pressed,
                [this]
            {
                _p->primitiveMenu->exec(_p->primitiveButton->mapToGlobal(QPoint(0, 0)), _p->primitiveActionGroup->checkedAction());
            });
            connect(
                _p->colorButton,
                &UI::ToolButton::pressed,
                [this]
            {
                _p->colorMenu->exec(_p->colorButton->mapToGlobal(QPoint(0, 0)), _p->colorActionGroup->checkedAction());
            });
            connect(
                _p->lineWidthButton,
                &UI::ToolButton::pressed,
                [this]
            {
                _p->lineWidthMenu->exec(_p->lineWidthButton->mapToGlobal(QPoint(0, 0)), _p->lineWidthActionGroup->checkedAction());
            });

            connect(
                _p->clearButton,
                &UI::ToolButton::clicked,
                [this]
            {
                if (_p->currentData)
                {
                    _p->currentData->clearPrimitives();
                    viewUpdate();
                }
            });

            auto viewWidget = session->viewWidget();
            connect(
                viewWidget,
                SIGNAL(pickPressed(const glm::ivec2 &)),
                SLOT(pickPressedCallback(const glm::ivec2 &)));
            connect(
                viewWidget,
                SIGNAL(pickReleased(const glm::ivec2 &)),
                SLOT(pickReleasedCallback(const glm::ivec2 &)));
            connect(
                viewWidget,
                SIGNAL(pickMoved(const glm::ivec2 &)),
                SLOT(pickMovedCallback(const glm::ivec2 &)));

            connect(
                context->annotatePrefs(),
                SIGNAL(primitiveChanged(djv::ViewLib::Enum::ANNOTATE_PRIMITIVE)),
                SLOT(setPrimitive(djv::ViewLib::Enum::ANNOTATE_PRIMITIVE)));
            connect(
                context->annotatePrefs(),
                SIGNAL(colorChanged(djv::ViewLib::Enum::ANNOTATE_COLOR)),
                SLOT(setColor(djv::ViewLib::Enum::ANNOTATE_COLOR)));
            connect(
                context->annotatePrefs(),
                SIGNAL(lineWidthChanged(djv::ViewLib::Enum::ANNOTATE_LINE_WIDTH)),
                SLOT(setLineWidth(djv::ViewLib::Enum::ANNOTATE_LINE_WIDTH)));

            auto fileGroup = session->fileGroup();
            connect(
                fileGroup,
                &FileGroup::fileInfoChanged,
                [this](const FileInfo & value)
            {
                _p->collection = Annotate::Collection::create(value);
                _p->currentData.reset();
                _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                _p->textEdit->clear();
                _p->textEdit->setEnabled(false);
                annotationsUpdate();
                viewUpdate();
            });
            auto playbackGroup = session->playbackGroup();
            connect(
                playbackGroup,
                &PlaybackGroup::frameChanged,
                [this](qint64 value)
            {
                _p->currentData.reset();
                int row = 0;
                if (_p->collection)
                {
                    int j = 0;
                    for (const auto & i : _p->collection->data())
                    {
                        if (i->frame() == value)
                        {
                            _p->currentData = i;
                            row = j;
                        }
                        ++j;
                    }
                }
                if (_p->currentData)
                {
                    _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                    _p->treeView->selectionModel()->select(
                        QItemSelection(_p->model->index(row, 0), _p->model->index(row, 1)),
                        QItemSelectionModel::Select);
                    _p->textEdit->setText(_p->currentData->text());
                    _p->textEdit->setEnabled(true);
                }
                else
                {
                    _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                    _p->textEdit->clear();
                    _p->textEdit->setEnabled(false);
                }
                viewUpdate();
            });
        }

        AnnotateTool::~AnnotateTool()
        {}

        void AnnotateTool::setPrimitive(djv::ViewLib::Enum::ANNOTATE_PRIMITIVE value)
        {
            if (value == _p->primitive)
                return;
            _p->primitive = value;
            context()->annotatePrefs()->setPrimitive(_p->primitive);
            widgetUpdate();
            Q_EMIT primitiveChanged(_p->primitive);
        }

        void AnnotateTool::setColor(djv::ViewLib::Enum::ANNOTATE_COLOR value)
        {
            if (value == _p->color)
                return;
            _p->color = value;
            context()->annotatePrefs()->setColor(_p->color);
            widgetUpdate();
            Q_EMIT colorChanged(_p->color);
        }

        void AnnotateTool::setLineWidth(Enum::ANNOTATE_LINE_WIDTH value)
        {
            if (value == _p->lineWidth)
                return;
            _p->lineWidth = value;
            context()->annotatePrefs()->setLineWidth(_p->lineWidth);
            widgetUpdate();
            Q_EMIT lineWidthChanged(_p->lineWidth);
        }

        void AnnotateTool::primitiveCallback(QAction * value)
        {
            setPrimitive(static_cast<Enum::ANNOTATE_PRIMITIVE>(_p->primitiveActionGroup->actions().indexOf(value)));
        }

        void AnnotateTool::colorCallback(QAction * value)
        {
            setColor(static_cast<Enum::ANNOTATE_COLOR>(_p->colorActionGroup->actions().indexOf(value)));
        }

        void AnnotateTool::lineWidthCallback(QAction * value)
        {
            setLineWidth(static_cast<Enum::ANNOTATE_LINE_WIDTH>(_p->lineWidthActionGroup->actions().indexOf(value)));
        }

        void AnnotateTool::pickPressedCallback(const glm::ivec2 & in)
        {
            if (isVisible())
            {
                auto data = _p->currentData;
                if (!data && _p->collection)
                {
                    auto playbackGroup = session()->playbackGroup();
                    playbackGroup->setPlayback(Enum::STOP);
                    const qint64 frame = playbackGroup->frame();
                    data = Annotate::Data::create(frame);
                    const size_t row = _p->collection->addData(data);
                    _p->currentData = data;
                    annotationsUpdate();
                    _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                    _p->treeView->selectionModel()->select(
                        QItemSelection(_p->model->index(static_cast<int>(row), 0), _p->model->index(static_cast<int>(row), 1)),
                        QItemSelectionModel::Select);
                    _p->textEdit->setText(data->text());
                    _p->textEdit->setEnabled(true);
                }
                if (data)
                {
                    _p->currentPrimitive = Annotate::PrimitiveFactory::create(_p->primitive, _p->color, _p->lineWidth);
                    data->addPrimitive(_p->currentPrimitive);
                    viewUpdate();
                }
            }
        }

        void AnnotateTool::pickReleasedCallback(const glm::ivec2 & in)
        {
            _p->currentPrimitive.reset();
        }

        void AnnotateTool::pickMovedCallback(const glm::ivec2 & in)
        {
            if (isVisible() && _p->currentPrimitive)
            {
                const auto view = session()->viewWidget();
                const glm::ivec2 & viewPos = view->viewPos();
                const float viewZoom = view->viewZoom();
                _p->currentPrimitive->mouse(glm::ivec2(
                    (in.x - viewPos.x) / viewZoom,
                    (in.y - viewPos.y) / viewZoom));
                session()->viewWidget()->update();
            }
        }

        namespace
        {
            const auto primitiveIcons = std::vector<std::string>(
            {
                "djv/UI/PrimitivePolyline",
                "djv/UI/PrimitiveRect",
                "djv/UI/PrimitiveEllipse"
            });
        
        } // namespace

        void AnnotateTool::styleUpdate()
        {
            _p->prevButton->setIcon(context()->iconLibrary()->icon("djv/UI/LeftIcon"));
            _p->nextButton->setIcon(context()->iconLibrary()->icon("djv/UI/RightIcon"));
            _p->removeButton->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));
            _p->addButton->setIcon(context()->iconLibrary()->icon("djv/UI/AddIcon"));
            for (int i = 0; i < Enum::ANNOTATE_PRIMITIVE_COUNT; ++i)
            {
                _p->primitiveActionGroup->actions()[i]->setIcon(context()->iconLibrary()->icon(primitiveIcons[i].c_str()));
            }
            _p->colorIcons.clear();
            for (int i = 0; i < Enum::ANNOTATE_COLOR_COUNT; ++i)
            {
                const int size = style()->pixelMetric(QStyle::PM_ButtonIconSize);
                QImage image(size, size, QImage::Format_ARGB32);
                image.fill(AV::ColorUtil::toQt(Enum::annotateColors()[i]));
                QIcon icon(QPixmap::fromImage(image));
                _p->colorIcons.push_back(icon);
                _p->colorActionGroup->actions()[i]->setIcon(icon);
            }
            _p->clearButton->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));

            _p->prevNextLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
            _p->addRemoveLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
            _p->drawLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }

        void AnnotateTool::widgetUpdate()
        {
            SignalBlocker signalBlocker(QObjectList() <<
                _p->primitiveActionGroup <<
                _p->colorActionGroup <<
                _p->lineWidthActionGroup);

            _p->primitiveActionGroup->actions()[_p->primitive]->setChecked(true);
            _p->colorActionGroup->actions()[_p->color]->setChecked(true);
            _p->lineWidthActionGroup->actions()[_p->lineWidth]->setChecked(true);

            _p->primitiveButton->setIcon(context()->iconLibrary()->icon(primitiveIcons[_p->primitive].c_str()));
            _p->colorButton->setIcon(_p->colorIcons[_p->color]);
            _p->lineWidthButton->setText(Enum::annotateLineWidthLabels()[_p->lineWidth]);
        }

        void AnnotateTool::annotationsUpdate()
        {
            _p->model->setCollection(_p->collection);
        }

        void AnnotateTool::viewUpdate()
        {
            std::vector<std::shared_ptr<Annotate::Data> > data;
            if (_p->collection)
            {
                const qint64 frame = session()->playbackGroup()->frame();
                for (const auto & i : _p->collection->data())
                {
                    if (frame == i->frame())
                    {
                        data.push_back(i);
                    }
                }
            }
            session()->viewWidget()->setAnnotateData(data);
        }

    } // namespace ViewLib
} // namespace djv
