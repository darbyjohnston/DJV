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
#include <djvViewApp/ImageViewSettings.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Style.h>
#include <djvUI/UISystem.h>

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
            std::shared_ptr<ImageViewSettings> settings;
            
            bool currentTool = false;
            glm::vec2 hoverPos = glm::vec2(0.f, 0.f);
            glm::vec2 dragStart = glm::vec2(0.f, 0.f);
            glm::vec2 dragImagePos = glm::vec2(0.f, 0.f);

            std::shared_ptr<MediaWidget> activeWidget;
            std::shared_ptr<ValueSubject<ImageViewLock> > lock;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> lockActionGroup;
            std::shared_ptr<UI::Menu> menu;
            
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<ImageViewLock> > lockObserver;
            std::shared_ptr<ValueObserver<PointerData> > hoverObserver;
            std::shared_ptr<ValueObserver<PointerData> > dragObserver;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ImageViewSystem::_init(Context * context)
        {
            IToolSystem::_init("djv::ViewApp::ImageViewSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = ImageViewSettings::create(context);
            p.lock = ValueSubject<ImageViewLock>::create();

            p.actions["Tool"] = UI::Action::create();
            p.actions["Tool"]->setIcon("djvIconMove");
            p.actions["Tool"]->addShortcut(GLFW_KEY_1);
            p.actions["Left"] = UI::Action::create();
            p.actions["Left"]->addShortcut(GLFW_KEY_KP_4);
            p.actions["Right"] = UI::Action::create();
            p.actions["Right"]->addShortcut(GLFW_KEY_KP_6);
            p.actions["Up"] = UI::Action::create();
            p.actions["Up"]->addShortcut(GLFW_KEY_KP_8);
            p.actions["Down"] = UI::Action::create();
            p.actions["Down"]->addShortcut(GLFW_KEY_KP_2);
            p.actions["NW"] = UI::Action::create();
            p.actions["NW"]->addShortcut(GLFW_KEY_KP_7);
            p.actions["NE"] = UI::Action::create();
            p.actions["NE"]->addShortcut(GLFW_KEY_KP_9);
            p.actions["SE"] = UI::Action::create();
            p.actions["SE"]->addShortcut(GLFW_KEY_KP_3);
            p.actions["SW"] = UI::Action::create();
            p.actions["SW"]->addShortcut(GLFW_KEY_KP_1);
            p.actions["ZoomIn"] = UI::Action::create();
            p.actions["ZoomIn"]->setIcon("djvIconZoomIn");
            p.actions["ZoomIn"]->addShortcut(GLFW_KEY_EQUAL);
            p.actions["ZoomIn"]->addShortcut(GLFW_KEY_KP_ADD);
            p.actions["ZoomOut"] = UI::Action::create();
            p.actions["ZoomOut"]->setIcon("djvIconZoomOut");
            p.actions["ZoomOut"]->addShortcut(GLFW_KEY_MINUS);
            p.actions["ZoomOut"]->addShortcut(GLFW_KEY_KP_SUBTRACT);
            p.actions["Fit"] = UI::Action::create();
            p.actions["Fit"]->setIcon("djvIconZoomFit");
            p.actions["Fit"]->addShortcut(GLFW_KEY_BACKSPACE);
            p.actions["Fit"]->addShortcut(GLFW_KEY_KP_0);
            p.actions["Center"] = UI::Action::create();
            p.actions["Center"]->setIcon("djvIconZoomCenter");
            p.actions["Center"]->addShortcut(GLFW_KEY_BACKSLASH);
            p.actions["Center"]->addShortcut(GLFW_KEY_KP_5);
            p.actions["LockFit"] = UI::Action::create();
            p.actions["LockFit"]->setIcon("djvIconZoomFit");
            p.actions["LockFit"]->addShortcut(GLFW_KEY_BACKSPACE, GLFW_MOD_SHIFT);
            p.actions["LockFit"]->addShortcut(GLFW_KEY_KP_0, GLFW_MOD_SHIFT);
            p.actions["LockCenter"] = UI::Action::create();
            p.actions["LockCenter"]->setIcon("djvIconZoomCenter");
            p.actions["LockCenter"]->addShortcut(GLFW_KEY_BACKSLASH, GLFW_MOD_SHIFT);
            p.actions["LockCenter"]->addShortcut(GLFW_KEY_KP_5, GLFW_MOD_SHIFT);
            p.lockActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.lockActionGroup->addAction(p.actions["LockFit"]);
            p.lockActionGroup->addAction(p.actions["LockCenter"]);
            //! \todo Implement me!
            p.actions["Grid"] = UI::Action::create();
            p.actions["Grid"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["HUD"] = UI::Action::create();
            p.actions["HUD"]->setShortcut(GLFW_KEY_H);
            p.actions["HUD"]->setEnabled(false);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Left"]);
            p.menu->addAction(p.actions["Right"]);
            p.menu->addAction(p.actions["Up"]);
            p.menu->addAction(p.actions["Down"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["ZoomIn"]);
            p.menu->addAction(p.actions["ZoomOut"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Fit"]);
            p.menu->addAction(p.actions["LockFit"]);
            p.menu->addAction(p.actions["Center"]);
            p.menu->addAction(p.actions["LockCenter"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["Grid"]);
            p.menu->addAction(p.actions["HUD"]);

            _actionsUpdate();

            auto weak = std::weak_ptr<ImageViewSystem>(std::dynamic_pointer_cast<ImageViewSystem>(shared_from_this()));
            p.clickedObservers["Left"] = ValueObserver<bool>::create(
                p.actions["Left"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_panImage(glm::vec2(1.f, 0.f));
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
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(-1.f, 0.f));
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
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(0.f, 1.f));
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
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(0.f, -1.f));
                        }
                    }
                });
            p.clickedObservers["NW"] = ValueObserver<bool>::create(
                p.actions["NW"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(1.f, 1.f));
                        }
                    }
                });
            p.clickedObservers["NE"] = ValueObserver<bool>::create(
                p.actions["NE"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(-1.f, 1.f));
                        }
                    }
                });
            p.clickedObservers["SE"] = ValueObserver<bool>::create(
                p.actions["SE"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(-1.f, -1.f));
                        }
                    }
                });
            p.clickedObservers["SW"] = ValueObserver<bool>::create(
                p.actions["SW"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            system->_panImage(glm::vec2(1.f, -1.f));
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
                            system->_p->settings->setLock(ImageViewLock::None);
                            if (auto widget = system->_p->activeWidget)
                            {
                                auto imageView = widget->getImageView();
                                const float zoom = imageView->observeImageZoom()->get();
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
                            system->_p->settings->setLock(ImageViewLock::None);
                            if (auto widget = system->_p->activeWidget)
                            {
                                auto imageView = widget->getImageView();
                                const float zoom = imageView->observeImageZoom()->get();
                                system->_zoomImage(zoom / 2.f);
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
                            system->_p->settings->setLock(ImageViewLock::None);
                            if (auto widget = system->_p->activeWidget)
                            {
                                auto imageView = widget->getImageView();
                                imageView->imageFit();
                            }
                        }
                    }
                });
            p.clickedObservers["Center"] = ValueObserver<bool>::create(
                p.actions["Center"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->settings->setLock(ImageViewLock::None);
                            if (auto widget = system->_p->activeWidget)
                            {
                                auto imageView = widget->getImageView();
                                imageView->imageCenter();
                            }
                        }
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->hoverObserver = ValueObserver<PointerData>::create(
                                    system->_p->activeWidget->observeHover(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->hoverPos = value.pos;
                                        }
                                    });
                                system->_p->dragObserver = ValueObserver<PointerData>::create(
                                    system->_p->activeWidget->observeDrag(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            if (system->_p->currentTool)
                                            {
                                                system->_p->settings->setLock(ImageViewLock::None);
                                                auto imageView = system->_p->activeWidget->getImageView();
                                                switch (value.state)
                                                {
                                                case PointerState::Start:
                                                    system->_p->dragStart = value.pos;
                                                    system->_p->dragImagePos = imageView->observeImagePos()->get();
                                                    break;
                                                case PointerState::Move:
                                                    imageView->setImagePos(system->_p->dragImagePos + (value.pos - system->_p->dragStart));
                                                    break;
                                                default: break;
                                                }
                                            }
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->hoverObserver.reset();
                                system->_p->dragObserver.reset();
                                system->_actionsUpdate();
                            }
                        }
                    });
            }

            p.lockActionGroup->setExclusiveCallback(
                [weak](int index)
                {
                    if (auto system = weak.lock())
                    {
                        ImageViewLock lock = ImageViewLock::None;
                        switch (index)
                        {
                        case 0: lock = ImageViewLock::Fit; break;
                        case 1: lock = ImageViewLock::Center; break;
                        }
                        system->_p->settings->setLock(lock);
                    }
                });

            p.lockObserver = ValueObserver<ImageViewLock>::create(
                p.settings->observeLock(),
                [weak](ImageViewLock value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->lock->setIfChanged(value);
                        switch (value)
                        {
                        case ImageViewLock::None:
                            system->_p->lockActionGroup->setChecked(-1);
                            break;
                        case ImageViewLock::Fit:
                            system->_p->lockActionGroup->setChecked(0);
                            break;
                        case ImageViewLock::Center:
                            system->_p->lockActionGroup->setChecked(1);
                            break;
                        default: break;
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

        ToolActionData ImageViewSystem::getToolAction() const
        {
            return
            {
                _p->actions["Tool"],
                "A"
            };
        }

        void ImageViewSystem::setCurrentTool(bool value)
        {
            _p->currentTool = value;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ImageViewSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData ImageViewSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "C"
            };
        }

        void ImageViewSystem::_panImage(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = p.activeWidget)
            {
                auto uiSystem = getContext()->getSystemT<UI::UISystem>();
                auto style = uiSystem->getStyle();
                const float m = style->getMetric(UI::MetricsRole::Move);
                auto imageView = widget->getImageView();
                imageView->setImagePos(imageView->observeImagePos()->get() + value * m);
            }
        }

        void ImageViewSystem::_zoomImage(float value)
        {
            DJV_PRIVATE_PTR();
            if (auto widget = p.activeWidget)
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

        void ImageViewSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            const bool activeWidget = p.activeWidget.get();
            p.actions["Left"]->setEnabled(activeWidget);
            p.actions["Right"]->setEnabled(activeWidget);
            p.actions["Up"]->setEnabled(activeWidget);
            p.actions["Down"]->setEnabled(activeWidget);
            p.actions["NW"]->setEnabled(activeWidget);
            p.actions["NE"]->setEnabled(activeWidget);
            p.actions["SE"]->setEnabled(activeWidget);
            p.actions["SW"]->setEnabled(activeWidget);
            p.actions["ZoomIn"]->setEnabled(activeWidget);
            p.actions["ZoomOut"]->setEnabled(activeWidget);
            p.actions["Fit"]->setEnabled(activeWidget);
            p.actions["Center"]->setEnabled(activeWidget);
            p.actions["Grid"]->setEnabled(activeWidget);
            p.actions["HUD"]->setEnabled(activeWidget);
        }

        void ImageViewSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Tool"]->setText(_getText(DJV_TEXT("Pan View")));
            p.actions["Tool"]->setTooltip(_getText(DJV_TEXT("Pan view tooltip")));
            p.actions["Left"]->setText(_getText(DJV_TEXT("Left")));
            p.actions["Left"]->setTooltip(_getText(DJV_TEXT("Left tooltip")));
            p.actions["Right"]->setText(_getText(DJV_TEXT("Right")));
            p.actions["Right"]->setTooltip(_getText(DJV_TEXT("Right tooltip")));
            p.actions["Up"]->setText(_getText(DJV_TEXT("Up")));
            p.actions["Up"]->setTooltip(_getText(DJV_TEXT("Up tooltip")));
            p.actions["Down"]->setText(_getText(DJV_TEXT("Down")));
            p.actions["Down"]->setTooltip(_getText(DJV_TEXT("Down tooltip")));
            p.actions["NW"]->setText(_getText(DJV_TEXT("North West")));
            p.actions["NW"]->setTooltip(_getText(DJV_TEXT("North west tooltip")));
            p.actions["NE"]->setText(_getText(DJV_TEXT("North East")));
            p.actions["NE"]->setTooltip(_getText(DJV_TEXT("North east tooltip")));
            p.actions["SE"]->setText(_getText(DJV_TEXT("South east")));
            p.actions["SE"]->setTooltip(_getText(DJV_TEXT("South east tooltip")));
            p.actions["SW"]->setText(_getText(DJV_TEXT("South West")));
            p.actions["SW"]->setTooltip(_getText(DJV_TEXT("South west tooltip")));
            p.actions["ZoomIn"]->setText(_getText(DJV_TEXT("Zoom In")));
            p.actions["ZoomIn"]->setTooltip(_getText(DJV_TEXT("Zoom in tooltip")));
            p.actions["ZoomOut"]->setText(_getText(DJV_TEXT("Zoom Out")));
            p.actions["ZoomOut"]->setTooltip(_getText(DJV_TEXT("Zoom out tooltip")));
            p.actions["Fit"]->setText(_getText(DJV_TEXT("Fit To Image")));
            p.actions["Fit"]->setTooltip(_getText(DJV_TEXT("Fit to image tooltip")));
            p.actions["Center"]->setText(_getText(DJV_TEXT("Center")));
            p.actions["Center"]->setTooltip(_getText(DJV_TEXT("Center image tooltip")));
            p.actions["LockFit"]->setText(_getText(DJV_TEXT("Lock Fit To Image")));
            p.actions["LockFit"]->setTooltip(_getText(DJV_TEXT("Lock fit to image tooltip")));
            p.actions["LockCenter"]->setText(_getText(DJV_TEXT("Lock Center")));
            p.actions["LockCenter"]->setTooltip(_getText(DJV_TEXT("Lock center image tooltip")));
            p.actions["Grid"]->setText(_getText(DJV_TEXT("Grid")));
            p.actions["Grid"]->setTooltip(_getText(DJV_TEXT("Grid tooltip")));
            p.actions["HUD"]->setText(_getText(DJV_TEXT("HUD")));
            p.actions["HUD"]->setTooltip(_getText(DJV_TEXT("HUD tooltip")));

            p.menu->setText(_getText(DJV_TEXT("View")));
        }

    } // namespace ViewApp
} // namespace djv

