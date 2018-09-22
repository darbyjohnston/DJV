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

#include <djvUI/HelpPrefs.h>

#include <djvUI/Prefs.h>

#include <djvCore/Debug.h>

#include <QApplication>

namespace djv
{
    namespace UI
    {
        namespace
        {
            class ToolTipFilter : public QObject
            {
            protected:
                bool eventFilter(QObject * object, QEvent * event)
                {
                    if (event->type() == QEvent::ToolTip)
                        return true;
                    return QObject::eventFilter(object, event);
                }
            };

        } // namespace

        struct HelpPrefs::Private
        {
            Private() :
                toolTipFilter(new ToolTipFilter)
            {}

            bool                          toolTips = HelpPrefs::toolTipsDefault();
            QScopedPointer<ToolTipFilter> toolTipFilter;
        };

        HelpPrefs::HelpPrefs(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("HelpPrefs::HelpPrefs");
            Prefs prefs("djv::UI::HelpPrefs", Prefs::SYSTEM);
            prefs.get("toolTips", _p->toolTips);
            toolTipsUpdate();
        }

        HelpPrefs::~HelpPrefs()
        {
            //DJV_DEBUG("HelpPrefs::~HelpPrefs");
            Prefs prefs("djv::UI::HelpPrefs", Prefs::SYSTEM);
            prefs.set("toolTips", _p->toolTips);
        }

        bool HelpPrefs::toolTipsDefault()
        {
            return true;
        }

        bool HelpPrefs::hasToolTips() const
        {
            return _p->toolTips;
        }

        void HelpPrefs::setToolTips(bool toolTips)
        {
            if (toolTips == _p->toolTips)
                return;
            _p->toolTips = toolTips;
            toolTipsUpdate();
            Q_EMIT toolTipsChanged(_p->toolTips);
        }

        void HelpPrefs::toolTipsUpdate()
        {
            if (!_p->toolTips)
            {
                qApp->installEventFilter(_p->toolTipFilter.data());
            }
            else
            {
                qApp->removeEventFilter(_p->toolTipFilter.data());
            }
        }

    } // namespace UI
} // namespace djv
