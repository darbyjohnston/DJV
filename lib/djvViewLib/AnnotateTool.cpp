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
#include <djvViewLib/AnnotateGroup.h>
#include <djvViewLib/AnnotateModel.h>
#include <djvViewLib/AnnotatePrefs.h>
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
            const QPointer<AnnotateGroup> & annotateGroup,
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
            const QStringList toolTips = QStringList() <<
                qApp->translate("djv::ViewLib::AnnotateTool", "Draw freehand lines") <<
                qApp->translate("djv::ViewLib::AnnotateTool", "Draw rectangles") <<
                qApp->translate("djv::ViewLib::AnnotateTool", "Draw circles");
            for (int i = 0; i < Enum::ANNOTATE_PRIMITIVE_COUNT; ++i)
            {
                auto button = new UI::ToolButton(context.data());
                button->setCheckable(true);
                button->setToolTip(toolTips[i]);
                _p->primitiveButtonGroup->addButton(button, i);
            }
            
            _p->colorButton = new UI::ToolButton(context.data());
            _p->colorButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Set the drawing color"));

            _p->lineWidthSpinBox = new QSpinBox;
            _p->lineWidthSpinBox->setRange(1, 100);
            _p->lineWidthSpinBox->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Set the line width"));

            _p->undoButton = new UI::ToolButton(context.data());
            _p->undoButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Undo drawing"));

            _p->redoButton = new UI::ToolButton(context.data());
            _p->redoButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Redo drawing"));

            _p->clearButton = new UI::ToolButton(context.data());
            _p->clearButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Clear the drawing"));

            _p->textEdit = new QTextEdit;

            _p->treeView = new QTreeView;
            _p->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
            _p->treeView->setModel(_p->model);

            _p->prevButton = new UI::ToolButton(context.data());
            _p->prevButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Go the previous annotation"));

            _p->nextButton = new UI::ToolButton(context.data());
            _p->nextButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Go to the next annotation"));

            _p->listVisibleButton = new UI::ToolButton(context.data());
            _p->listVisibleButton->setCheckable(true);
            _p->listVisibleButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Show the list of annotations"));

            _p->addButton = new UI::ToolButton(context.data());
            _p->addButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Add a new annotation"));

            _p->removeButton = new UI::ToolButton(context.data());
            _p->removeButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Remove the current annotation"));

            _p->exportButton = new UI::ToolButton(context.data());
            _p->exportButton->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateTool", "Export the annotations"));

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
                [session]
            {
                session->annotateGroup()->undoDrawing();
            });

            connect(
                _p->redoButton,
                &UI::ToolButton::clicked,
                [session]
            {
                session->annotateGroup()->redoDrawing();
            });

            connect(
                _p->clearButton,
                &UI::ToolButton::clicked,
                [this]
            {
                if (_p->currentAnnotation)
                {
                    _p->currentAnnotation->clearPrimitives();
                }
            });

            connect(
                _p->textEdit,
                &QTextEdit::textChanged,
                [this, session]
            {
                const auto & text = _p->textEdit->toPlainText();
                if (!_p->currentAnnotation)
                {
                    session->annotateGroup()->addAnnotation(text);
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
                _p->prevButton,
                &UI::ToolButton::clicked,
                [session]
            {
                session->annotateGroup()->prevAnnotation();
            });

            connect(
                _p->nextButton,
                &UI::ToolButton::clicked,
                [session]
            {
                session->annotateGroup()->nextAnnotation();
            });

            connect(
                _p->addButton,
                &UI::ToolButton::clicked,
                [session]
            {
                session->annotateGroup()->addAnnotation();
            });

            connect(
                _p->removeButton,
                &UI::ToolButton::clicked,
                [session]
            {
                session->annotateGroup()->removeAnnotation();
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
                _p->exportButton,
                &UI::ToolButton::clicked,
                [session]
            {
                session->annotateGroup()->exportAnnotations();
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
                        SLOT(widgetUpdate()));
                }
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
            _p->addButton->setIcon(context()->iconLibrary()->icon("djv/UI/AddIcon"));
            _p->removeButton->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));
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
                _p->textEdit <<
                _p->listVisibleButton);

            _p->primitiveButtonGroup->buttons()[_p->primitive]->setChecked(true);
            _p->lineWidthSpinBox->setValue(static_cast<int>(_p->lineWidth));
            
            const QString text = _p->currentAnnotation ? _p->currentAnnotation->text() : QString();
            if (text != _p->textEdit->toPlainText())
            {
                _p->textEdit->setText(text);
            }

            _p->model->setAnnotations(_p->annotations);
            _p->treeView->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
            const int index = _p->annotations.indexOf(_p->currentAnnotation);
            _p->treeView->selectionModel()->select(
                QItemSelection(_p->model->index(index, 0), _p->model->index(index, 1)),
                QItemSelectionModel::Select);
            _p->treeView->setVisible(_p->listVisible);
            
            const int count = _p->annotations.count();
            _p->prevButton->setEnabled(count > 1);
            _p->nextButton->setEnabled(count > 1);
            _p->removeButton->setEnabled(_p->currentAnnotation);
            _p->listVisibleButton->setChecked(_p->listVisible);
            _p->exportButton->setEnabled(count > 0);
        }

    } // namespace ViewLib
} // namespace djv
