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

#include <djvViewLib/AnnotateActions.h>
#include <djvViewLib/AnnotateData.h>
#include <djvViewLib/AnnotateGroup.h>
#include <djvViewLib/AnnotateModel.h>
#include <djvViewLib/AnnotatePrefs.h>
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

#undef DELETE

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
                lineWidth(context->annotatePrefs()->lineWidth()),
                listVisible(context->annotatePrefs()->isListVisible())
            {}

            Enum::ANNOTATE_PRIMITIVE primitive;
            AV::Color color;
            size_t lineWidth;
            bool listVisible;
            QList<QPointer<Annotate::Data> > annotations;
            QPointer<Annotate::Data> currentAnnotation;

            QPointer<QButtonGroup> primitiveButtonGroup;
            QPointer<UI::ToolButton> colorButton;
            QPointer<QSpinBox> lineWidthSpinBox;
            QPointer<QTextEdit> textEdit;
            QPointer<AnnotateModel> model;
            QPointer<QTreeView> treeView;
            QPointer<UI::ToolButton> listVisibleButton;

            QPointer<QHBoxLayout> drawingLayout;
            QPointer<QHBoxLayout> controlsLayout;
        };

        AnnotateTool::AnnotateTool(
            const QPointer<AnnotateActions> & annotateActions,
            const QPointer<AnnotateGroup> & annotateGroup,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            AbstractTool(session, context, parent),
            _p(new Private(context))
        {
            // Create the models.
            _p->model = new AnnotateModel(context);

            // Create the widgets.
            _p->primitiveButtonGroup = new QButtonGroup(this);
            _p->primitiveButtonGroup->setExclusive(true);
            for (int i = 0; i < Enum::ANNOTATE_PRIMITIVE_COUNT; ++i)
            {
                auto button = new UI::ToolButton(context.data());
                button->setDefaultAction(annotateActions->group(AnnotateActions::PRIMITIVE_GROUP)->actions()[i]);
                _p->primitiveButtonGroup->addButton(button, i);
            }
            
            _p->colorButton = new UI::ToolButton(context.data());
            _p->colorButton->setDefaultAction(annotateActions->action(AnnotateActions::COLOR));

            _p->lineWidthSpinBox = new QSpinBox;
            _p->lineWidthSpinBox->setRange(1, 100);
            _p->lineWidthSpinBox->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Set the line width"));

            auto undoButton = new UI::ToolButton(context.data());
            undoButton->setDefaultAction(annotateActions->action(AnnotateActions::UNDO));

            auto redoButton = new UI::ToolButton(context.data());
            redoButton->setDefaultAction(annotateActions->action(AnnotateActions::REDO));

            auto clearButton = new UI::ToolButton(context.data());
            clearButton->setDefaultAction(annotateActions->action(AnnotateActions::CLEAR));

            _p->textEdit = new QTextEdit;

            _p->treeView = new QTreeView;
            _p->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
            _p->treeView->setModel(_p->model);

            auto newButton = new UI::ToolButton(context.data());
            newButton->setDefaultAction(annotateActions->action(AnnotateActions::NEW));

            auto deleteButton = new UI::ToolButton(context.data());
            deleteButton->setDefaultAction(annotateActions->action(AnnotateActions::DELETE));

            auto prevButton = new UI::ToolButton(context.data());
            prevButton->setDefaultAction(annotateActions->action(AnnotateActions::PREV));

            auto nextButton = new UI::ToolButton(context.data());
            nextButton->setDefaultAction(annotateActions->action(AnnotateActions::NEXT));

            _p->listVisibleButton = new UI::ToolButton(context.data());
            _p->listVisibleButton->setCheckable(true);
            _p->listVisibleButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Show the list of annotations"));

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
            //_p->drawingLayout->addWidget(undoButton);
            //_p->drawingLayout->addWidget(redoButton);
            _p->drawingLayout->addWidget(clearButton);
            _p->drawingLayout->addStretch();
            layout->addLayout(_p->drawingLayout);
            layout->addWidget(_p->textEdit);
            layout->addWidget(_p->treeView, 1);
            _p->controlsLayout = new QHBoxLayout;
            _p->controlsLayout->setMargin(0);
            _p->controlsLayout->addWidget(newButton);
            _p->controlsLayout->addWidget(deleteButton);
            _p->controlsLayout->addWidget(prevButton);
            _p->controlsLayout->addWidget(nextButton);
            _p->controlsLayout->addStretch();
            _p->controlsLayout->addWidget(_p->listVisibleButton);
            layout->addLayout(_p->controlsLayout);

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::AnnotateTool", "Annotate"));
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            styleUpdate();
            modelUpdate();
            widgetUpdate();
            auto playbackGroup = session->playbackGroup();
            _p->model->setFrameList(playbackGroup->sequence().frames);
            _p->model->setSpeed(playbackGroup->speed());

            // Setup the callbacks.
            connect(
                _p->primitiveButtonGroup,
                SIGNAL(buttonToggled(int, bool)),
                SLOT(primitiveCallback(int, bool)));

            connect(
                _p->lineWidthSpinBox,
                SIGNAL(valueChanged(int)),
                SLOT(lineWidthCallback(int)));

            connect(
                _p->textEdit,
                &QTextEdit::textChanged,
                [this, session]
            {
                const auto & text = _p->textEdit->toPlainText();
                if (!_p->currentAnnotation)
                {
                    session->annotateGroup()->newAnnotation(text);
                }
                else if (_p->currentAnnotation)
                {
                    _p->currentAnnotation->setText(text);
                }
            });

            connect(
                _p->treeView->selectionModel(),
                &QItemSelectionModel::currentChanged,
                [session](const QModelIndex & current, const QModelIndex &)
            {
                session->annotateGroup()->setCurrentAnnotation(reinterpret_cast<Annotate::Data *>(current.internalPointer()));
            });

            connect(
                _p->listVisibleButton,
                &UI::ToolButton::toggled,
                [this, context](bool value)
            {
                _p->listVisible = value;
                context->annotatePrefs()->setListVisible(value);
                widgetUpdate();
            });

            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::primitiveChanged,
                [this](Enum::ANNOTATE_PRIMITIVE value)
            {
                _p->primitive = value;
                widgetUpdate();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::colorChanged,
                [this](const AV::Color & value)
            {
                _p->color = value;
                widgetUpdate();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::lineWidthChanged,
                [this](size_t value)
            {
                _p->lineWidth = value;
                widgetUpdate();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::listVisibleChanged,
                [this](bool value)
            {
                _p->listVisible = value;
                widgetUpdate();
            });

            connect(
                playbackGroup,
                &PlaybackGroup::frameListChanged,
                [this](const FrameList & value)
            {
                _p->model->setFrameList(value);
            });
            connect(
                playbackGroup,
                &PlaybackGroup::speedChanged,
                [this](const Speed & value)
            {
                _p->model->setSpeed(value);
            });

            connect(
                annotateGroup,
                &AnnotateGroup::annotationsChanged,
                [this](const QList<Annotate::Data *> & value)
            {
                Q_FOREACH(auto i, _p->annotations)
                {
                    if (i)
                    {
                        disconnect(
                            i,
                            SIGNAL(textChanged(const QString &)),
                            this,
                            SLOT(modelUpdate()));
                        disconnect(
                            i,
                            SIGNAL(textChanged(const QString &)),
                            this,
                            SLOT(widgetUpdate()));
                    }
                }
                _p->annotations.clear();
                Q_FOREACH(auto i, value)
                {
                    _p->annotations.push_back(i);
                    connect(
                        i,
                        SIGNAL(textChanged(const QString &)),
                        SLOT(modelUpdate()));
                    connect(
                        i,
                        SIGNAL(textChanged(const QString &)),
                        SLOT(widgetUpdate()));
                }
                modelUpdate();
                widgetUpdate();
            });
            connect(
                annotateGroup,
                &AnnotateGroup::currentAnnotationChanged,
                [this](Annotate::Data * value)
            {
                _p->currentAnnotation = value;
                widgetUpdate();
            });
            connect(
                annotateGroup,
                &AnnotateGroup::annotationAdded,
                [this](Annotate::Data *)
            {
                _p->textEdit->setFocus();
            });
        }

        AnnotateTool::~AnnotateTool()
        {}

        void AnnotateTool::primitiveCallback(int id, bool checked)
        {
            if (checked)
            {
                _p->primitive = static_cast<Enum::ANNOTATE_PRIMITIVE>(id);
                context()->annotatePrefs()->setPrimitive(_p->primitive);
                widgetUpdate();
            }
        }

        void AnnotateTool::lineWidthCallback(int value)
        {
            _p->lineWidth = value;
            context()->annotatePrefs()->setLineWidth(value);
            widgetUpdate();
        }

        bool AnnotateTool::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return AbstractTool::event(event);
        }

        void AnnotateTool::styleUpdate()
        {
            _p->listVisibleButton->setIcon(context()->iconLibrary()->icon("djv/UI/ListIcon"));

            _p->drawingLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
            _p->controlsLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }

        void AnnotateTool::modelUpdate()
        {
            _p->model->setAnnotations(_p->annotations);
            _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
            const int index = _p->annotations.indexOf(_p->currentAnnotation);
            _p->treeView->selectionModel()->select(
                QItemSelection(_p->model->index(index, 0), _p->model->index(index, 1)),
                QItemSelectionModel::Select);
        }

        void AnnotateTool::widgetUpdate()
        {
            SignalBlocker signalBlocker(QObjectList() <<
                _p->primitiveButtonGroup <<
                _p->lineWidthSpinBox <<
                _p->textEdit <<
                _p->listVisibleButton);

            _p->primitiveButtonGroup->buttons()[_p->primitive]->setChecked(true);
            _p->lineWidthSpinBox->setValue(static_cast<int>(_p->lineWidth));
            
            const QString text = _p->currentAnnotation ? _p->currentAnnotation->text() : QString();
            if (text != _p->textEdit->toPlainText())
            {
                _p->textEdit->setText(text);
            }

            _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
            const int index = _p->annotations.indexOf(_p->currentAnnotation);
            _p->treeView->selectionModel()->select(
                QItemSelection(_p->model->index(index, 0), _p->model->index(index, 1)),
                QItemSelectionModel::Select);
            _p->treeView->setVisible(_p->listVisible);
            
            const int count = _p->annotations.count();
            _p->listVisibleButton->setChecked(_p->listVisible);
        }

    } // namespace ViewLib
} // namespace djv
