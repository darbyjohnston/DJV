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
#include <QButtonGroup>
#include <QBoxLayout>
#include <QTreeView>
#include <QMenu>
#include <QPointer>
#include <QSpinBox>
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
            AV::Color color;
            size_t lineWidth = 1;
            bool listVisible = false;
            std::shared_ptr<Annotate::Collection> collection;
            std::shared_ptr<Annotate::Data> currentData;
            std::shared_ptr<Annotate::AbstractPrimitive> currentPrimitive;

            QPointer<QButtonGroup> primitiveButtonGroup;
            QPointer<UI::ToolButton> colorButton;
            QPointer<QSpinBox> lineWidthSpinBox;
            QPointer<UI::ToolButton> undoButton;
            QPointer<UI::ToolButton> redoButton;
            QPointer<UI::ToolButton> clearButton;
            QPointer<QTextEdit> textEdit;
            QPointer<AnnotateModel> model;
            QPointer<QTreeView> treeView;
            QPointer<UI::ToolButton> prevButton;
            QPointer<UI::ToolButton> nextButton;
            QPointer<UI::ToolButton> listVisibleButton;
            QPointer<UI::ToolButton> addButton;
            QPointer<UI::ToolButton> removeButton;
            QPointer<UI::ToolButton> exportButton;

            QPointer<QHBoxLayout> drawingLayout;
            QPointer<QHBoxLayout> controlsLayout;
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
            _p->primitiveButtonGroup = new QButtonGroup(this);
            _p->primitiveButtonGroup->setExclusive(true);
            for (int i = 0; i < Enum::ANNOTATE_PRIMITIVE_COUNT; ++i)
            {
                auto button = new UI::ToolButton(context.data());
                button->setCheckable(true);
                _p->primitiveButtonGroup->addButton(button, i);
            }
            _p->colorButton = new UI::ToolButton(context.data());
            _p->lineWidthSpinBox = new QSpinBox;
            _p->lineWidthSpinBox->setRange(1, 100);
            _p->undoButton = new UI::ToolButton(context.data());
            _p->redoButton = new UI::ToolButton(context.data());
            _p->clearButton = new UI::ToolButton(context.data());

            _p->textEdit = new QTextEdit;

            _p->treeView = new QTreeView;
            _p->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
            _p->treeView->setModel(_p->model);

            _p->prevButton = new UI::ToolButton(context.data());
            _p->nextButton = new UI::ToolButton(context.data());
            _p->listVisibleButton = new UI::ToolButton(context.data());
            _p->listVisibleButton->setCheckable(true);
            _p->addButton = new UI::ToolButton(context.data());
            _p->removeButton = new UI::ToolButton(context.data());
            _p->exportButton = new UI::ToolButton(context.data());

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            _p->drawingLayout = new QHBoxLayout;
            _p->drawingLayout->setMargin(0);
            Q_FOREACH (auto i, _p->primitiveButtonGroup->buttons())
            {
                _p->drawingLayout->addWidget(i);
            }
            _p->drawingLayout->addWidget(_p->colorButton);
            _p->drawingLayout->addWidget(_p->lineWidthSpinBox);
            _p->drawingLayout->addWidget(_p->undoButton);
            _p->drawingLayout->addWidget(_p->redoButton);
            _p->drawingLayout->addWidget(_p->clearButton);
            _p->drawingLayout->addStretch();
            layout->addLayout(_p->drawingLayout);
            layout->addWidget(_p->textEdit);
            layout->addWidget(_p->treeView, 1);
            _p->controlsLayout = new QHBoxLayout;
            _p->controlsLayout->setMargin(0);
            _p->controlsLayout->addWidget(_p->prevButton);
            _p->controlsLayout->addWidget(_p->nextButton);
            _p->controlsLayout->addWidget(_p->addButton);
            _p->controlsLayout->addWidget(_p->removeButton);
            _p->controlsLayout->addWidget(_p->listVisibleButton);
            _p->controlsLayout->addStretch();
            _p->controlsLayout->addWidget(_p->exportButton);
            layout->addLayout(_p->controlsLayout);

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::AnnotateTool", "Annotate"));
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            _p->collection = Annotate::Collection::create(FileInfo());
            styleUpdate();
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->primitiveButtonGroup,
                SIGNAL(buttonToggled(int, bool)),
                SLOT(primitiveCallback(int, bool)));
            connect(
                _p->colorButton,
                &UI::ToolButton::clicked,
                [this]
            {

            });
            connect(
                _p->lineWidthSpinBox,
                SIGNAL(valueChanged(int)),
                SLOT(lineWidthCallback(int)));
            connect(
                _p->undoButton,
                &UI::ToolButton::clicked,
                [this]
            {
            });
            connect(
                _p->redoButton,
                &UI::ToolButton::clicked,
                [this]
            {
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
                _p->treeView->selectionModel(),
                &QItemSelectionModel::currentChanged,
                [this, session](const QModelIndex & current, const QModelIndex &)
            {
                if (auto p = current.internalPointer())
                {
                    _p->currentData = *reinterpret_cast<std::shared_ptr<Annotate::Data> *>(p);
                    _p->textEdit->setText(_p->currentData->text());
                    auto playbackGroup = session->playbackGroup();
                    playbackGroup->setFrame(_p->currentData->frame());
                    viewUpdate();
                }
                else
                {
                    _p->currentData.reset();
                    _p->textEdit->clear();
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
                        auto playbackGroup = session->playbackGroup();
                        playbackGroup->setFrame(_p->currentData->frame());
                    }
                    else
                    {
                        _p->textEdit->clear();
                    }
                }
            });

            connect(
                _p->listVisibleButton,
                &UI::ToolButton::toggled,
                [this](bool value)
            {
                _p->listVisible = value;
                widgetUpdate();
            });

            connect(
                _p->exportButton,
                &UI::ToolButton::clicked,
                [this]
            {
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
                SIGNAL(colorChanged(const djv::AV::Color &)),
                SLOT(setColor(const djv::AV::Color &)));
            connect(
                context->annotatePrefs(),
                SIGNAL(lineWidthChanged(size_t)),
                SLOT(setLineWidth(size_t)));

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
                }
                else
                {
                    _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
                    _p->textEdit->clear();
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

        void AnnotateTool::setColor(const AV::Color & value)
        {
            if (value == _p->color)
                return;
            _p->color = value;
            context()->annotatePrefs()->setColor(_p->color);
            widgetUpdate();
            Q_EMIT colorChanged(_p->color);
        }

        void AnnotateTool::setLineWidth(size_t value)
        {
            if (value == _p->lineWidth)
                return;
            _p->lineWidth = value;
            context()->annotatePrefs()->setLineWidth(_p->lineWidth);
            widgetUpdate();
            Q_EMIT lineWidthChanged(_p->lineWidth);
        }

        void AnnotateTool::primitiveCallback(int id, bool checked)
        {
            if (checked)
            {
                setPrimitive(static_cast<Enum::ANNOTATE_PRIMITIVE>(id));
            }
        }

        void AnnotateTool::lineWidthCallback(int value)
        {
            setLineWidth(value);
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
                "djv/UI/AnnotateIcon",
                "djv/UI/AnnotateRectangleIcon",
                "djv/UI/AnnotateEllipseIcon"
            });
        
        } // namespace

        void AnnotateTool::styleUpdate()
        {
            size_t j = 0;
            Q_FOREACH(auto i, _p->primitiveButtonGroup->buttons())
            {
                i->setIcon(context()->iconLibrary()->icon(primitiveIcons[j].c_str()));
                ++j;
            }

            const int size = style()->pixelMetric(QStyle::PM_ButtonIconSize);
            QImage image(size, size, QImage::Format_ARGB32);
            image.fill(AV::ColorUtil::toQt(_p->color));
            _p->colorButton->setIcon(QPixmap::fromImage(image));

            _p->undoButton->setIcon(context()->iconLibrary()->icon("djv/UI/UndoIcon"));
            _p->redoButton->setIcon(context()->iconLibrary()->icon("djv/UI/RedoIcon"));
            _p->clearButton->setIcon(context()->iconLibrary()->icon("djv/UI/EraseIcon"));

            _p->prevButton->setIcon(context()->iconLibrary()->icon("djv/UI/LeftIcon"));
            _p->nextButton->setIcon(context()->iconLibrary()->icon("djv/UI/RightIcon"));
            _p->removeButton->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));
            _p->addButton->setIcon(context()->iconLibrary()->icon("djv/UI/AddIcon"));
            _p->listVisibleButton->setIcon(context()->iconLibrary()->icon("djv/UI/ListIcon"));
            _p->exportButton->setIcon(context()->iconLibrary()->icon("djv/UI/ExportIcon"));

            _p->drawingLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
            _p->controlsLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }

        void AnnotateTool::widgetUpdate()
        {
            SignalBlocker signalBlocker(QObjectList() <<
                _p->primitiveButtonGroup <<
                _p->lineWidthSpinBox <<
                _p->listVisibleButton);
            _p->primitiveButtonGroup->buttons()[_p->primitive]->setChecked(true);
            _p->lineWidthSpinBox->setValue(static_cast<int>(_p->lineWidth));
            _p->treeView->setVisible(_p->listVisible);
            _p->listVisibleButton->setChecked(_p->listVisible);
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
