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

#include <djvUI/Core.h>

#include <djvGraphics/OpenGLImage.h>

#include <djvCore/Time.h>

#include <QObject>

#include <memory>

namespace djv
{
    namespace UI
    {
        //! This class provides miscellaneous preferences.
        class MiscPrefs : public QObject
        {
            Q_OBJECT

            //! This property holds the global time units.
            Q_PROPERTY(
                djv::Core::Time::UNITS timeUnits
                READ                   timeUnits
                WRITE                  setTimeUnits
                NOTIFY                 timeUnitsChanged)

            //! This property holds the global speed.
            Q_PROPERTY(
                djv::Core::Speed::FPS speed
                READ                  speed
                WRITE                 setSpeed
                NOTIFY                speedChanged)

            //! This property holds the maximum number of frames a sequence can hold.    
            Q_PROPERTY(
                qint64 maxFrames
                READ   maxFrames
                WRITE  setMaxFrames
                NOTIFY maxFramesChanged)

            //! This property holds the image filter.
            Q_PROPERTY(
                djv::Graphics::OpenGLImageFilter filter
                READ                             filter
                WRITE                            setFilter
                NOTIFY                           filterChanged)

            //! This property holds whether tool tips are enabled.    
            Q_PROPERTY(
                bool   toolTips
                READ   hasToolTips
                WRITE  setToolTips
                NOTIFY toolTipsChanged)

        public:
            explicit MiscPrefs(QObject * parent = nullptr);
            ~MiscPrefs();

            //! Get the global time units.
            Core::Time::UNITS timeUnits() const;

            //! Get the global speed.
            Core::Speed::FPS speed() const;

            //! Get the maximum number of frames a sequence can hold.
            qint64 maxFrames() const;

            //! Get the image filter.
            const Graphics::OpenGLImageFilter & filter() const;
            
            //! The default for whether tool tips are enabled.    
            static bool toolTipsDefault();

            //! Get whether tool tips are enabled.
            bool hasToolTips() const;

        public Q_SLOTS:
            //! Set the global time units.
            void setTimeUnits(djv::Core::Time::UNITS);

            //! Set the global speed.
            void setSpeed(djv::Core::Speed::FPS);
            
            //! Set the maximum number of frames a sequence can hold.
            void setMaxFrames(qint64);
            
            //! Set the image filter.
            void setFilter(const djv::Graphics::OpenGLImageFilter &);
            
            //! Set whether tool tips are enabled.
            void setToolTips(bool);

        Q_SIGNALS:
            //! This signal is emitted when the global time units are changed.
            void timeUnitsChanged(djv::Core::Time::UNITS);

            //! This signal is emitted when the global speed is changed.
            void speedChanged(djv::Core::Speed::FPS);
            
            //! This signal is emitted when the maximum number of frames in a sequence
            //! is changed.
            void maxFramesChanged(qint64);
            
            //! This signal is emitted when the image filter is changed.
            void filterChanged(const djv::Graphics::OpenGLImageFilter &);
            
            //! This signal is emitted when the tool tips are changed.
            void toolTipsChanged(bool);

        private:
            void toolTipsUpdate();

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
