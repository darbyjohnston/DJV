// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/String.h>

#include <memory>

#if defined(DJV_PLATFORM_WINDOWS)
#if defined(DJV_MMAP)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#else // DJV_MMAP
#include <stdio.h>
#endif // DJV_MMAP
#endif // DJV_PLATFORM_WINDOWS

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            //! This class provides file I/O.
            class FileIO
            {
                DJV_NON_COPYABLE(FileIO);

            protected:
                FileIO();

            public:
                ~FileIO();

                static std::shared_ptr<FileIO> create();

                enum class Mode
                {
                    Read,
                    Write,
                    ReadWrite,
                    Append,

                    Count,
                    First = Read
                };

                //! Open the file.
                //! Throws:
                //! - Error
                void open(const std::string& fileName, Mode);

                //! Open a temporary file.
                //! Throws:
                //! - Error
                void openTemp();

                //! Close the file.
                bool close(std::string* error = nullptr);

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

                //! Move the position relative to the current position.
                void seek(size_t);

                //! Get whether the file position is EOF.
                bool isEOF() const;

                ///@}

                //! \name Reading
                //! Throws:
                //! - Error
                ///@{

                void read(void *, size_t, size_t wordSize = 1);

                void read8(int8_t *, size_t = 1);
                void readU8(uint8_t *, size_t = 1);
                void read16(int16_t *, size_t = 1);
                void readU16(uint16_t *, size_t = 1);
                void read32(int32_t *, size_t = 1);
                void readU32(uint32_t *, size_t = 1);
                void readF32(float *, size_t = 1);

                ///@}

                //! \name Writing
                //! Throws:
                //! - Error
                ///@{

                void write(const void *, size_t, size_t wordSize = 1);

                void write8(const int8_t *, size_t);
                void writeU8(const uint8_t *, size_t);
                void write16(const int16_t *, size_t);
                void writeU16(const uint16_t *, size_t);
                void write32(const int32_t *, size_t);
                void writeU32(const uint32_t *, size_t);
                void writeF32(const float *, size_t);

                void write8(int8_t);
                void writeU8(uint8_t);
                void write16(int16_t);
                void writeU16(uint16_t);
                void write32(int32_t);
                void writeU32(uint32_t);
                void writeF32(float);

                void write(const std::string&);

                ///@}

                //! \name Memory Mapping
                ///@{

#if defined(DJV_MMAP)
                //! Get the current memory-map position.
                const uint8_t * mmapP() const;

                //! Get a pointer to the end of the memory-map.
                const uint8_t * mmapEnd() const;
#endif // DJV_MMAP

                ///@}

                //! \name Endian
                ///@{

                //! Get whether automatic endian conversion is performed.
                bool hasEndianConversion() const;

                //! Set whether automatic endian conversion is performed.
                void setEndianConversion(bool);

                ///@}

                //! \name Utilities
                ///@{

                //! Read the contents from a file.
                //! Throws:
                //! - Error
                static std::string readContents(const std::shared_ptr<FileIO>&);

                //! Read a word from a file.
                //! Throws:
                //! - Error
                static void readWord(const std::shared_ptr<FileIO>&, char *, size_t maxLen = String::cStringLength);

                //! Read a line from a file.
                //! Throws:
                //! - Error
                //! \todo Should we handle comments like readWord()?
                static void readLine(const std::shared_ptr<FileIO>&, char *, size_t maxLen = String::cStringLength);

                //! Read all the lines from a file.
                //! Throws:
                //! - Error
                static std::vector<std::string> readLines(const std::string& fileName);

                //! Write lines to a file.
                //! Throws:
                //! - Error
                static void writeLines(const std::string& fileName, const std::vector<std::string>&);

                ///@}

            private:
                void _setPos(size_t, bool seek);

                std::string     _fileName;
                Mode            _mode               = Mode::First;
                size_t          _pos                = 0;
                size_t          _size               = 0;
                bool            _endianConversion   = false;
#if defined(DJV_PLATFORM_WINDOWS)
#if defined(DJV_MMAP)
                HANDLE          _f                  = INVALID_HANDLE_VALUE;
#else // DJV_MMAP
                FILE*           _f                  = nullptr;
#endif // DJV_MMAP
#if defined(DJV_MMAP)
                void *          _mmap               = nullptr;
                const uint8_t * _mmapStart          = nullptr;
                const uint8_t * _mmapEnd            = nullptr;
                const uint8_t * _mmapP              = nullptr;
#endif // DJV_MMAP
#else // DJV_PLATFORM_WINDOWS
                int             _f                  = -1;
#if defined(DJV_MMAP)
                void *          _mmap               = reinterpret_cast<void *>(-1);
                const uint8_t * _mmapStart          = nullptr;
                const uint8_t * _mmapEnd            = nullptr;
                const uint8_t * _mmapP              = nullptr;
#endif // DJV_MMAP
#endif //DJV_PLATFORM_WINDOWS
            };

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

