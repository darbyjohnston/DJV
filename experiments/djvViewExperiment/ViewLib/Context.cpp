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

#include <ViewLib/Context.h>

#include <ViewLib/ColorPickerTool.h>
#include <ViewLib/FileSystem.h>
#include <ViewLib/HelpSystem.h>
#include <ViewLib/HistogramTool.h>
#include <ViewLib/ImageSystem.h>
#include <ViewLib/InfoTool.h>
#include <ViewLib/MagnifyTool.h>
#include <ViewLib/PlaybackSystem.h>
#include <ViewLib/Project.h>
#include <ViewLib/ToolSystem.h>
#include <ViewLib/ViewSystem.h>
#include <ViewLib/WindowSystem.h>
#include <ViewLib/WorkspaceSystem.h>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        struct Context::Private
        {
        };

        Context::Context(int & argc, char ** argv) :
            UI::Context(argc, argv),
            _p(new Private)
        {
            addSystem(new ColorPickerTool(this));
            addSystem(new FileSystem(this));
            addSystem(new HelpSystem(this));
            addSystem(new HistogramTool(this));
            addSystem(new ImageSystem(this));
            addSystem(new InfoTool(this));
            addSystem(new MagnifyTool(this));
            addSystem(new PlaybackSystem(this));
            addSystem(new ToolSystem(this));
            addSystem(new WindowSystem(this));
            addSystem(new ViewSystem(this));
            auto workspaceSystem = new WorkspaceSystem(this);
            addSystem(workspaceSystem);

            for (auto system : getSystemsT<IViewSystem>())
            {
                system->setCurrentWorkspace(workspaceSystem->getCurrentWorkspace());
                system->setCurrentProject(workspaceSystem->getCurrentProject());
            }

            connect(
                workspaceSystem,
                &WorkspaceSystem::currentWorkspaceChanged,
                [this](const QPointer<Workspace> & value)
            {
                for (auto system : getSystemsT<IViewSystem>())
                {
                    system->setCurrentWorkspace(value);
                }
            });
            connect(
                workspaceSystem,
                &WorkspaceSystem::currentProjectChanged,
                [this](const QPointer<Project> & value)
            {
                for (auto system : getSystemsT<IViewSystem>())
                {
                    system->setCurrentProject(value);
                }
            });
        }

        Context::~Context()
        {}

    } // namespace ViewLib
} // namespace djv

