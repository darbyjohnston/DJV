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

#include <djvUI/MiscPrefs.h>

#include <djvUI/Prefs.h>

#include <djvCore/Debug.h>
#include <djvCore/Sequence.h>

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

        struct MiscPrefs::Private
        {
            Private() :
                toolTipFilter(new ToolTipFilter)
            {}

            bool toolTips = MiscPrefs::toolTipsDefault();
            QScopedPointer<ToolTipFilter> toolTipFilter;
        };

        MiscPrefs::MiscPrefs(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("MiscPrefs::MiscPrefs");
            Prefs prefs("djv::UI::MiscPrefs", Prefs::SYSTEM);
            Core::Time::UNITS timeUnits = Core::Time::units();
            if (prefs.get("timeUnits", timeUnits))
            {
                Core::Time::setUnits(timeUnits);
            }
            Core::Speed::FPS speed = Core::Speed::speed();
            if (prefs.get("speed", speed))
            {
                Core::Speed::setSpeed(speed);
            }
            qint64 maxFrames = Core::Sequence::maxFrames();
            if (prefs.get("maxFrames", maxFrames))
            {
                Core::Sequence::setMaxFrames(maxFrames);
            }
            Graphics::OpenGLImageFilter filter;
            if (prefs.get("filter", filter))
            {
                Graphics::OpenGLImageFilter::setFilter(filter);
            }
            prefs.get("toolTips", _p->toolTips);
            toolTipsUpdate();
        }

        MiscPrefs::~MiscPrefs()
        {
            //DJV_DEBUG("MiscPrefs::~MiscPrefs");
            Prefs prefs("djv::UI::MiscPrefs", Prefs::SYSTEM);
            prefs.set("timeUnits", Core::Time::units());
            prefs.set("speed", Core::Speed::speed());
            prefs.set("maxFrames", Core::Sequence::maxFrames());
            prefs.set("filter", Graphics::OpenGLImageFilter::filter());
            prefs.set("toolTips", _p->toolTips);
        }

        Core::Time::UNITS MiscPrefs::timeUnits() const
        {
            return Core::Time::units();
        }

        Core::Speed::FPS MiscPrefs::speed() const
        {
            return Core::Speed::speed();
        }

        qint64 MiscPrefs::maxFrames() const
        {
            return Core::Sequence::maxFrames();
        }

        const Graphics::OpenGLImageFilter & MiscPrefs::filter() const
        {
            return Graphics::OpenGLImageFilter::filter();
        }

        bool MiscPrefs::toolTipsDefault()
        {
            return true;
        }

        bool MiscPrefs::hasToolTips() const
        {
            return _p->toolTips;
        }

        void MiscPrefs::setTimeUnits(Core::Time::UNITS units)
        {
            if (units == this->timeUnits())
                return;
            Core::Time::setUnits(units);
            Q_EMIT timeUnitsChanged(this->timeUnits());
        }

        void MiscPrefs::setSpeed(Core::Speed::FPS speed)
        {
            if (speed == this->speed())
                return;
            Core::Speed::setSpeed(speed);
            Q_EMIT speedChanged(this->speed());
        }

        void MiscPrefs::setMaxFrames(qint64 size)
        {
            if (size == this->maxFrames())
                return;
            Core::Sequence::setMaxFrames(size);
            Q_EMIT maxFramesChanged(this->maxFrames());
        }

        void MiscPrefs::setFilter(const Graphics::OpenGLImageFilter & filter)
        {
            if (filter == this->filter())
                return;
            Graphics::OpenGLImageFilter::setFilter(filter);
            Q_EMIT filterChanged(filter);
        }

        void MiscPrefs::setToolTips(bool toolTips)
        {
            if (toolTips == _p->toolTips)
                return;
            _p->toolTips = toolTips;
            toolTipsUpdate();
            Q_EMIT toolTipsChanged(_p->toolTips);
        }

        void MiscPrefs::toolTipsUpdate()
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
