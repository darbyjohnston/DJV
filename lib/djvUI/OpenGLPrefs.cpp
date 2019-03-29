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

#include <djvUI/OpenGLPrefs.h>

#include <djvUI/Prefs.h>

#include <djvCore/Debug.h>

#include <QApplication>

namespace djv
{
    namespace UI
    {
        struct OpenGLPrefs::Private
        {
        };

        OpenGLPrefs::OpenGLPrefs(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("OpenGLPrefs::OpenGLPrefs");
            Prefs prefs("djv::UI::OpenGLPrefs", Prefs::SYSTEM);
            AV::OpenGLImageFilter filter;
            if (prefs.get("filter", filter))
            {
                AV::OpenGLImageFilter::setFilter(filter);
            }
        }

        OpenGLPrefs::~OpenGLPrefs()
        {
            //DJV_DEBUG("OpenGLPrefs::~OpenGLPrefs");
            Prefs prefs("djv::UI::OpenGLPrefs", Prefs::SYSTEM);
            prefs.set("filter", AV::OpenGLImageFilter::filter());
        }

        const AV::OpenGLImageFilter & OpenGLPrefs::filter() const
        {
            return AV::OpenGLImageFilter::filter();
        }

        void OpenGLPrefs::setFilter(const AV::OpenGLImageFilter & filter)
        {
            if (filter == this->filter())
                return;
            AV::OpenGLImageFilter::setFilter(filter);
            Q_EMIT filterChanged(filter);
            Q_EMIT prefChanged();
        }

    } // namespace UI
} // namespace djv
