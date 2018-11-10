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

#include <djvAV/IO.h>
#include <djvAV/OpenEXR.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileInfo.h>

#include <ImfInputFile.h>

namespace djv
{
    namespace AV
    {
        //! This class provides a memory-mapped input stream.
        class MemoryMappedIStream : public Imf::IStream
        {
        public:
            MemoryMappedIStream(const char fileName[]);
            ~MemoryMappedIStream() override;

            bool isMemoryMapped() const override;
            char * readMemoryMapped(int n) override;
            bool read(char c[], int n) override;
            Imf::Int64 tellg() override;
            void seekg(Imf::Int64 pos) override;

        private:
            Core::FileIO _f;
            quint64      _size = 0;
            quint64      _pos  = 0;
            char *       _p    = nullptr;
        };

        class OpenEXRLoad : public Load
        {
        public:
            OpenEXRLoad(const Core::FileInfo &, const OpenEXR::Options &, const QPointer<Core::CoreContext> &);
            ~OpenEXRLoad() override;

            void read(Image &, const ImageIOInfo &) override;

        private:
            void _open(const QString &, IOInfo &);
            void _close();

            OpenEXR::Options                     _options;
            std::unique_ptr<MemoryMappedIStream> _s;
            std::unique_ptr<Imf::InputFile>      _f;
            Core::Box2i                          _displayWindow;
            Core::Box2i                          _dataWindow;
            Core::Box2i                          _intersectedWindow;
            std::vector<OpenEXR::Layer>          _layers;
            PixelData                            _tmp;
            bool                                 _fast = false;
        };

    } // namespace AV
} // namespace djv
