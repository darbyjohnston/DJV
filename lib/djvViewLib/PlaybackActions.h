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

#include <djvViewLib/AbstractActions.h>
#include <djvViewLib/Enum.h>

#include <QActionGroup>

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the playback group actions.
        class PlaybackActions : public AbstractActions
        {
            Q_OBJECT

        public:
            //! This enumeration provides the actions.
            enum ACTION
            {
                PLAYBACK_TOGGLE,
                EVERY_FRAME,

                ACTION_COUNT
            };
            Q_ENUM(ACTION);

            //! This enumeration provides the action groups.
            enum GROUP
            {
                PLAYBACK_GROUP,
                LOOP_GROUP,
                FRAME_GROUP,
                IN_OUT_GROUP,
                LAYOUT_GROUP,

                GROUP_COUNT
            };
            Q_ENUM(GROUP);

            explicit PlaybackActions(const QPointer<Context> &, QObject * parent = nullptr);
            ~PlaybackActions() override;

        private Q_SLOTS:
            void update();

        private:
            DJV_PRIVATE_COPY(PlaybackActions);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
