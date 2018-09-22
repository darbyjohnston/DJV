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

#include <djvViewLib/InputPrefs.h>

#include <djvUI/Prefs.h>

namespace djv
{
    namespace ViewLib
    {
        InputPrefs::InputPrefs(Context * context, QObject * parent) :
            AbstractPrefs(context, parent),
            _mouseWheel(mouseWheelDefault()),
            _mouseWheelShift(mouseWheelShiftDefault()),
            _mouseWheelCtrl(mouseWheelCtrlDefault())
        {
            //DJV_DEBUG("InputPrefs::InputPrefs");
            UI::Prefs prefs("djv::ViewLib::InputPrefs");
            prefs.get("mouseWheel", _mouseWheel);
            prefs.get("mouseWheelShift", _mouseWheelShift);
            prefs.get("mouseWheelCtrl", _mouseWheelCtrl);
        }

        InputPrefs::~InputPrefs()
        {
            //DJV_DEBUG("InputPrefs::~InputPrefs");
            UI::Prefs prefs("djv::ViewLib::InputPrefs");
            prefs.set("mouseWheel", _mouseWheel);
            prefs.set("mouseWheelShift", _mouseWheelShift);
            prefs.set("mouseWheelCtrl", _mouseWheelCtrl);
        }

        Util::MOUSE_WHEEL InputPrefs::mouseWheelDefault()
        {
            return Util::MOUSE_WHEEL_VIEW_ZOOM;
        }

        Util::MOUSE_WHEEL InputPrefs::mouseWheel() const
        {
            return _mouseWheel;
        }

        Util::MOUSE_WHEEL InputPrefs::mouseWheelShiftDefault()
        {
            return Util::MOUSE_WHEEL_PLAYBACK_SHUTTLE;
        }

        Util::MOUSE_WHEEL InputPrefs::mouseWheelShift() const
        {
            return _mouseWheelShift;
        }

        Util::MOUSE_WHEEL InputPrefs::mouseWheelCtrlDefault()
        {
            return Util::MOUSE_WHEEL_PLAYBACK_SPEED;
        }

        Util::MOUSE_WHEEL InputPrefs::mouseWheelCtrl() const
        {
            return _mouseWheelCtrl;
        }

        void InputPrefs::setMouseWheel(Util::MOUSE_WHEEL in)
        {
            if (in == _mouseWheel)
                return;
            _mouseWheel = in;
            Q_EMIT prefChanged();
        }

        void InputPrefs::setMouseWheelShift(Util::MOUSE_WHEEL in)
        {
            if (in == _mouseWheelShift)
                return;
            _mouseWheelShift = in;
            Q_EMIT prefChanged();
        }

        void InputPrefs::setMouseWheelCtrl(Util::MOUSE_WHEEL in)
        {
            if (in == _mouseWheelCtrl)
                return;
            _mouseWheelCtrl = in;
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
