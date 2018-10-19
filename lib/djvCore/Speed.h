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

#pragma once

#include <djvCore/Debug.h>
#include <djvCore/StringUtil.h>

#include <QMetaType>

namespace djv
{
    namespace Core
    {
        //! This class provides speed information.
        class Speed
        {
            Q_GADGET

        public:
            //! This enumeration provides the frames per second.
            enum FPS
            {
                FPS_1,
                FPS_3,
                FPS_6,
                FPS_12,
                FPS_15,
                FPS_16,
                FPS_18,
                FPS_23_976,
                FPS_24,
                FPS_25,
                FPS_29_97,
                FPS_30,
                FPS_50,
                FPS_59_94,
                FPS_60,
                FPS_120,

                FPS_COUNT
            };
            Q_ENUM(FPS);

            //! Get the frames per second labels.
            static const QStringList & fpsLabels();

            Speed();
            Speed(int scale, int duration = 1);
            Speed(FPS);

            //! Get the time scale.    
            int scale() const;

            //! Get the time duration.
            int duration() const;

            //! Set the frames per second.
            void set(FPS);

            //! Get whether the speed is valid.
            bool isValid() const;

            //! Convert a speed to a floating point value.
            static float speedToFloat(const Speed &);

            //! Convert a floating point value to a speed.
            static Speed floatToSpeed(float);

            //! Get the speed default.
            static FPS speedDefault();

            //! Get the global speed.
            static FPS speed();

            //! Set the global speed.
            static void setSpeed(FPS);

        private:
            int _scale = 0;
            int _duration = 0;
        };

    } // namespace Core

    DJV_COMPARISON_OPERATOR(Core::Speed);

    DJV_STRING_OPERATOR(Core::Speed::FPS);
    DJV_STRING_OPERATOR(Core::Speed);

    DJV_DEBUG_OPERATOR(Core::Speed);

} // namspace djv

Q_DECLARE_METATYPE(djv::Core::Speed)
