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

#include <djvViewLib/Session.h>

#include <djvViewLib/AnnotateData.h>
#include <djvViewLib/AnnotateGroup.h>
#include <djvViewLib/ControlsWindow.h>
#include <djvViewLib/FileCache.h>
#include <djvViewLib/FileExport.h>
#include <djvViewLib/FileGroup.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/HelpGroup.h>
#include <djvViewLib/HudInfo.h>
#include <djvViewLib/ImageGroup.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/PlaybackPrefs.h>
#include <djvViewLib/StatusBar.h>
#include <djvViewLib/ToolGroup.h>
#include <djvViewLib/ViewContext.h>
#include <djvViewLib/ViewGroup.h>
#include <djvViewLib/ViewPrefs.h>
#include <djvViewLib/ViewPrefsWidget.h>
#include <djvViewLib/WindowGroup.h>
#include <djvViewLib/WindowPrefs.h>

#include <djvUI/OpenGLPrefs.h>

#include <djvCore/DebugLog.h>

#include <QApplication>
#include <QDockWidget>
#include <QMenu>
#include <QToolBar>

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            QVector<QPointer<Session> > _sessionList;

        } // namespace

        struct Session::Private
        {
            Private(const QPointer<ViewContext> & context) :
                context(context)
            {}

            std::shared_ptr<AV::Image> image;
            std::shared_ptr<AV::Image> frameStoreImage;

            QPointer<ImageView> viewWidget;

            QScopedPointer<FileGroup> fileGroup;
            QScopedPointer<WindowGroup> windowGroup;
            QScopedPointer<ViewGroup> viewGroup;
            QScopedPointer<ImageGroup> imageGroup;
            QScopedPointer<PlaybackGroup> playbackGroup;
            QScopedPointer<ToolGroup> toolGroup;
            QScopedPointer<AnnotateGroup> annotateGroup;
            QScopedPointer<HelpGroup> helpGroup;

            QScopedPointer<MainWindow> mainWindow;

            QPointer<ViewContext> context;
        };

        Session::Session(const QPointer<Session> & copy, const QPointer<ViewContext> & context, QObject * parent) :
            QObject(parent),
            _p(new Private(context))
        {
            _sessionList.append(this);

            // Create the view.
            _p->viewWidget = new ImageView(context);

            // Create the groups.
            _p->fileGroup.reset(new FileGroup(copy ? copy->_p->fileGroup.data() : nullptr, this, context));
            _p->windowGroup.reset(new WindowGroup(copy ? copy->_p->windowGroup.data() : nullptr, this, context));
            _p->viewGroup.reset(new ViewGroup(copy ? copy->_p->viewGroup.data() : nullptr, this, context));
            _p->imageGroup.reset(new ImageGroup(copy ? copy->_p->imageGroup.data() : nullptr, this, context));
            _p->playbackGroup.reset(new PlaybackGroup(copy ? copy->_p->playbackGroup.data() : nullptr, this, context));
            _p->toolGroup.reset(new ToolGroup(copy ? copy->_p->toolGroup.data() : nullptr, this, context));
            _p->annotateGroup.reset(new AnnotateGroup(copy ? copy->_p->annotateGroup.data() : nullptr, this, context));
            _p->helpGroup.reset(new HelpGroup(copy ? copy->_p->helpGroup.data() : nullptr, this, context));

            // Create the main window.
            _p->mainWindow.reset(new MainWindow(copy ? copy : nullptr, this, context));
            _p->mainWindow->addDockWidget(Qt::RightDockWidgetArea, _p->imageGroup->displayProfileDockWidget());
            const auto & toolDockWidgets = _p->toolGroup->dockWidgets();
            const auto toolDockWidgetAreas = QList<Qt::DockWidgetArea>() <<
                Qt::LeftDockWidgetArea <<
                Qt::LeftDockWidgetArea <<
                Qt::RightDockWidgetArea <<
                Qt::RightDockWidgetArea <<
                Qt::RightDockWidgetArea;
            for (int i = 0; i < Enum::TOOL_COUNT; ++i)
            {
                _p->mainWindow->addDockWidget(toolDockWidgetAreas[i], toolDockWidgets[i]);
            }
            _p->mainWindow->addDockWidget(Qt::RightDockWidgetArea, _p->annotateGroup->annotateToolDockWidget());

            // Initialize.
            if (copy)
            {
                _p->frameStoreImage = copy->_p->frameStoreImage;
            }
            _p->viewWidget->setAnnotationsVisible(_p->annotateGroup->areAnnotationsVisible());
            imageUpdate();
            playbackUpdate();

            // Setup the view callbacks.
            connect(
                _p->viewWidget,
                SIGNAL(fileDropped(const djv::Core::FileInfo &)),
                SLOT(fileOpen(const djv::Core::FileInfo &)));

            // Setup the file group callbacks.
            connect(
                _p->fileGroup.data(),
                SIGNAL(imageChanged()),
                SLOT(imageUpdate()));
            connect(
                _p->fileGroup.data(),
                SIGNAL(setFrameStore()),
                SLOT(setFrameStoreCallback()));
            connect(
                _p->fileGroup.data(),
                SIGNAL(reloadFrame()),
                SLOT(reloadFrameCallback()));
            connect(
                _p->fileGroup.data(),
                SIGNAL(exportSequence(const djv::Core::FileInfo &)),
                SLOT(exportSequenceCallback(const djv::Core::FileInfo &)));
            connect(
                _p->fileGroup.data(),
                SIGNAL(exportFrame(const djv::Core::FileInfo &)),
                SLOT(exportFrameCallback(const djv::Core::FileInfo &)));

            // Setup the image group callbacks.
            connect(
                _p->imageGroup.data(),
                SIGNAL(showFrameStoreChanged(bool)),
                SLOT(imageUpdate()));
            connect(
                _p->imageGroup.data(),
                SIGNAL(setFrameStore()),
                SLOT(setFrameStoreCallback()));
            connect(
                _p->imageGroup.data(),
                SIGNAL(displayProfileChanged(const djv::ViewLib::DisplayProfile &)),
                SLOT(imageUpdate()));
            connect(
                _p->imageGroup.data(),
                SIGNAL(redrawNeeded()),
                SLOT(imageUpdate()));

            // Setup the playback group callbacks.
            connect(
                _p->playbackGroup.data(),
                SIGNAL(playbackChanged(djv::ViewLib::Enum::PLAYBACK)),
                SLOT(playbackUpdate()));
            connect(
                _p->playbackGroup.data(),
                SIGNAL(frameChanged(qint64)),
                SLOT(imageUpdate()));

            // Setup the annotate group callbacks.
            connect(
                _p->annotateGroup.data(),
                SIGNAL(frameAnnotationsChanged(const QList<djv::ViewLib::Annotate::Data *> &)),
                _p->viewWidget,
                SLOT(setAnnotations(const QList<djv::ViewLib::Annotate::Data *> &)));
            connect(
                _p->annotateGroup.data(),
                &AnnotateGroup::currentAnnotationChanged,
                [this](Annotate::Data * value)
            {
                if (value)
                {
                    _p->playbackGroup->setFrame(value->frameIndex());
                }
            });
            connect(
                _p->annotateGroup.data(),
                SIGNAL(annotationsVisibleChanged(bool)),
                _p->viewWidget,
                SLOT(setAnnotationsVisible(bool)));

            // Setup the preferences callbacks.
            connect(
                context->UIContext::openGLPrefs(),
                SIGNAL(filterChanged(const djv::AV::OpenGLImageFilter &)),
                SLOT(imageUpdate()));
            connect(
                context->viewPrefs(),
                SIGNAL(backgroundChanged(const djv::AV::Color &)),
                SLOT(imageUpdate()));
        }

        Session::~Session()
        {}

        QPointer<FileGroup> Session::fileGroup() const
        {
            return _p->fileGroup.data();
        }

        QPointer<WindowGroup> Session::windowGroup() const
        {
            return _p->windowGroup.data();
        }

        QPointer<ViewGroup> Session::viewGroup() const
        {
            return _p->viewGroup.data();
        }

        QPointer<ImageGroup> Session::imageGroup() const
        {
            return _p->imageGroup.data();
        }

        QPointer<PlaybackGroup> Session::playbackGroup() const
        {
            return _p->playbackGroup.data();
        }

        QPointer<ToolGroup> Session::toolGroup() const
        {
            return _p->toolGroup.data();
        }

        QPointer<AnnotateGroup> Session::annotateGroup() const
        {
            return _p->annotateGroup.data();
        }

        QPointer<HelpGroup> Session::helpGroup() const
        {
            return _p->helpGroup.data();
        }

        const std::shared_ptr<AV::Image> & Session::image() const
        {
            return _p->imageGroup->isFrameStoreVisible() ? _p->frameStoreImage : _p->image;
        }

        AV::OpenGLImageOptions Session::imageOptions() const
        {
            AV::OpenGLImageOptions out;
            out.xform.mirror = _p->imageGroup->mirror();
            auto image = this->image();
            if (image)
            {
                out.xform.scale = Enum::imageScale(_p->imageGroup->scale(), image->size());
            }
            out.xform.rotate = Enum::imageRotate(_p->imageGroup->rotate());
            out.premultipliedAlpha = _p->imageGroup->hasPremultipliedAlpha();
            if (image && _p->imageGroup->hasColorProfile())
            {
                out.colorProfile = image->colorProfile;
            }
            out.displayProfile = _p->imageGroup->displayProfile();
            out.channel = _p->imageGroup->channel();
            out.background = _p->context->viewPrefs()->background();
            return out;
        }

        QPointer<MainWindow> Session::mainWindow() const
        {
            return _p->mainWindow.data();
        }

        const QPointer<ImageView> & Session::viewWidget() const
        {
            return _p->viewWidget;
        }

        void Session::close()
        {
            const int i = _sessionList.indexOf(this);
            if (i != -1)
            {
                _sessionList.remove(i);
                if (0 == _sessionList.count())
                {
                    Q_FOREACH(QWidget * widget, qApp->topLevelWidgets())
                    {
                        if (widget != _p->mainWindow.data())
                        {
                            widget->close();
                        }
                    }
                }
            }
            deleteLater();
        }

        QVector<QPointer<Session> > Session::sessionList()
        {
            return _sessionList;
        }

        void Session::fileOpen(const Core::FileInfo & fileInfo, bool init)
        {
            //DJV_DEBUG("Session::fileOpen");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            //DJV_DEBUG_PRINT("init = " << init);

            _p->image.reset();
            _p->fileGroup->open(fileInfo);
            const Core::Sequence & sequence = _p->fileGroup->ioInfo().sequence;
            //DJV_DEBUG_PRINT("sequence = " << sequence);
            _p->playbackGroup->setSequence(sequence);
            if (init)
            {
                _p->playbackGroup->setFrame(0);
                _p->playbackGroup->setPlayback(
                    (sequence.frames.count() > 1 && _p->context->playbackPrefs()->hasAutoStart()) ?
                    Enum::FORWARD :
                    Enum::STOP);
            }
            imageUpdate();
            Q_EMIT fileChanged(init);
        }

        void Session::setFileLayer(int in)
        {
            _p->fileGroup->setLayer(in);
        }

        void Session::setFileProxy(AV::PixelDataInfo::PROXY in)
        {
            _p->fileGroup->setProxy(in);
        }

        void Session::setFileCacheEnabled(bool in)
        {
            _p->fileGroup->setCacheEnabled(in);
        }

        void Session::showFullScreen()
        {
            _p->windowGroup->setFullScreen(true);
        }

        void Session::setPlayback(Enum::PLAYBACK in)
        {
            _p->playbackGroup->setPlayback(in);
        }

        void Session::setPlaybackFrame(qint64 in)
        {
            _p->playbackGroup->setFrame(in);
        }

        void Session::setPlaybackSpeed(const Core::Speed & in)
        {
            _p->playbackGroup->setSpeed(in);
        }

        void Session::reloadFrameCallback()
        {
            //DJV_DEBUG("Session::reloadFrameCallback");
            const qint64 frame = _p->playbackGroup->frame();
            //DJV_DEBUG_PRINT("frame = " << frame);
            _p->context->fileCache()->removeItem(FileCacheKey(this, frame));
        }

        void Session::exportSequenceCallback(const Core::FileInfo & in)
        {
            //DJV_DEBUG("Session::exportSequenceCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            Core::Sequence sequence;
            const Core::FrameList & frames = _p->playbackGroup->sequence().frames;
            if (frames.count())
            {
                sequence.frames = frames.mid(
                    _p->playbackGroup->inPoint(),
                    _p->playbackGroup->outPoint() - _p->playbackGroup->inPoint() + 1);
            }
            sequence.speed = _p->playbackGroup->speed();
            const FileExportInfo info(
                _p->fileGroup->fileInfo(),
                in,
                _p->fileGroup->ioInfo().layers[_p->fileGroup->layer()],
                sequence,
                _p->fileGroup->layer(),
                _p->fileGroup->proxy(),
                _p->fileGroup->hasU8Conversion(),
                _p->imageGroup->hasColorProfile(),
                imageOptions());
            _p->context->fileExport()->start(info);
        }

        void Session::exportFrameCallback(const Core::FileInfo & in)
        {
            //DJV_DEBUG("Session::exportFrameCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            Core::Sequence sequence;
            const Core::FrameList & frames = _p->playbackGroup->sequence().frames;
            if (frames.count())
            {
                sequence.frames += frames[_p->playbackGroup->frame()];
            }
            sequence.speed = _p->playbackGroup->speed();
            //DJV_DEBUG_PRINT("sequence = " << sequence);
            const FileExportInfo info(
                _p->fileGroup->fileInfo(),
                in,
                _p->fileGroup->ioInfo().layers[_p->fileGroup->layer()],
                sequence,
                _p->fileGroup->layer(),
                _p->fileGroup->proxy(),
                _p->fileGroup->hasU8Conversion(),
                _p->imageGroup->hasColorProfile(),
                imageOptions());
            _p->context->fileExport()->start(info);
        }

        void Session::setFrameStoreCallback()
        {
            //DJV_DEBUG("Session::setFrameStoreCallback");
            if (_p->image)
            {
                _p->frameStoreImage = _p->image;
            }
        }

        void Session::imageUpdate()
        {
            //DJV_DEBUG("Session::imageUpdate");

            const qint64 frame = _p->playbackGroup->frame();
            _p->image = _p->fileGroup->image(frame);
            if (_p->image)
            {
                //DJV_DEBUG_PRINT("image = " << *_p->image);
            }

            _p->fileGroup->setPreloadFrame(frame);

            Q_EMIT imageChanged(image());
            Q_EMIT imageOptionsChanged(imageOptions());
        }

        void Session::playbackUpdate()
        {
            switch (_p->playbackGroup->playback())
            {
            case Enum::FORWARD:
            case Enum::REVERSE:
                _p->fileGroup->setPreloadActive(false);
                break;
            case Enum::STOP:
                _p->fileGroup->setPreloadActive(true);
                break;
            default: break;
            }
        }

    } // namespace ViewLib
} // namespace djv
