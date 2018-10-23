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

#include <djvViewLib/DisplayProfile.h>

#include <djvCore/Util.h>

#include <QWidget>

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        class ImageView;
        class ViewContext;

        //! This class provides a display profile widget.
        class DisplayProfileWidget : public QWidget
        {
            Q_OBJECT

        public:
            explicit DisplayProfileWidget(
                const QPointer<ImageView> &,
                const QPointer<ViewContext> &,
                QWidget * parent = nullptr);
            ~DisplayProfileWidget() override;

            //! Get the display profile.
            const DisplayProfile & displayProfile() const;

        public Q_SLOTS:
            //! Set the display profile.    
            void setDisplayProfile(const djv::ViewLib::DisplayProfile &);

        Q_SIGNALS:
            //! This signal is emitted when the display profile is changed.
            void displayProfileChanged(const djv::ViewLib::DisplayProfile &);

        protected:
            bool event(QEvent *) override;

        private Q_SLOTS:
            void lutCallback(const djv::Core::FileInfo &);
            void brightnessCallback(float);
            void contrastCallback(float);
            void saturationCallback(float);
            void levelsIn0Callback(float);
            void levelsIn1Callback(float);
            void gammaCallback(float);
            void levelsOut0Callback(float);
            void levelsOut1Callback(float);
            void softClipCallback(float);
            void addCallback();
            void resetCallback();

            void styleUpdate();
            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(DisplayProfileWidget);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
