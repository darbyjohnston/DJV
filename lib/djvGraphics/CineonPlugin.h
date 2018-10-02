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

#include <djvGraphics/Cineon.h>
#include <djvGraphics/ImageIO.h>

#include <djvCore/FileIO.h>

namespace djv
{
    namespace Graphics
    {
        //! This plugin provides support for the Kodak Cineon image file format. Cineon
        //! is a specialized image file format for working with motion picture film.
        //!
        //! File extensions: .cin
        //!
        //! Supported features:
        //!
        //! - 10-bit RGB
        //! - Interleaved channels only
        //!
        //! References:
        //!
        //! - Kodak, "4.5 DRAFT - Image File Format Proposal for Digital Pictures"
        class CineonPlugin : public ImageIO
        {
        public:
            explicit CineonPlugin(const QPointer<Core::CoreContext> &);
            ~CineonPlugin() override;

            QString pluginName() const override;
            QStringList extensions() const override;

            QStringList option(const QString &) const override;
            bool setOption(const QString &, QStringList &) override;
            QStringList options() const override;

            void commandLine(QStringList &) override;
            QString commandLineHelp() const override;

            ImageLoad * createLoad() const override;
            ImageSave * createSave() const override;

        private:
            Cineon::Options _options;
        };

    } // namespace Graphics
} // namespace djv
