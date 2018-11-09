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

#include <djvGraphics/PNGPlugin.h>

#include <djvGraphics/PNGLoad.h>
#include <djvGraphics/PNGSave.h>

#include <djvCore/CoreContext.h>

namespace djv
{
    namespace Graphics
    {
        PNGPlugin::PNGPlugin(const QPointer<Core::CoreContext> & context) :
            ImageIO(context)
        {}

        QString PNGPlugin::pluginName() const
        {
            return PNG::staticName;
        }

        QStringList PNGPlugin::extensions() const
        {
            return QStringList() << ".png";
        }

        std::unique_ptr<ImageLoad> PNGPlugin::createLoad(const Core::FileInfo & fileInfo) const
        {
            return std::unique_ptr<ImageLoad>(new PNGLoad(fileInfo, context()));
        }

        std::unique_ptr<ImageSave> PNGPlugin::createSave(const Core::FileInfo & fileInfo, const ImageIOInfo & imageIOInfo) const
        {
            return std::unique_ptr<ImageSave>(new PNGSave(fileInfo, imageIOInfo, context()));
        }

    } // namespace Graphics
} // namespace djv
