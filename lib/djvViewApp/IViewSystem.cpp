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

#include <djvViewApp/ITool.h>

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
            std::shared_ptr<UI::MDI::Canvas> toolCanvas;
            std::map<std::string, std::shared_ptr<ITool> > tools;
            std::map<std::string, glm::vec2> toolsPos;
            std::map<std::string, glm::vec2> toolsSize;
            std::function<void(const std::string&)> closeToolCallback;
        };

        void IViewSystem::_init(const std::string & name, Context * context)
        {
            ISystem::_init(name, context);

            DJV_PRIVATE_PTR();

            addDependency(context->getSystemT<UI::UIComponentsSystem>());
        }
            
        IViewSystem::IViewSystem() :
            _p(new Private)
        {}

        IViewSystem::~IViewSystem()
        {}
        
        std::map<std::string, std::shared_ptr<UI::Action> > IViewSystem::getActions()
        {
            return std::map<std::string, std::shared_ptr<UI::Action> >();
        }

        MenuData IViewSystem::getMenu()
        {
            return MenuData();
        }

        std::vector<std::shared_ptr<UI::ISettingsWidget> > IViewSystem::createSettingsWidgets()
        {
            return std::vector<std::shared_ptr<UI::ISettingsWidget> >();
        }

        void IViewSystem::setToolCanvas(const std::shared_ptr<UI::MDI::Canvas>& value)
        {
            _p->toolCanvas = value;
        }

        void IViewSystem::_openTool(const std::string& name, const std::shared_ptr<ITool>& tool)
        {
            DJV_PRIVATE_PTR();
            p.toolCanvas->addChild(tool);
            p.tools[name] = tool;
            auto weak = std::weak_ptr<IViewSystem>(std::dynamic_pointer_cast<IViewSystem>(shared_from_this()));
            tool->setCloseCallback(
                [weak, name]
            {
                if (auto system = weak.lock())
                {
                    system->_closeTool(name);
                    if (system->_p->closeToolCallback)
                    {
                        system->_p->closeToolCallback(name);
                    }
                }
            });
            const auto j = p.toolsPos.find(name);
            if (j != p.toolsPos.end())
            {
                p.toolCanvas->setWidgetPos(tool, j->second);
            }
            const auto k = p.toolsSize.find(name);
            if (k != p.toolsSize.end())
            {
                tool->resize(k->second);
            }
        }

        void IViewSystem::_closeTool(const std::string& name)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.tools.find(name);
            if (i != p.tools.end())
            {
                p.toolsPos[name] = p.toolCanvas->getWidgetPos(i->second);
                p.toolsSize[name] = i->second->getSize();
                p.toolCanvas->removeChild(i->second);
                p.tools.erase(i);
            }
        }

        void IViewSystem::_setCloseToolCallback(const std::function<void(const std::string&)>& value)
        {
            _p->closeToolCallback = value;
        }

    } // namespace ViewApp
} // namespace djv
