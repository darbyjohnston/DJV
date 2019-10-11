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

#include <djvViewApp/IViewSystem.h>

#include <djvViewApp/MDIWidget.h>

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUI/MDICanvas.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct IViewSystem::Private
        {
            std::shared_ptr<UI::MDI::Canvas> canvas;
            std::map<std::string, std::shared_ptr<MDIWidget> > widgets;
            std::map<std::string, BBox2f> widgetGeom;
            std::shared_ptr<ValueObserver<bool> > textChangedObserver;
        };

        void IViewSystem::_init(const std::string & name, const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init(name, context);
            DJV_PRIVATE_PTR();

            addDependency(context->getSystemT<UI::UIComponentsSystem>());

            auto weak = std::weak_ptr<IViewSystem>(std::dynamic_pointer_cast<IViewSystem>(shared_from_this()));
            p.textChangedObserver = ValueObserver<bool>::create(
                context->getSystemT<TextSystem>()->observeTextChanged(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_textUpdate();
                        }
                    }
                });
        }
            
        IViewSystem::IViewSystem() :
            _p(new Private)
        {}

        IViewSystem::~IViewSystem()
        {}
        
        std::map<std::string, std::shared_ptr<UI::Action> > IViewSystem::getActions() const
        {
            return std::map<std::string, std::shared_ptr<UI::Action> >();
        }

        MenuData IViewSystem::getMenu() const
        {
            return MenuData();
        }

        std::vector<std::shared_ptr<UI::ISettingsWidget> > IViewSystem::createSettingsWidgets() const
        {
            return std::vector<std::shared_ptr<UI::ISettingsWidget> >();
        }

        void IViewSystem::setCanvas(const std::shared_ptr<UI::MDI::Canvas>& value)
        {
            _p->canvas = value;
        }

        void IViewSystem::_openWidget(const std::string& name, const std::shared_ptr<MDIWidget>& widget)
        {
            DJV_PRIVATE_PTR();
            p.canvas->addChild(widget);
            p.widgets[name] = widget;
            auto weak = std::weak_ptr<IViewSystem>(std::dynamic_pointer_cast<IViewSystem>(shared_from_this()));
            widget->setCloseCallback(
                [weak, name]
                {
                    if (auto system = weak.lock())
                    {
                        system->_closeWidget(name);
                    }
                });
            const auto j = p.widgetGeom.find(name);
            if (j != p.widgetGeom.end())
            {
                p.canvas->setWidgetPos(widget, j->second.min);
                widget->resize(j->second.getSize());
            }
        }

        void IViewSystem::_closeWidget(const std::string& name)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.widgets.find(name);
            if (i != p.widgets.end())
            {
                const glm::vec2& pos = p.canvas->getWidgetPos(i->second);
                const glm::vec2& size = i->second->getSize();
                p.widgetGeom[name] = BBox2f(pos.x, pos.y, size.x, size.y);
                p.canvas->removeChild(i->second);
                p.widgets.erase(i);
            }
        }

        const std::map<std::string, std::shared_ptr<MDIWidget> >& IViewSystem::_getWidgets() const
        {
            return _p->widgets;
        }

        const std::map<std::string, BBox2f>& IViewSystem::_getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void IViewSystem::_setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void IViewSystem::_textUpdate()
        {}

    } // namespace ViewApp
} // namespace djv
