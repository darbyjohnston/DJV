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

#include <QApplication>
#include <QDockWidget>

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

            QList<Annotate::Data *> annotations;
            QPointer<Annotate::Data> currentAnnotation;
            Enum::ANNOTATE_PRIMITIVE primitive;
            AV::Color color;
            size_t lineWidth;

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

            _p->annotateTool = new AnnotateTool(session, context);
            _p->annotateToolDockWidget = new QDockWidget(qApp->translate("djv::ViewLib::AnnotateGroup", "Annotate Tool"));
            _p->annotateToolDockWidget->setWidget(_p->annotateTool);

            update();

            connect(
                _p->actions->action(AnnotateActions::ANNOTATE_TOOL),
                SIGNAL(toggled(bool)),
                SLOT(update()));

            _p->actions->action(AnnotateActions::ANNOTATE_TOOL)->connect(
                _p->annotateToolDockWidget,
                SIGNAL(visibilityChanged(bool)),
                SLOT(setChecked(bool)));

            connect(
                this,
                SIGNAL(annotationsChanged(const QList<djv::ViewLib::Annotate::Data *> &)),
                _p->annotateTool,
                SLOT(setAnnotations(const QList<djv::ViewLib::Annotate::Data *> &)));
            connect(
                this,
                SIGNAL(currentAnnotationChanged(const djv::ViewLib::Annotate::Data *)),
                _p->annotateTool,
                SLOT(setCurrentAnnotation(const djv::ViewLib::Annotate::Data *)));

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
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::colorChanged,
                [this](const AV::Color & value)
            {
                _p->color = value;
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::lineWidthChanged,
                [this](size_t value)
            {
                _p->lineWidth = value;
            });
        }

        AnnotateGroup::~AnnotateGroup()
        {}

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

        void AnnotateGroup::addAnnotation()
        {
            auto playbackGroup = session()->playbackGroup();
            playbackGroup->setPlayback(Enum::STOP);
            const qint64 frame = playbackGroup->frame();
            auto annotation = new Annotate::Data(frame);
            _p->annotations.push_back(annotation);
            _p->currentAnnotation = annotation;
            Q_EMIT annotationsChanged(_p->annotations);
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
        }

        void AnnotateGroup::removeAnnotation()
        {

        }

        void AnnotateGroup::clearAnnotations()
        {

        }

        void AnnotateGroup::setCurrentAnnotation(Annotate::Data * value)
        {
            if (value == _p->currentAnnotation)
                return;
            _p->currentAnnotation = value;
            if (_p->currentAnnotation)
            {
                auto playbackGroup = session()->playbackGroup();
                playbackGroup->setFrame(_p->currentAnnotation->frame());
            }
            Q_EMIT currentAnnotationChanged(_p->currentAnnotation);
        }

        void AnnotateGroup::nextAnnotation()
        {

        }

        void AnnotateGroup::prevAnnotation()
        {

        }

        void AnnotateGroup::undoDrawing()
        {

        }

        void AnnotateGroup::redoDrawing()
        {

        }

        void AnnotateGroup::exportAnnotations()
        {

        }

        void AnnotateGroup::pickPressedCallback(const glm::ivec2 & in)
        {
            if (_p->annotateToolDockWidget->isVisible())
            {
                if (!_p->currentAnnotation)
                {
                    addAnnotation();
                }
                if (_p->currentAnnotation)
                {
                    _p->currentAnnotation->addPrimitive(_p->primitive, _p->color, _p->lineWidth);
                    _p->currentAnnotation->mouse(in);
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
                const auto view = session()->viewWidget();
                const glm::ivec2 & viewPos = view->viewPos();
                const float viewZoom = view->viewZoom();
                _p->currentAnnotation->mouse(glm::ivec2(
                    (in.x - viewPos.x) / viewZoom,
                    (in.y - viewPos.y) / viewZoom));
            }
        }

        void AnnotateGroup::update()
        {
            _p->annotateToolDockWidget->setVisible(_p->actions->action(AnnotateActions::ANNOTATE_TOOL)->isChecked());
        }

    } // namespace ViewLib
} // namespace djv
