//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvGraphics/ImageIO.h>
#include <djvGraphics/TIFF.h>

namespace djv
{
    namespace Graphics
    {
        //! \class TIFFPlugin
        //!
        //! This plugin provides support for the Tagged Image File Format (TIFF).
        //!
        //! Requires:
        //!
        //! - libtiff - http://www.libtiff.org
        //!
        //! File extensions: .tiff, .tif
        //!
        //! Supported features:
        //!
        //! - 8-bit, 16-bit, 32-bit float, Luminance, Luminance Alpha, RGB, RGBA
        //! - Interleaved channels only
        //! - File compression
        class TIFFPlugin : public ImageIO
        {
        public:
            explicit TIFFPlugin(Core::CoreContext *);

            void initPlugin() override;
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
            TIFF::Options _options;
        };

    } // namespace Graphics
} // namespace djv
