//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <Project.h>

#include <Context.h>

#include <iostream>

namespace djv
{
    namespace ViewExperiment
    {
        namespace
        {
            size_t projectCount = 0;

        } // namespace

        struct Project::Private
        {
            QFileInfo fileInfo;
            bool changes = false;
        };
        
        Project::Project(const QPointer<Context> &, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            ++projectCount;
            _p->fileInfo = QString("Untitled %1").arg(projectCount);
            //std::cout << "Project::Project(" << _p->fileInfo.fileName().toLatin1().data() << ")" << std::endl;
        }
        
        Project::~Project()
        {
            //std::cout << "Project::~Project(" << _p->fileInfo.fileName().toLatin1().data() << ")" << std::endl;
        }

        const QFileInfo & Project::getFileInfo() const
        {
            return _p->fileInfo;
        }

        bool Project::hasChanges() const
        {
            return _p->changes;
        }
            
        void Project::open(const QFileInfo & fileInfo)
        {
            _p->fileInfo = fileInfo;
            Q_EMIT fileInfoChanged(_p->fileInfo);
        }
        
        void Project::close()
        {
            _p->fileInfo = QFileInfo();
            Q_EMIT fileInfoChanged(_p->fileInfo);
        }
        
        void Project::save()
        {}
        
        void Project::saveAs(const QFileInfo & fileInfo)
        {
            _p->fileInfo = fileInfo;
            Q_EMIT fileInfoChanged(_p->fileInfo);
        }

    } // namespace ViewExperiment
} // namespace djv

