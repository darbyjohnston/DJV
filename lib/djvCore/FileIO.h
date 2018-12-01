//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/String.h>

#include <memory>

namespace djv
{
    namespace Core
    {
        //! This class provides file I/O.
        class FileIO
        {
            DJV_NON_COPYABLE(FileIO);

        public:
            FileIO();
            FileIO(FileIO&&);
            ~FileIO();
            FileIO& operator = (FileIO&&);

            enum class Mode
            {
                Read,
                Write,
                ReadWrite,
                
                Count,
                First = Read
            };
            
            //! Open the file.
            //! Throws:
            //! - std::exception
            void open(const std::string& fileName, Mode);
            
            //! Open a temporary file.
            //! Throws:
            //! - std::exception
            void openTempFile(const std::string& fileName);
            
            //! Close the file.
            //! Throws:
            //! - std::exception
            void close();

            //! Get whether the file is open.
            bool isOpen() const;

            //! Get the file name.
            const std::string& getFileName() const;
            
            //! Get the file size.
            size_t getSize() const;
            
            //! \name File Position
            ///@{
            
            size_t getPos() const;
            void setPos(size_t);
            void seek(size_t);

            //! Get whether the file position is EOF.
            bool isEOF() const;

            ///@}

            //! \name File Read
            //! Throws:
            //! - std::exception
            ///@{

            void read(void*, size_t, size_t wordSize = 1);

            inline void read8(int8_t*, size_t = 1);
            inline void readU8(uint8_t*, size_t = 1);
            inline void read16(int16_t*, size_t = 1);
            inline void readU16(uint16_t*, size_t = 1);
            inline void read32(int32_t*, size_t = 1);
            inline void readU32(uint32_t*, size_t = 1);
            inline void readF32(float*, size_t = 1);

            ///@}

            //! \name File Write
            //! Throws:
            //! - std::exception
            ///@{
            
            void write(const void*, size_t, size_t wordSize = 1);
            
            inline void write8(const int8_t*, size_t);
            inline void writeU8(const uint8_t*, size_t);
            inline void write16(const int16_t*, size_t);
            inline void writeU16(const uint16_t*, size_t);
            inline void write32(const int32_t*, size_t);
            inline void writeU32(const uint32_t*, size_t);
            inline void writeF32(const float*, size_t);

            inline void write8(int8_t);
            inline void writeU8(uint8_t);
            inline void write16(int16_t);
            inline void writeU16(uint16_t);
            inline void write32(int32_t);
            inline void writeU32(uint32_t);
            inline void writeF32(float);

            inline void write(const std::string&);

            ///@}
            
            //! \name File Memory Mapping
            ///@{
            
            //! Get the current memory-map position.
            const uint8_t* mmapP() const;

            //! Get a pointer to the end of the memory-map.
            const uint8_t* mmapEnd() const;
            
            ///@}

            //! \name File Endian
            ///@{
            
            //! Get whether automatic endian conversion is performed.
            bool getEndian() const;
            
            //! Set whether automatic endian conversion is performed.
            void setEndian(bool);
            
            ///@}

            //! \name File Utilities
            ///@{

            //! Read the contents from a file.
            //! Throws:
            //! - std::exception
            static void readContents(FileIO&, std::string&);

            //! Read a word from a file.
            //! Throws:
            //! - std::exception
            static void readWord(FileIO&, char*, size_t maxLen = String::cStringLength);

            //! Read a line from a file.
            //! Throws:
            //! - std::exception
            //! \todo Should we handle comments like readWord()?
            static void readLine(FileIO&, char*, size_t maxLen = String::cStringLength);

            //! Read all the lines from a file.
            //! Throws:
            //! - std::exception
            static std::vector<std::string> readLines(const std::string& fileName);

            ///@}

        private:
            void _setPos(size_t, bool seek);

            DJV_PRIVATE();
        };

        FILE* fopen(const std::string& fileName, const std::string& mode);

    } // namespace Core
} // namespace djv

#include <djvCore/FileIOInline.h>