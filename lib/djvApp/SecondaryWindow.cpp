// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/SecondaryWindow.h>

#include <djvApp/Models/ColorModel.h>
#include <djvApp/Models/FilesModel.h>
#include <djvApp/Models/ViewportModel.h>
#include <djvApp/App.h>

#include <tlTimelineUI/Viewport.h>

namespace djv
{
    namespace app
    {
        struct SecondaryWindow::Private
        {
            std::shared_ptr<tl::timelineui::Viewport> viewport;

            std::shared_ptr<feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::CompareOptions> > compareOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::OCIOOptions> > ocioOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::LUTOptions> > lutOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<feather_tk::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::BackgroundOptions> > backgroundOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<feather_tk::ImageType> > colorBufferObserver;
        };

        void SecondaryWindow::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<feather_tk::Window>& shared)
        {
            Window::_init(context, "djv 2", feather_tk::Size2I(1920, 1080));
            FEATHER_TK_P();

            p.viewport = tl::timelineui::Viewport::create(context);
            p.viewport->setParent(shared_from_this());

            p.playerObserver = feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& value)
                {
                    _p->viewport->setPlayer(value);
                });

            p.compareOptionsObserver = feather_tk::ValueObserver<tl::timeline::CompareOptions>::create(
                app->getFilesModel()->observeCompareOptions(),
                [this](const tl::timeline::CompareOptions& value)
                {
                    _p->viewport->setCompareOptions(value);
                });

            p.ocioOptionsObserver = feather_tk::ValueObserver<tl::timeline::OCIOOptions>::create(
                app->getColorModel()->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                    _p->viewport->setOCIOOptions(value);
                });

            p.lutOptionsObserver = feather_tk::ValueObserver<tl::timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                    _p->viewport->setLUTOptions(value);
                });

            p.imageOptionsObserver = feather_tk::ValueObserver<feather_tk::ImageOptions>::create(
                app->getViewportModel()->observeImageOptions(),
                [this](const feather_tk::ImageOptions& value)
                {
                    _p->viewport->setImageOptions({ value });
                });

            p.displayOptionsObserver = feather_tk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _p->viewport->setDisplayOptions({ value });
                });

            p.backgroundOptionsObserver = feather_tk::ValueObserver<tl::timeline::BackgroundOptions>::create(
                app->getViewportModel()->observeBackgroundOptions(),
                [this](const tl::timeline::BackgroundOptions& value)
                {
                    _p->viewport->setBackgroundOptions(value);
                });

            p.colorBufferObserver = feather_tk::ValueObserver<feather_tk::ImageType>::create(
                app->getViewportModel()->observeColorBuffer(),
                [this](feather_tk::ImageType value)
                {
                    _p->viewport->setColorBuffer(value);
                });
        }

        SecondaryWindow::SecondaryWindow() :
            _p(new Private)
        {}

        SecondaryWindow::~SecondaryWindow()
        {
            _makeCurrent();
            _p->viewport->setParent(nullptr);
        }

        std::shared_ptr<SecondaryWindow> SecondaryWindow::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<feather_tk::Window>& shared)
        {
            auto out = std::shared_ptr<SecondaryWindow>(new SecondaryWindow);
            out->_init(context, app, shared);
            return out;
        }

        const std::shared_ptr<tl::timelineui::Viewport>& SecondaryWindow::getViewport() const
        {
            return _p->viewport;
        }

        void SecondaryWindow::setView(
            const feather_tk::V2I& pos,
            double zoom,
            bool frame)
        {
            FEATHER_TK_P();
            p.viewport->setViewPosAndZoom(pos, zoom);
            p.viewport->setFrameView(frame);
        }
    }
}
