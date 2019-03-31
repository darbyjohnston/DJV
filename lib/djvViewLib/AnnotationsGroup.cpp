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

#include <djvViewLib/AnnotationsGroup.h>

#include <djvViewLib/AnnotationsActions.h>
#include <djvViewLib/AnnotationsData.h>
#include <djvViewLib/AnnotationsMenu.h>
#include <djvViewLib/AnnotationsPrefs.h>
#include <djvViewLib/AnnotationsTool.h>
#include <djvViewLib/AnnotationsToolBar.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/Session.h>

#include <QApplication>
#include <QDockWidget>

namespace djv
{
    namespace ViewLib
    {
        struct AnnotationsGroup::Private
        {
            Private(const QPointer<ViewContext> & context)
            {}

            Enum::PRIMITIVE              primitive = Enum::PRIMITIVE_POLYLINE;
            AV::Color                    color     = AV::Color(.8f, 0.f, 0.f);
            size_t                       lineWidth = 3;
            Annotations::Frames          frames;
            QPointer<AnnotationsActions> actions;
            QPointer<AnnotationsTool>    tool;
            QPointer<QDockWidget>        dockWidget;
        };

        AnnotationsGroup::AnnotationsGroup(
            const QPointer<AnnotationsGroup>& copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            AbstractGroup(session, context),
            _p(new Private(context))
        {
            if (copy)
            {
            }

            // Create the actions.
            _p->actions = new AnnotationsActions(context, this);

            // Create the widgets.
            _p->tool = new AnnotationsTool(session, context);
            _p->dockWidget = new QDockWidget(
                qApp->translate("djv::ViewLib::AnnotationsGroup", "Annotations"));
            _p->dockWidget->setWidget(_p->tool);

            // Initialize.
            update();

            // Setup callbacks.
            connect(
                _p->actions->action(AnnotationsActions::ANNOTATIONS_WIDGET),
                SIGNAL(toggled(bool)),
                SLOT(update()));

            _p->actions->action(AnnotationsActions::ANNOTATIONS_WIDGET)->connect(
                _p->dockWidget,
                SIGNAL(visibilityChanged(bool)),
                SLOT(setChecked(bool)));

            connect(
                _p->tool,
                SIGNAL(primitiveChanged(Enum::PRIMITIVE)),
                SLOT(setPrimitive(Enum::PRIMITIVE)));
            connect(
                _p->tool,
                SIGNAL(colorChanged(const AV::Color &)),
                SLOT(setColor(const AV::Color &)));
            connect(
                _p->tool,
                SIGNAL(lineWidthChanged(size_t)),
                SLOT(setLineWidth(size_t)));
            connect(
                _p->tool,
                SIGNAL(clearPrimitives()),
                SLOT(clearPrimitivesCallback()));

            connect(
                session->playbackGroup(),
                SIGNAL(frameChanged(qint64)),
                SLOT(update()));
        }

        AnnotationsGroup::~AnnotationsGroup()
        {}

        QPointer<QDockWidget> AnnotationsGroup::dockWidget() const
        {
            return _p->dockWidget;
        }

        void AnnotationsGroup::addAnnotation(const Annotations::Data & value)
        {
            auto playbackGroup = session()->playbackGroup();
            const qint64 frame = playbackGroup->frame();
            _p->frames[frame].push_back(value);
            update();
        }

        Enum::PRIMITIVE AnnotationsGroup::primitive() const
        {
            return _p->primitive;
        }

        const AV::Color & AnnotationsGroup::color() const
        {
            return _p->color;
        }

        size_t AnnotationsGroup::lineWidth() const
        {
            return _p->lineWidth;
        }

        QPointer<QMenu> AnnotationsGroup::createMenu() const
        {
            return new AnnotationsMenu(_p->actions.data());
        }

        QPointer<QToolBar> AnnotationsGroup::createToolBar() const
        {
            return new AnnotationsToolBar(_p->actions.data(), context());
        }

        void AnnotationsGroup::setPrimitive(Enum::PRIMITIVE value)
        {
            if (value == _p->primitive)
                return;
            _p->primitive = value;
            Q_EMIT primitiveChanged(_p->primitive);
        }

        void AnnotationsGroup::setColor(const AV::Color & value)
        {
            if (value == _p->color)
                return;
            _p->color = value;
            Q_EMIT colorChanged(_p->color);
        }

        void AnnotationsGroup::setLineWidth(size_t value)
        {
            if (value == _p->lineWidth)
                return;
            _p->lineWidth = value;
            Q_EMIT lineWidthChanged(_p->lineWidth);
        }

        void AnnotationsGroup::clearPrimitivesCallback()
        {

        }

        void AnnotationsGroup::update()
        {
            _p->dockWidget->setVisible(_p->actions->action(AnnotationsActions::ANNOTATIONS_WIDGET)->isChecked());
            const qint64 frame = session()->playbackGroup()->frame();
            const auto i = _p->frames.find(frame);
            if (i != _p->frames.end())
            {
                session()->viewWidget()->setAnnotationsData(i->second);
            }
            else
            {
                session()->viewWidget()->setAnnotationsData({});
            }
        }

    } // namespace ViewLib
} // namespace djv
