// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/BackgroundImageWidget.h>

#include <djvViewApp/WindowSettings.h>

#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#include <djvAV/IOSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TextSystem.h>
#include <djvSystem/TimerFunc.h>

#include <djvCore/StringFormat.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct BackgroundImageWidget::Private
        {
            std::shared_ptr<Image::Image> image;
            bool scale = false;
            bool colorize = false;
            std::shared_ptr<AV::IO::IRead> read;
            std::shared_ptr<System::Timer> timer;
            std::shared_ptr<ValueObserver<std::string> > imageObserver;
            std::shared_ptr<ValueObserver<bool> > scaleObserver;
            std::shared_ptr<ValueObserver<bool> > colorizeObserver;
        };

        void BackgroundImageWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::BackgroundImageWidget");

            p.timer = System::Timer::create(context);
            p.timer->setRepeating(true);

            auto weak = std::weak_ptr<BackgroundImageWidget>(std::dynamic_pointer_cast<BackgroundImageWidget>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
            {
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.imageObserver = ValueObserver<std::string>::create(
                    windowSettings->observeBackgroundImage(),
                    [weak, contextWeak](const std::string& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->image.reset();
                                widget->_p->read.reset();
                                widget->_p->timer->stop();
                                if (!value.empty())
                                {
                                    try
                                    {
                                        auto io = context->getSystemT<AV::IO::IOSystem>();
                                        widget->_p->read = io->read(value);
                                        widget->_p->timer->start(
                                            System::getTimerDuration(System::TimerValue::Fast),
                                            [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                                            {
                                                if (auto widget = weak.lock())
                                                {
                                                    std::shared_ptr<Image::Image> image;
                                                    {
                                                        std::unique_lock<std::mutex> lock(widget->_p->read->getMutex());
                                                        auto& queue = widget->_p->read->getVideoQueue();
                                                        if (!queue.isEmpty())
                                                        {
                                                            image = queue.popFrame().image;
                                                        }
                                                    }
                                                    if (image)
                                                    {
                                                        widget->_p->image = image;
                                                        widget->_p->read.reset();
                                                        widget->_p->timer->stop();
                                                    }
                                                }
                                            });
                                    }
                                    catch (const std::exception& e)
                                    {
                                        auto logSystem = context->getSystemT<System::LogSystem>();
                                        logSystem->log("djv::ViewApp::BackgroundImageWidget", e.what(), System::LogLevel::Error);
                                    }
                                }
                            }
                        }
                    });

                p.scaleObserver = ValueObserver<bool>::create(
                    windowSettings->observeBackgroundImageScale(),
                    [weak, contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->scale = value;
                                widget->_redraw();
                            }
                        }
                    });

                p.colorizeObserver = ValueObserver<bool>::create(
                    windowSettings->observeBackgroundImageColorize(),
                    [weak, contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->colorize = value;
                                widget->_redraw();
                            }
                        }
                    });
            }
        }

        BackgroundImageWidget::BackgroundImageWidget() :
            _p(new Private)
        {}

        BackgroundImageWidget::~BackgroundImageWidget()
        {}

        std::shared_ptr<BackgroundImageWidget> BackgroundImageWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<BackgroundImageWidget>(new BackgroundImageWidget);
            out->_init(context);
            return out;
        }

        void BackgroundImageWidget::_paintEvent(System::Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const auto& style = _getStyle();
                const auto& render = _getRender();
                const Math::BBox2f& g = getGeometry();
                const Image::Size& size = p.image->getSize();
                glm::vec2 c(g.min.x + size.w / 2.F, g.min.y + size.h / 2.F);
                float zoom = 1.F;
                if (p.scale)
                {
                    zoom = g.w() / static_cast<float>(size.w);
                    if (zoom * size.h > g.h())
                    {
                        zoom = g.h() / static_cast<float>(size.h);
                    }
                    c.x *= zoom;
                    c.y *= zoom;
                }
                glm::mat3x3 m(1.F);
                const glm::vec2 pos(
                    floorf(g.w() / 2.F - c.x),
                    floorf(g.h() / 2.F - c.y));
                m = glm::translate(m, g.min + pos);
                m = glm::scale(m, glm::vec2(zoom, zoom));
                render->pushTransform(m);
                if (p.colorize)
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Button));
                    render->drawFilledImage(p.image, glm::vec2(0.F, 0.F));
                }
                else
                {
                    render->setFillColor(Image::Color(1.F, 1.F, 1.F));
                    render->drawImage(p.image, glm::vec2(0.F, 0.F));
                }
                render->popTransform();
            }
        }

    } // namespace ViewApp
} // namespace djv

