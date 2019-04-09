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

#include <djvViewLib/AnnotateGroup.h>

#include <djvViewLib/AnnotateActions.h>
#include <djvViewLib/AnnotateData.h>
#include <djvViewLib/AnnotateColorDialog.h>
#include <djvViewLib/AnnotateExportDialog.h>
#include <djvViewLib/AnnotateMenu.h>
#include <djvViewLib/AnnotatePrefs.h>
#include <djvViewLib/AnnotateTool.h>
#include <djvViewLib/AnnotateToolBar.h>
#include <djvViewLib/FileGroup.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/QuestionDialog.h>
#include <djvUI/StylePrefs.h>

#include <QApplication>
#include <QDockWidget>
#include <QStyle>

#undef DELETE

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct AnnotateGroup::Private
        {
            Private(const QPointer<ViewContext> & context) :
                primitive(context->annotatePrefs()->primitive()),
                color(context->annotatePrefs()->color()),
                lineWidth(context->annotatePrefs()->lineWidth())
            {}

            QList<Annotate::Data *>  annotations;
            QList<Annotate::Data *>  frameAnnotations;
            QPointer<Annotate::Data> currentAnnotation;
            Enum::ANNOTATE_PRIMITIVE primitive;
            AV::Color                color;
            size_t                   lineWidth;

            QPointer<AnnotateActions> actions;
            QPointer<AnnotateTool>    annotateTool;
            QPointer<QDockWidget>     annotateToolDockWidget;
        };

        AnnotateGroup::AnnotateGroup(
            const QPointer<AnnotateGroup>& copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            AbstractGroup(session, context),
            _p(new Private(context))
        {
            if (copy)
            {
            }

            _p->actions = new AnnotateActions(context, this);

            _p->annotateTool = new AnnotateTool(_p->actions, this, session, context);
            _p->annotateToolDockWidget = new QDockWidget(qApp->translate("djv::ViewLib::AnnotateGroup", "Annotate Tool"));
            _p->annotateToolDockWidget->setWidget(_p->annotateTool);

            update();

            connect(
                _p->actions->action(AnnotateActions::ANNOTATE_TOOL),
                SIGNAL(toggled(bool)),
                SLOT(update()));
            connect(
                _p->actions->action(AnnotateActions::COLOR),
                &QAction::triggered,
                [this, context]
            {
                auto dialog = QSharedPointer<AnnotateColorDialog>(new AnnotateColorDialog(context));
                dialog->setColor(_p->color);
                connect(
                    dialog.data(),
                    &AnnotateColorDialog::colorChanged,
                    [this, context](const AV::Color & value)
                {
                    _p->color = value;
                    context->annotatePrefs()->setColor(_p->color);
                    update();
                });
                dialog->exec();
            });
            connect(
                _p->actions->action(AnnotateActions::LINE_WIDTH_INC),
                &QAction::triggered,
                [this, context]
            {
                _p->lineWidth = static_cast<size_t>(Math::clamp(static_cast<int>(_p->lineWidth) + 10, 1, 100));
                context->annotatePrefs()->setLineWidth(_p->lineWidth);
            });
            connect(
                _p->actions->action(AnnotateActions::LINE_WIDTH_DEC),
                &QAction::triggered,
                [this, context]
            {
                _p->lineWidth = static_cast<size_t>(Math::clamp(static_cast<int>(_p->lineWidth) - 10, 1, 100));
                context->annotatePrefs()->setLineWidth(_p->lineWidth);
            });
            connect(
                _p->actions->action(AnnotateActions::UNDO),
                &QAction::triggered,
                [this]
            {

            });
            connect(
                _p->actions->action(AnnotateActions::REDO),
                &QAction::triggered,
                [this]
            {

            });
            connect(
                _p->actions->action(AnnotateActions::CLEAR),
                &QAction::triggered,
                [this]
            {
                clearDrawing();
            });
            connect(
                _p->actions->action(AnnotateActions::NEW),
                &QAction::triggered,
                [this]
            {
                newAnnotation();
            });
            connect(
                _p->actions->action(AnnotateActions::DELETE),
                &QAction::triggered,
                [this]
            {
                deleteAnnotation();
            });
            connect(
                _p->actions->action(AnnotateActions::NEXT),
                &QAction::triggered,
                [this]
            {
                nextAnnotation();
            });
            connect(
                _p->actions->action(AnnotateActions::PREV),
                &QAction::triggered,
                [this]
            {
                prevAnnotation();
            });
            connect(
                _p->actions->action(AnnotateActions::EXPORT),
                &QAction::triggered,
                [this]
            {
                exportAnnotations();
            });

            connect(
                _p->actions->group(AnnotateActions::PRIMITIVE_GROUP),
                &QActionGroup::triggered,
                [this, context](QAction * action)
            {
                _p->primitive = static_cast<Enum::ANNOTATE_PRIMITIVE>(action->data().toInt());
                context->annotatePrefs()->setPrimitive(_p->primitive);
            });

            _p->actions->action(AnnotateActions::ANNOTATE_TOOL)->connect(
                _p->annotateToolDockWidget,
                SIGNAL(visibilityChanged(bool)),
                SLOT(setChecked(bool)));

            auto fileGroup = session->fileGroup();
            connect(
                fileGroup,
                &FileGroup::fileInfoChanged,
                [this](const FileInfo & value)
            {
                clearAnnotations();
            });

            auto playbackGroup = session->playbackGroup();
            connect(
                playbackGroup,
                &PlaybackGroup::frameChanged,
                [this](qint64 value)
            {
                _p->frameAnnotations.clear();
                Q_FOREACH(auto i, _p->annotations)
                {
                    if (i->frame() == value)
                    {
                        _p->frameAnnotations.push_back(i);
                    }
                }
                const int index = _p->frameAnnotations.indexOf(_p->currentAnnotation);
                if (-1 == index)
                {
                    if (_p->frameAnnotations.count())
                    {
                        _p->currentAnnotation = _p->frameAnnotations.back();
                    }
                    else
                    {
                        _p->currentAnnotation.clear();
                    }
                }
                update();
                Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
                Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
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
                &AnnotatePrefs::primitiveChanged,
                [this](Enum::ANNOTATE_PRIMITIVE value)
            {
                _p->primitive = value;
                update();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::colorChanged,
                [this](const AV::Color & value)
            {
                _p->color = value;
                update();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::lineWidthChanged,
                [this](size_t value)
            {
                _p->lineWidth = value;
                update();
            });

            connect(
                context->stylePrefs(),
                &UI::StylePrefs::prefChanged,
                [this]
            {
                update();
            });
        }

        AnnotateGroup::~AnnotateGroup()
        {}

        const QList<Annotate::Data *> & AnnotateGroup::annotations() const
        {
            return _p->annotations;
        }

        const QList<Annotate::Data *> & AnnotateGroup::frameAnnotations() const
        {
            return _p->frameAnnotations;
        }

        Annotate::Data * AnnotateGroup::currentAnnotation() const
        {
            return _p->currentAnnotation;
        }

        QPointer<QDockWidget> AnnotateGroup::annotateToolDockWidget() const
        {
            return _p->annotateToolDockWidget;
        }

        QPointer<QMenu> AnnotateGroup::createMenu() const
        {
            return new AnnotateMenu(_p->actions.data());
        }

        QPointer<QToolBar> AnnotateGroup::createToolBar() const
        {
            return new AnnotateToolBar(_p->actions.data(), context());
        }

        void AnnotateGroup::newAnnotation(const QString & text)
        {
            auto playbackGroup = session()->playbackGroup();
            playbackGroup->setPlayback(Enum::STOP);
            const qint64 frame = playbackGroup->frame();
            auto annotation = new Annotate::Data(frame, text);
            connect(
                annotation,
                SIGNAL(primitivesChanged()),
                SLOT(update()));
            _p->annotations.push_back(annotation);
            qStableSort(_p->annotations.begin(), _p->annotations.end(),
                [](const Annotate::Data * a, const Annotate::Data * b)
            {
                return a && b ? (a->frame() < b->frame()) : 0;
            });
            _p->frameAnnotations.clear();
            Q_FOREACH(auto i, _p->annotations)
            {
                if (i->frame() == frame)
                {
                    _p->frameAnnotations.push_back(i);
                }
            }
            _p->currentAnnotation = annotation;
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
            Q_EMIT annotationsChanged(_p->annotations);
            Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
            Q_EMIT annotationAdded(annotation);
        }

        void AnnotateGroup::deleteAnnotation()
        {
            int index = _p->annotations.indexOf(_p->currentAnnotation);
            if (index != -1)
            {
                UI::QuestionDialog dialog(
                    qApp->translate("djv::ViewLib::AnnotateGroup", "Are you sure you want to delete the current annotation?"));
                if (QDialog::Accepted == dialog.exec())
                {
                    delete _p->annotations[index];
                    _p->annotations.removeAt(index);
                    _p->frameAnnotations.clear();
                    const qint64 frame = session()->playbackGroup()->frame();
                    Q_FOREACH(auto i, _p->annotations)
                    {
                        if (i->frame() == frame)
                        {
                            _p->frameAnnotations.push_back(i);
                        }
                    }
                    if (index >= _p->annotations.size())
                    {
                        --index;
                    }
                    if (index >= 0 && index < _p->annotations.size())
                    {
                        _p->currentAnnotation = _p->annotations[index];
                    }
                    update();
                    Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
                    Q_EMIT annotationsChanged(_p->annotations);
                    Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
                }
            }
        }

        void AnnotateGroup::clearAnnotations()
        {
            Q_FOREACH(auto i, _p->annotations)
            {
                delete i;
            }
            _p->annotations.clear();
            _p->frameAnnotations.clear();
            _p->currentAnnotation.clear();
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
            Q_EMIT annotationsChanged(_p->annotations);
            Q_EMIT frameAnnotationsChanged(_p->frameAnnotations);
        }

        void AnnotateGroup::setCurrentAnnotation(Annotate::Data * value)
        {
            if (value == _p->currentAnnotation)
                return;
            _p->currentAnnotation = value;
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
        }

        void AnnotateGroup::nextAnnotation()
        {
            int index = _p->annotations.indexOf(_p->currentAnnotation);
            ++index;
            if (index >= _p->annotations.size())
            {
                index = 0;
            }
            if (index >= 0 && index < _p->annotations.count())
            {
                _p->currentAnnotation = _p->annotations[index];
            }
            else
            {
                _p->currentAnnotation.clear();
            }
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
        }

        void AnnotateGroup::prevAnnotation()
        {
            int index = _p->annotations.indexOf(_p->currentAnnotation);
            --index;
            if (index < 0)
            {
                index = _p->annotations.count() - 1;
            }
            if (index >= 0 && index < _p->annotations.count())
            {
                _p->currentAnnotation = _p->annotations[index];
            }
            else
            {
                _p->currentAnnotation.clear();
            }
            update();
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
        }

        void AnnotateGroup::undoDrawing()
        {

        }

        void AnnotateGroup::redoDrawing()
        {

        }

        void AnnotateGroup::clearDrawing()
        {
            if (_p->currentAnnotation)
            {
                UI::QuestionDialog dialog(
                    qApp->translate("djv::ViewLib::AnnotateGroup", "Are you sure you want to clear the drawing?"));
                if (QDialog::Accepted == dialog.exec())
                {
                    _p->currentAnnotation->clearPrimitives();
                }
            }
        }

        void AnnotateGroup::exportAnnotations()
        {
            if (QDialog::Accepted == AnnotateExportDialog(context()).exec())
            {
            }
        }

        void AnnotateGroup::pickPressedCallback(const glm::ivec2 & in)
        {
            if (_p->annotateToolDockWidget->isVisible())
            {
                if (!_p->currentAnnotation)
                {
                    newAnnotation();
                }
                if (_p->currentAnnotation)
                {
                    _p->currentAnnotation->addPrimitive(_p->primitive, _p->color, _p->lineWidth);
                    _p->currentAnnotation->mouse(transformMousePos(in));
                }
            }
        }

        void AnnotateGroup::pickReleasedCallback(const glm::ivec2 & in)
        {
        }

        void AnnotateGroup::pickMovedCallback(const glm::ivec2 & in)
        {
            if (_p->annotateToolDockWidget->isVisible() && _p->currentAnnotation)
            {
                _p->currentAnnotation->mouse(transformMousePos(in));
            }
        }

        void AnnotateGroup::update()
        {
            const int size = qApp->style()->pixelMetric(QStyle::PM_ButtonIconSize);
            QImage image(size, size, QImage::Format_ARGB32);
            image.fill(AV::ColorUtil::toQt(_p->color));
            _p->actions->action(AnnotateActions::COLOR)->setIcon(QPixmap::fromImage(image));

            const int count = _p->annotations.count();
            _p->actions->action(AnnotateActions::CLEAR)->setEnabled(_p->currentAnnotation ? _p->currentAnnotation->primitives().size() : false);
            _p->actions->action(AnnotateActions::NEXT)->setEnabled(count > 1);
            _p->actions->action(AnnotateActions::PREV)->setEnabled(count > 1);

            _p->actions->group(AnnotateActions::PRIMITIVE_GROUP)->actions()[_p->primitive]->setChecked(true);

            _p->annotateToolDockWidget->setVisible(_p->actions->action(AnnotateActions::ANNOTATE_TOOL)->isChecked());
        }

        glm::ivec2 AnnotateGroup::transformMousePos(const glm::ivec2 & value) const
        {
            const auto view = session()->viewWidget();
            const glm::ivec2 & viewPos = view->viewPos();
            const float viewZoom = view->viewZoom();
            return glm::ivec2(
                (value.x - viewPos.x) / viewZoom,
                (value.y - viewPos.y) / viewZoom);
        }

    } // namespace ViewLib
} // namespace djv
