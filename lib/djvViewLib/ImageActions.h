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

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the image group actions.
        class ImageActions : public AbstractActions
        {
            Q_OBJECT
            Q_ENUMS(ACTION)
            Q_ENUMS(GROUP)

        public:
            //! This enumeration provides the actions.
            enum ACTION
            {
                SHOW_FRAME_STORE,
                SET_FRAME_STORE,
                MIRROR_H,
                MIRROR_V,
                COLOR_PROFILE,
                DISPLAY_PROFILE_VISIBLE,

                ACTION_COUNT
            };

            //! This enumeration provides the action groups.
            enum GROUP
            {
                SCALE_GROUP,
                ROTATE_GROUP,
                DISPLAY_PROFILE_GROUP,
                CHANNEL_GROUP,

                GROUP_COUNT
            };

            explicit ImageActions(const QPointer<Context> &, QObject * parent = nullptr);
            ~ImageActions() override;

        private Q_SLOTS:
            void update();

        private:
            DJV_PRIVATE_COPY(ImageActions);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
