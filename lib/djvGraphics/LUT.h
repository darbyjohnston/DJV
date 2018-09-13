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

#include <djvGraphics/Image.h>

#include <djvCore/Error.h>

class djvFileIO;

namespace djv
{
    namespace Graphics
    {
        //! \struct LUT
        //!
        //! This struct provides LUT utilities.
        struct LUT
        {
            //! Plugin name.
            static const QString staticName;

            //! Plugin extensions.
            static const QStringList staticExtensions;

            //! This enumeration provides the file formats
            enum FORMAT
            {
                FORMAT_INFERNO,
                FORMAT_KODAK,

                FORMAT_COUNT
            };

            //! Get the file format labels.
            static const QStringList & formatLabels();

            //! This enumeration provides the file types.
            enum TYPE
            {
                TYPE_AUTO,
                TYPE_U8,
                TYPE_U10,
                TYPE_U16,

                TYPE_COUNT
            };

            //! Get the file type labels.
            static const QStringList & typeLabels();

            //! Open an Inferno LUT.
            static void infernoOpen(djvFileIO &, PixelDataInfo &, TYPE) throw (djvError);

            //! Load an Inferno LUT.
            static void infernoLoad(djvFileIO &, Image &) throw (djvError);

            //! Open a Kodak LUT.
            static void kodakOpen(djvFileIO &, PixelDataInfo &, TYPE) throw (djvError);

            //! Load a Kodak LUT.
            static void kodakLoad(djvFileIO &, Image &) throw (djvError);

            //! Open an Inferno LUT.
            static void infernoOpen(djvFileIO &, const PixelDataInfo &) throw (djvError);

            //! Save an Inferno LUT.
            static void infernoSave(djvFileIO & io, const PixelData *) throw (djvError);

            //! Open a Kodak LUT.
            static void kodakOpen(djvFileIO &, const PixelDataInfo &) throw (djvError);

            //! Save a Kodak LUT.
            static void kodakSave(djvFileIO &, const PixelData *) throw (djvError);

            //! This enumeration provides the options.
            enum OPTIONS
            {
                TYPE_OPTION,

                OPTIONS_COUNT
            };

            //! Get the option labels.
            static const QStringList & optionsLabels();

            //! This struct provides options.
            struct Options
            {
                Options();

                TYPE type;
            };
        };

    } // namespace Graphics
} // namespace djv

DJV_STRING_OPERATOR(djv::Graphics::LUT::FORMAT);
DJV_STRING_OPERATOR(djv::Graphics::LUT::TYPE);
