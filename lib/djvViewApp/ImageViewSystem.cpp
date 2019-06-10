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

#include <djvViewApp/ImageViewSystem.h>

#include <djvViewApp/ImageView.h>
#include <djvViewApp/MDICanvas.h>
#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/MainWindow.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageViewSystem::Private
        {
            std::weak_ptr<MDICanvas> canvas;
            std::weak_ptr<MDIWidget> widget;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            glm::vec2 hoverPos = glm::vec2(0.f, 0.f);
            glm::vec2 dragStart = glm::vec2(0.f, 0.f);
            glm::vec2 dragImagePos = glm::vec2(0.f, 0.f);
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ImageViewSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::ImageViewSystem", context);

            DJV_PRIVATE_PTR();

            //! \todo Implement me!
            p.actions["Navigation"] = UI::Action::create();
            p.actions["Navigation"]->setEnabled(false);
            p.actions["Left"] = UI::Action::create();
            p.actions["Left"]->addShortcut(GLFW_KEY_KP_4);
            p.actions["Right"] = UI::Action::create();
            p.actions["Right"]->addShortcut(GLFW_KEY_KP_6);
            p.actions["Up"] = UI::Action::create();
            p.actions["Up"]->addShortcut(GLFW_KEY_KP_8);
            p.actions["Down"] = UI::Action::create();
            p.actions["Down"]->addShortcut(GLFW_KEY_KP_2);
            p.actions["ZoomIn"] = UI::Action::create();
            p.actions["ZoomIn"]->addShortcut(GLFW_KEY_EQUAL);
            p.actions["ZoomOut"] = UI::Action::create();
            p.actions["ZoomOut"]->addShortcut(GLFW_KEY_MINUS);
            p.actions["ResetZoom"] = UI::Action::create();
            p.actions["ResetZoom"]->addShortcut(GLFW_KEY_0);
            p.actions["ResetPositionAndZoom"] = UI::Action::create();
            p.actions["ResetPositionAndZoom"]->addShortcut(GLFW_KEY_DELETE);
            p.actions["Fit"] = UI::Action::create();
            p.actions["Fit"]->setIcon("djvIconZoomFit");
            p.actions["Fit"]->addShortcut(GLFW_KEY_BACKSPACE);
            p.actions["Fit"]->addShortcut(GLFW_KEY_KP_ENTER);
            //! \todo Implement me!
            p.actions["Grid"] = UI::Action::create();
            p.actions["Grid"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["HUD"] = UI::Action::create();
            p.actions["HUD"]->setShortcut(GLFW_KEY_H);
            p.actions["HUD"]->setEnabled(false);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Navigation"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Left"]);
            p.menu->addAction(p.actions["Right"]);
            p.menu->addAction(p.actions["Up"]);
            p.menu->addAction(p.actions["Down"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["ZoomIn"]);
            p.menu->addAction(p.actions["ZoomOut"]);
            p.menu->addAction(p.actions["ResetZoom"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["ResetPositionAndZoom"]);
            p.menu->addAction(p.actions["Fit"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Grid"]);
            p.menu->addAction(p.actions["HUD"]);

            auto weak = std::weak_ptr<ImageViewSystem>(std::dynamic_pointer_cast<ImageViewSystem>(shared_from_this()));
            p.clickedObservers["Left"] = ValueObserver<bool>::create(
                p.actions["Left"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_moveImage(glm::vec2(-10.f, 0.f));
                        }
                    }
                });
            p.clickedObservers["Right"] = ValueObserver<bool>::create(
                p.actions["Right"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_moveImage(glm::vec2(10.f, 0.f));
                        }
                    }
                });
            p.clickedObservers["Up"] = ValueObserver<bool>::create(
                p.actions["Up"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_moveImage(glm::vec2(0.f, -10.f));
                        }
                    }
                });
            p.clickedObservers["Down"] = ValueObserver<bool>::create(
                p.actions["Down"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_moveImage(glm::vec2(0.f, 10.f));
                        }
                    }
                });

            p.clickedObservers["ZoomIn"] = ValueObserver<bool>::create(
                p.actions["ZoomIn"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto widget = system->_p->widget.lock())
                            {
                                auto imageView = widget->getImageView();
                                const float zoom = imageView->getImageZoom();
                                system->_zoomImage(zoom * 2.f);
                            }
                        }
                    }
                });
            p.clickedObservers["ZoomOut"] = ValueObserver<bool>::create(
                p.actions["ZoomOut"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto widget = system->_p->widget.lock())
                            {
                                auto imageView = widget->getImageView();
                                const float zoom = imageView->getImageZoom();
                                system->_zoomImage(zoom / 2.f);
                            }
                        }
                    }
                });
            p.clickedObservers["ResetZoom"] = ValueObserver<bool>::create(
                p.actions["ResetZoom"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto widget = system->_p->widget.lock())
                            {
                                auto imageView = widget->getImageView();
                                imageView->setImageZoom(1.f);
                            }
                        }
                    }
                });
            p.clickedObservers["ResetPositionAndZoom"] = ValueObserver<bool>::create(
                p.actions["ResetPositionAndZoom"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto widget = system->_p->widget.lock())
                            {
                                auto imageView = widget->getImageView();
                                imageView->setImagePosAndZoom(glm::vec2(0.f, 0.f), 1.f);
                            }
                        }
                    }
                });

            p.clickedObservers["Fit"] = ValueObserver<bool>::create(
                p.actions["Fit"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto widget = system->_p->widget.lock())
                            {
                                auto imageView = widget->getImageView();
                                const AV::Image::Size& imageSize = imageView->getImageSize();
                                const BBox2f& g = imageView->getGeometry();
                                float zoom = g.w() / static_cast<float>(imageSize.w);
                                if (zoom * imageSize.h > g.h())
                                {
                                    zoom = g.h() / static_cast<float>(imageSize.h);
                                }
                                imageView->setImagePosAndZoom(
                                    glm::vec2(
                                        g.w() / 2.f - ((imageSize.w * zoom) / 2.f),
                                        g.h() / 2.f - ((imageSize.h * zoom) / 2.f)),
                                    zoom);
                            }
                        }
                    }
                });

            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        ImageViewSystem::ImageViewSystem() :
            _p(new Private)
        {}

        ImageViewSystem::~ImageViewSystem()
        {}

        std::shared_ptr<ImageViewSystem> ImageViewSystem::create(Context * context)
        {
            auto out = std::shared_ptr<ImageViewSystem>(new ImageViewSystem);
            out->_init(context);
            return out;
        }

        void ImageViewSystem::setMDICanvas(const std::shared_ptr<MDICanvas>& value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = p.widget.lock())
            {
                widget->setHoverCallback(nullptr);
                widget->setDragCallback(nullptr);
            }
            if (auto canvas = p.canvas.lock())
            {
                canvas->setActiveCallback(nullptr);
            }
            p.canvas = value;
            p.widget.reset();
            auto weak = std::weak_ptr<ImageViewSystem>(std::dynamic_pointer_cast<ImageViewSystem>(shared_from_this()));
            if (auto canvas = p.canvas.lock())
            {
                p.widget = canvas->getActiveWidget();
                canvas->setActiveCallback(
                    [weak](const std::shared_ptr<MDIWidget>& value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->widget = value;
                        system->_initWidget();
                    }
                });
            }
            if (auto widget = p.widget.lock())
            {
                _initWidget();
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ImageViewSystem::getActions()
        {
            return _p->actions;
        }

        MenuData ImageViewSystem::getMenu()
        {
            return
            {
                _p->menu,
                "C"
            };
        }

        void ImageViewSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Navigation"]->setText(_getText(DJV_TEXT("Navigation")));
            p.actions["Navigation"]->setTooltip(_getText(DJV_TEXT("Navigation tooltip")));
            p.actions["Left"]->setText(_getText(DJV_TEXT("Left")));
            p.actions["Left"]->setTooltip(_getText(DJV_TEXT("Left tooltip")));
            p.actions["Right"]->setText(_getText(DJV_TEXT("Right")));
            p.actions["Right"]->setTooltip(_getText(DJV_TEXT("Right tooltip")));
            p.actions["Up"]->setText(_getText(DJV_TEXT("Up")));
            p.actions["Up"]->setTooltip(_getText(DJV_TEXT("Up tooltip")));
            p.actions["Down"]->setText(_getText(DJV_TEXT("Down")));
            p.actions["Down"]->setTooltip(_getText(DJV_TEXT("Down tooltip")));
            p.actions["ZoomIn"]->setText(_getText(DJV_TEXT("Zoom In")));
            p.actions["ZoomIn"]->setTooltip(_getText(DJV_TEXT("Zoom in tooltip")));
            p.actions["ZoomOut"]->setText(_getText(DJV_TEXT("Zoom Out")));
            p.actions["ZoomOut"]->setTooltip(_getText(DJV_TEXT("Zoom out tooltip")));
            p.actions["ResetZoom"]->setText(_getText(DJV_TEXT("Reset Zoom")));
            p.actions["ResetZoom"]->setTooltip(_getText(DJV_TEXT("Reset zoom tooltip")));
            p.actions["ResetPositionAndZoom"]->setText(_getText(DJV_TEXT("Reset Position and Zoom")));
            p.actions["ResetPositionAndZoom"]->setTooltip(_getText(DJV_TEXT("Reset poisiton and zoom tooltip")));
            p.actions["Fit"]->setText(_getText(DJV_TEXT("Fit To Image")));
            p.actions["Fit"]->setTooltip(_getText(DJV_TEXT("Fit to image tooltip")));
            p.actions["Grid"]->setText(_getText(DJV_TEXT("Grid")));
            p.actions["Grid"]->setTooltip(_getText(DJV_TEXT("Grid tooltip")));
            p.actions["HUD"]->setText(_getText(DJV_TEXT("HUD")));
            p.actions["HUD"]->setTooltip(_getText(DJV_TEXT("HUD tooltip")));

            p.menu->setText(_getText(DJV_TEXT("View")));
        }

        void ImageViewSystem::_initWidget()
        {
            DJV_PRIVATE_PTR();
            if (auto widget = p.widget.lock())
            {
                auto weak = std::weak_ptr<ImageViewSystem>(std::dynamic_pointer_cast<ImageViewSystem>(shared_from_this()));
                widget->setHoverCallback(
                    [weak](Hover hover, const glm::vec2& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->hoverPos = value;
                        }
                    });
                widget->setDragCallback(
                    [weak](Drag drag, const glm::vec2& value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto widget = system->_p->widget.lock())
                            {
                                auto imageView = widget->getImageView();
                                switch (drag)
                                {
                                case Drag::Start:
                                    system->_p->dragStart = value;
                                    system->_p->dragImagePos = imageView->getImagePos();
                                    break;
                                case Drag::Move:
                                    imageView->setImagePos(system->_p->dragImagePos + (value - system->_p->dragStart));
                                    break;
                                default: break;
                                }
                            }
                        }
                    });
            }
        }

        void ImageViewSystem::_moveImage(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = p.widget.lock())
            {
                auto imageView = widget->getImageView();
                imageView->setImagePos(imageView->getImagePos() + value);
            }
        }

        void ImageViewSystem::_zoomImage(float value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = p.widget.lock())
            {
                auto imageView = widget->getImageView();
                const float w = imageView->getWidth();
                const float h = imageView->getHeight();
                glm::vec2 focus = glm::vec2(0.f, 0.f);
                if (BBox2f(0.f, 0.f, w, h).contains(p.hoverPos))
                {
                    focus = p.hoverPos;
                }
                else
                {
                    focus.x = w / 2.f;
                    focus.y = h / 2.f;
                }
                imageView->setImageZoomFocus(value, focus);
            }
        }

    } // namespace ViewApp
} // namespace djv

