//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/ImageViewSystem.h>

#include <djvUIComponents/ActionButton.h>

#include <djvUI/Action.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct ImageViewSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::PopupWidget> toolBarWidget;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void ImageViewSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::ImageViewSystem", context);

            DJV_PRIVATE_PTR();
            //! \todo Implement me!
            p.actions["Left"] = UI::Action::create();
            p.actions["Left"]->addShortcut(GLFW_KEY_LEFT, GLFW_MOD_SHIFT | GLFW_MOD_CONTROL);
            p.actions["Left"]->addShortcut(GLFW_KEY_KP_4);
            p.actions["Left"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["Right"] = UI::Action::create();
            p.actions["Right"]->addShortcut(GLFW_KEY_RIGHT, GLFW_MOD_SHIFT | GLFW_MOD_CONTROL);
            p.actions["Right"]->addShortcut(GLFW_KEY_KP_6);
            p.actions["Right"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["Up"] = UI::Action::create();
            p.actions["Up"]->addShortcut(GLFW_KEY_UP, GLFW_MOD_SHIFT | GLFW_MOD_CONTROL);
            p.actions["Up"]->addShortcut(GLFW_KEY_KP_8);
            p.actions["Up"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["Down"] = UI::Action::create();
            p.actions["Down"]->addShortcut(GLFW_KEY_DOWN, GLFW_MOD_SHIFT | GLFW_MOD_CONTROL);
            p.actions["Down"]->addShortcut(GLFW_KEY_KP_2);
            p.actions["Down"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["Center"] = UI::Action::create();
            p.actions["Center"]->addShortcut(GLFW_KEY_KP_5);
            p.actions["Center"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["ZoomIn"] = UI::Action::create();
			p.actions["ZoomIn"]->setIcon("djvIconZoomIn");
            p.actions["ZoomIn"]->addShortcut(GLFW_KEY_EQUAL);
            p.actions["ZoomIn"]->addShortcut(GLFW_KEY_KP_ADD);
            p.actions["ZoomIn"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["ZoomOut"] = UI::Action::create();
			p.actions["ZoomOut"]->setIcon("djvIconZoomOut");
            p.actions["ZoomOut"]->addShortcut(GLFW_KEY_MINUS);
            p.actions["ZoomOut"]->addShortcut(GLFW_KEY_KP_SUBTRACT);
            p.actions["ZoomOut"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["ResetZoom"] = UI::Action::create();
			p.actions["ResetZoom"]->setIcon("djvIconZoomReset");
            p.actions["ResetZoom"]->addShortcut(GLFW_KEY_0);
            p.actions["ResetZoom"]->addShortcut(GLFW_KEY_KP_0);
            p.actions["ResetZoom"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["ResetPosZoom"] = UI::Action::create();
            p.actions["ResetPosZoom"]->addShortcut(GLFW_KEY_DELETE);
            p.actions["ResetPosZoom"]->addShortcut(GLFW_KEY_KP_DECIMAL);
            p.actions["ResetPosZoom"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["Fit"] = UI::Action::create();
			p.actions["Fit"]->setIcon("djvIconZoomFit");
            p.actions["Fit"]->addShortcut(GLFW_KEY_BACKSPACE);
            p.actions["Fit"]->addShortcut(GLFW_KEY_KP_ENTER);
            p.actions["Fit"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["HUD"] = UI::Action::create();
            p.actions["HUD"]->setShortcut(GLFW_KEY_H);
            p.actions["HUD"]->setEnabled(false);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addWidget(UI::ActionButton::create(p.actions["Left"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["Right"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["Up"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["Down"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["Center"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["ZoomIn"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["ZoomOut"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["ResetZoom"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["ResetPosZoom"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["Fit"], context));
            vLayout->addSeparator();
            vLayout->addWidget(UI::ActionButton::create(p.actions["HUD"], context));
            p.toolBarWidget = UI::PopupWidget::create(context);
            p.toolBarWidget->setIcon("djvIconView");
            p.toolBarWidget->setWidget(vLayout);
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

        std::map<std::string, std::shared_ptr<UI::Action> > ImageViewSystem::getActions()
        {
            return _p->actions;
        }

        ToolBarWidget ImageViewSystem::getToolBarWidget()
        {
            return
            {
                _p->toolBarWidget,
                "B"
            };
        }

        void ImageViewSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Left"]->setText(_getText(DJV_TEXT("View left")));
            p.actions["Left"]->setTooltip(_getText(DJV_TEXT("Left tooltip")));
            p.actions["Right"]->setText(_getText(DJV_TEXT("View right")));
            p.actions["Right"]->setTooltip(_getText(DJV_TEXT("Right tooltip")));
            p.actions["Up"]->setText(_getText(DJV_TEXT("View up")));
            p.actions["Up"]->setTooltip(_getText(DJV_TEXT("Up tooltip")));
            p.actions["Down"]->setText(_getText(DJV_TEXT("View down")));
            p.actions["Down"]->setTooltip(_getText(DJV_TEXT("Down tooltip")));
            p.actions["Center"]->setText(_getText(DJV_TEXT("Center the view")));
            p.actions["Center"]->setTooltip(_getText(DJV_TEXT("Center tooltip")));
            p.actions["ZoomIn"]->setText(_getText(DJV_TEXT("Zoom the view in")));
            p.actions["ZoomIn"]->setTooltip(_getText(DJV_TEXT("Zoom in tooltip")));
            p.actions["ZoomOut"]->setText(_getText(DJV_TEXT("Zoom the view out")));
            p.actions["ZoomOut"]->setTooltip(_getText(DJV_TEXT("Zoom out tooltip")));
            p.actions["ResetZoom"]->setText(_getText(DJV_TEXT("Reset the zoom")));
            p.actions["ResetZoom"]->setTooltip(_getText(DJV_TEXT("Reset zoom tooltip")));
            p.actions["ResetPosZoom"]->setText(_getText(DJV_TEXT("Reset the position and zoom")));
            p.actions["ResetPosZoom"]->setTooltip(_getText(DJV_TEXT("Reset position and zoom tooltip")));
            p.actions["Fit"]->setText(_getText(DJV_TEXT("Fit the view to the image")));
            p.actions["Fit"]->setTooltip(_getText(DJV_TEXT("Fit tooltip")));
            p.actions["HUD"]->setText(_getText(DJV_TEXT("Show the HUD")));
            p.actions["HUD"]->setTooltip(_getText(DJV_TEXT("HUD tooltip")));

            p.toolBarWidget->setTooltip(_getText(DJV_TEXT("Image view system tool bar tooltip")));
        }

    } // namespace ViewLib
} // namespace djv

