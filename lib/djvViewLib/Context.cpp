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

#include <djvViewLib/Context.h>

#include <djvViewLib/ColorPickerTool.h>
#include <djvViewLib/FileObject.h>
#include <djvViewLib/HelpObject.h>
#include <djvViewLib/HistogramTool.h>
#include <djvViewLib/ImageObject.h>
#include <djvViewLib/InfoTool.h>
#include <djvViewLib/MagnifyTool.h>
#include <djvViewLib/PlaybackObject.h>
#include <djvViewLib/Project.h>
#include <djvViewLib/ToolObject.h>
#include <djvViewLib/ViewObject.h>
#include <djvViewLib/WindowObject.h>
#include <djvViewLib/WorkspaceObject.h>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        struct Context::Private
        {
            std::vector<QPointer<QObject> > objects;
        };

        void Context::_init(int & argc, char ** argv)
        {
            UI::Context::_init(argc, argv);

            addObject(new ColorPickerTool(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new FileObject(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new HelpObject(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new HistogramTool(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new ImageObject(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new InfoTool(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new MagnifyTool(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new PlaybackObject(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new ToolObject(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new WindowObject(std::dynamic_pointer_cast<Context>(shared_from_this())));
            addObject(new ViewObject(std::dynamic_pointer_cast<Context>(shared_from_this())));
            auto workspaceObject = new WorkspaceObject(std::dynamic_pointer_cast<Context>(shared_from_this()));
            addObject(workspaceObject);

            for (auto object : getObjectsT<IViewObject>())
            {
                object->setCurrentWorkspace(workspaceObject->getCurrentWorkspace());
                object->setCurrentProject(workspaceObject->getCurrentProject());
            }

            QObject::connect(
                workspaceObject,
                &WorkspaceObject::currentWorkspaceChanged,
                [this](const QPointer<Workspace> & value)
            {
                for (auto object : getObjectsT<IViewObject>())
                {
                    object->setCurrentWorkspace(value);
                }
            });
            QObject::connect(
                workspaceObject,
                &WorkspaceObject::currentProjectChanged,
                [this](const QPointer<Project> & value)
            {
                for (auto object : getObjectsT<IViewObject>())
                {
                    object->setCurrentProject(value);
                }
            });
        }

        Context::Context() :
            _p(new Private)
        {}

        Context::~Context()
        {}

        std::shared_ptr<Context> Context::create(int & argc, char ** argv)
        {
            auto out = std::shared_ptr<Context>(new Context);
            out->_init(argc, argv);
            return out;
        }

        const std::vector<QPointer<QObject> > & Context::getObjects() const
        {
            return _p->objects;
        }

        void Context::addObject(const QPointer<QObject> & object)
        {
            _p->objects.push_back(object);
        }

    } // namespace ViewLib
} // namespace djv

