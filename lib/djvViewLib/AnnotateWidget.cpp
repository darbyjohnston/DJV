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

#include <djvViewLib/AnnotateWidget.h>

#include <djvViewLib/AnnotateActions.h>
#include <djvViewLib/AnnotateData.h>
#include <djvViewLib/AnnotateGroup.h>
#include <djvViewLib/AnnotateModel.h>
#include <djvViewLib/AnnotatePrefs.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/FileEdit.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/ToolButton.h>

#include <djvAV/Color.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTreeView>
#include <QMenu>
#include <QPointer>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

#undef DELETE

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            const std::vector<std::pair<QString, QString> > exportFileFormats =
            {
                { ".png", "PNG" },
                { ".jpg", "JPG" }
            };

        } // namespace

        struct AnnotateEditWidget::Private
        {
            Private(const QPointer<ViewContext> & context) :
                context(context),
                primitive(context->annotatePrefs()->primitive()),
                color(context->annotatePrefs()->color()),
                lineWidth(context->annotatePrefs()->lineWidth()),
                listVisible(context->annotatePrefs()->isListVisible())
            {}

            QPointer<ViewContext> context;

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

        AnnotateEditWidget::AnnotateEditWidget(
            const QPointer<AnnotateActions> & annotateActions,
            const QPointer<AnnotateGroup> & annotateGroup,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            QWidget(parent),
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
                qApp->translate("djv::ViewLib::AnnotateEditWidget", "Set the line width"));

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
                qApp->translate("djv::ViewLib::AnnotateEditWidget", "Show the list of annotations"));

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
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
            styleUpdate();
            modelUpdate();
            widgetUpdate();
            auto playbackGroup = session->playbackGroup();
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
                &QItemSelectionModel::selectionChanged,
                [session](const QItemSelection & selected, const QItemSelection&)
            {
                auto indexes = selected.indexes();
                if (indexes.count())
                {
                    session->annotateGroup()->setCurrentAnnotation(reinterpret_cast<Annotate::Data*>(indexes[0].internalPointer()));
                }
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

        AnnotateEditWidget::~AnnotateEditWidget()
        {}

        void AnnotateEditWidget::primitiveCallback(int id, bool checked)
        {
            if (checked)
            {
                _p->primitive = static_cast<Enum::ANNOTATE_PRIMITIVE>(id);
                _p->context->annotatePrefs()->setPrimitive(_p->primitive);
                widgetUpdate();
            }
        }

        void AnnotateEditWidget::lineWidthCallback(int value)
        {
            _p->lineWidth = value;
            _p->context->annotatePrefs()->setLineWidth(value);
            widgetUpdate();
        }

        bool AnnotateEditWidget::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QWidget::event(event);
        }

        void AnnotateEditWidget::styleUpdate()
        {
            _p->listVisibleButton->setIcon(_p->context->iconLibrary()->icon("djv/UI/ListIcon"));

            _p->drawingLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
            _p->controlsLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }

        void AnnotateEditWidget::modelUpdate()
        {
            _p->model->setAnnotations(_p->annotations);
            const int index = _p->annotations.indexOf(_p->currentAnnotation);
            _p->treeView->selectionModel()->select(
                QItemSelection(_p->model->index(index, 0), _p->model->index(index, 1)),
                QItemSelectionModel::ClearAndSelect);
        }

        void AnnotateEditWidget::widgetUpdate()
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

            const int index = _p->annotations.indexOf(_p->currentAnnotation);
            _p->treeView->selectionModel()->select(
                QItemSelection(_p->model->index(index, 0), _p->model->index(index, 1)),
                QItemSelectionModel::ClearAndSelect);
            _p->treeView->setVisible(_p->listVisible);
            
            const int count = _p->annotations.count();
            _p->listVisibleButton->setChecked(_p->listVisible);
        }

        struct AnnotateSummaryWidget::Private
        {
            QPointer<QTextEdit> textEdit;
        };

        AnnotateSummaryWidget::AnnotateSummaryWidget(
            const QPointer<AnnotateGroup> & annotateGroup,
            const QPointer<Session> &,
            const QPointer<ViewContext> &,
            QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->textEdit = new QTextEdit;
            _p->textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            auto layout = new QVBoxLayout(this);
            layout->addWidget(_p->textEdit);

            connect(
                _p->textEdit,
                &QTextEdit::textChanged,
                [this, annotateGroup]
            {
                annotateGroup->setSummary(_p->textEdit->toPlainText());
            });

            connect(
                annotateGroup,
                &AnnotateGroup::summaryChanged,
                [this](const QString & value)
            {
                const QString text = _p->textEdit->toPlainText();
                if (value != text)
                {
                    _p->textEdit->setText(value);
                }
            });
        }

        AnnotateSummaryWidget::~AnnotateSummaryWidget()
        {}

        void AnnotateSummaryWidget::widgetUpdate()
        {}

        struct AnnotateExportWidget::Private
        {
            Private(const QPointer<ViewContext> & context) :
                extension(context->annotatePrefs()->exportExtension()),
                script(context->annotatePrefs()->exportScript()),
                scriptOptions(context->annotatePrefs()->exportScriptOptions()),
                scriptInterpreter(context->annotatePrefs()->exportScriptInterpreter())
            {}

            QPointer<ViewContext> context;
            QPointer<AnnotateGroup> group;
            FileInfo fileInfo;
            QString extension;
            FileInfo script;
            QString scriptOptions;
            QString scriptInterpreter;
            QPointer< UI::FileEdit> fileEdit;
            QPointer<QComboBox> extensionComboBox;
            QPointer<UI::FileEdit> scriptFileEdit;
            QPointer<QLineEdit> scriptOptionsLineEdit;
            QPointer<UI::FileEdit> scriptInterpreterFileEdit;
            QPointer<QPushButton> exportButton;
        };

        AnnotateExportWidget::AnnotateExportWidget(
            const QPointer<AnnotateGroup> & group,
            const QPointer<Session> &,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            QWidget(parent),
            _p(new Private(context))
        {
            _p->context = context;
            _p->group = group;

            _p->fileEdit = new UI::FileEdit(context.data());
            _p->extensionComboBox = new QComboBox;
            auto imagesGroupBox = new QGroupBox(qApp->translate("djv::ViewLib::AnnotateExportWidget", "Images"));
            auto vLayout = new QVBoxLayout;
            vLayout->addWidget(_p->fileEdit);
            vLayout->addWidget(new QLabel(qApp->translate("djv::ViewLib::AnnotateExportWidget", "Format:")));
            vLayout->addWidget(_p->extensionComboBox);
            imagesGroupBox->setLayout(vLayout);

            _p->scriptFileEdit = new UI::FileEdit(context.data());
            _p->scriptOptionsLineEdit = new QLineEdit;
            _p->scriptInterpreterFileEdit = new UI::FileEdit(context.data());
            auto scriptGroupBox = new QGroupBox(qApp->translate("djv::ViewLib::AnnotateExportWidget", "Script"));
            vLayout = new QVBoxLayout;
            vLayout->addWidget(_p->scriptFileEdit);
            vLayout->addWidget(new QLabel(qApp->translate("djv::ViewLib::AnnotateExportWidget", "Options:")));
            vLayout->addWidget(_p->scriptOptionsLineEdit);
            vLayout->addWidget(new QLabel(qApp->translate("djv::ViewLib::AnnotateExportWidget", "Interpreter:")));
            vLayout->addWidget(_p->scriptInterpreterFileEdit);
            scriptGroupBox->setLayout(vLayout);

            _p->exportButton = new QPushButton;
            _p->exportButton->setText(qApp->translate("djv::ViewLib::AnnotateExportWidget", "Export"));

            auto layout = new QVBoxLayout(this);
            layout->addWidget(imagesGroupBox);
            layout->addWidget(scriptGroupBox);
            layout->addWidget(_p->exportButton);
            layout->addStretch(1);

            setWindowTitle(qApp->translate("djv::ViewLib::AnnotateExportWidget", "Export Annotations"));
            setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
            widgetUpdate();

            connect(
                _p->fileEdit,
                &UI::FileEdit::fileInfoChanged,
                [this, group](const FileInfo & value)
            {
                _p->fileInfo = value;
                group->setExport(_p->fileInfo);
            });

            connect(
                _p->extensionComboBox,
                SIGNAL(currentIndexChanged(int)),
                SLOT(exportExtensionCallback(int)));

            connect(
                _p->scriptFileEdit,
                &UI::FileEdit::fileInfoChanged,
                [this, context](const FileInfo& value)
            {
                _p->script = value;
                context->annotatePrefs()->setExportScript(value);
            });

            connect(
                _p->scriptOptionsLineEdit,
                &QLineEdit::editingFinished,
                [this, group, context]
            {
                const auto& text = _p->scriptOptionsLineEdit->text();
                _p->scriptOptions = text;
                context->annotatePrefs()->setExportScriptOptions(text);
            });

            connect(
                _p->scriptInterpreterFileEdit,
                &UI::FileEdit::fileInfoChanged,
                [this, context](const FileInfo & value)
            {
                _p->scriptInterpreter = value;
                context->annotatePrefs()->setExportScriptInterpreter(value);
            });

            connect(
                _p->exportButton,
                &QPushButton::clicked,
                [this, group]
            {
                group->exportAnnotations();
            });

            connect(
                group,
                &AnnotateGroup::exportChanged,
                [this](const FileInfo & value)
            {
                _p->fileInfo = value;
                widgetUpdate();
            });

            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::exportExtensionChanged,
                [this](const QString& value)
            {
                _p->extension = value;
                widgetUpdate();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::exportScriptChanged,
                [this](const FileInfo & value)
            {
                _p->script = value;
                widgetUpdate();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::exportScriptOptionsChanged,
                [this](const QString& value)
            {
                _p->scriptOptions = value;
                widgetUpdate();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::exportScriptInterpreterChanged,
                [this](const FileInfo& value)
            {
                _p->scriptInterpreter = value;
                widgetUpdate();
            });
        }

        AnnotateExportWidget::~AnnotateExportWidget()
        {}

        void AnnotateExportWidget::exportExtensionCallback(int value)
        {
            _p->extension = exportFileFormats[value].first;
            _p->fileInfo.setExtension(_p->extension);
            _p->group->setExport(_p->fileInfo);
            _p->context->annotatePrefs()->setExportExtension(_p->extension);
            widgetUpdate();
        }

        void AnnotateExportWidget::widgetUpdate()
        {
            SignalBlocker signalBlocker(QObjectList() <<
                _p->fileEdit <<
                _p->extensionComboBox <<
                _p->scriptFileEdit <<
                _p->scriptOptionsLineEdit <<
                _p->scriptInterpreterFileEdit <<
                _p->exportButton);
            _p->fileEdit->setFileInfo(_p->fileInfo);
            _p->extensionComboBox->clear();
            QStringList fileFormatExtensions;
            QStringList fileFormatNames;
            for (const auto& i : exportFileFormats)
            {
                fileFormatExtensions.append(i.first);
                fileFormatNames.append(i.second);
            }
            _p->extensionComboBox->addItems(fileFormatNames);
            _p->extensionComboBox->setCurrentIndex(fileFormatExtensions.indexOf(_p->extension));
            _p->scriptFileEdit->setFileInfo(_p->script);
            _p->scriptOptionsLineEdit->setText(_p->scriptOptions);
            _p->scriptInterpreterFileEdit->setFileInfo(_p->scriptInterpreter);
            _p->exportButton->setEnabled(!_p->fileInfo.isEmpty());
        }

    } // namespace ViewLib
} // namespace djv
