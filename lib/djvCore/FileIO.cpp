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

#include <djvCore/FileIO.h>

namespace djv
{
    namespace Core
    {
        FileIO::FileIO(FileIO&& other) :
            _f(other._f),
            _fileName(other._fileName),
            _mode(other._mode),
            _pos(other._pos),
            _size(other._size),
            _endian(other._endian),
            _mmap(other._mmap),
            _mmapStart(other._mmapStart),
            _mmapEnd(other._mmapEnd),
            _mmapP(other._mmapP)
        {
#if defined(DJV_PLATFORM_WINDOWS)
            other._f = INVALID_HANDLE_VALUE;
            other._fileName = std::string();
            other._mode = Mode::First;
            other._pos = 0;
            other._size = 0;
            other._endian = false;
            other._mmap = nullptr;
            other._mmapStart = nullptr;
            other._mmapEnd = nullptr;
            other._mmapP = nullptr;
#else // DJV_PLATFORM_WINDOWS
            other._f = -1;
            other._fileName = std::string();
            other._mode = Mode::First;
            other._pos = 0;
            other._size = 0;
            other._endian = false;
            other._mmap = (void *)-1;
            other._mmapStart = nullptr;
            other._mmapEnd = nullptr;
            other._mmapP = nullptr;
#endif //DJV_PLATFORM_WINDOWS
        }

        FileIO::~FileIO()
        {
            try
            {
                close();
            }
            catch (const std::exception&)
            {}
        }

        FileIO& FileIO::operator = (FileIO&& other)
        {
            if (this != &other)
            {
                _f = other._f;
                _fileName = other._fileName;
                _mode = other._mode;
                _pos = other._pos;
                _size = other._size;
                _endian = other._endian;
                _mmap = other._mmap;
                _mmapStart = other._mmapStart;
                _mmapEnd = other._mmapEnd;
                _mmapP = other._mmapP;
#if defined(DJV_PLATFORM_WINDOWS)
                other._f = INVALID_HANDLE_VALUE;
                other._fileName = std::string();
                other._mode = Mode::First;
                other._pos = 0;
                other._size = 0;
                other._endian = false;
                other._mmap = nullptr;
                other._mmapStart = nullptr;
                other._mmapEnd = nullptr;
                other._mmapP = nullptr;
#else // DJV_PLATFORM_WINDOWS
                other._f = -1;
                other._fileName = std::string();
                other._mode = Mode::First;
                other._pos = 0;
                other._size = 0;
                other._endian = false;
                other._mmap = (void *)-1;
                other._mmapStart = nullptr;
                other._mmapEnd = nullptr;
                other._mmapP = nullptr;
#endif //DJV_PLATFORM_WINDOWS
            }
            return *this;
        }

        void FileIO::readContents(FileIO& fileIO, std::string& out)
        {
            const uint8_t* p = fileIO.mmapP();
            const uint8_t* end = fileIO.mmapEnd();
            out = std::string(reinterpret_cast<const char*>(p), end - p);
        }

        void FileIO::readWord(FileIO& io, char* out, size_t maxLen)
        {
            DJV_ASSERT(maxLen);
            out[0] = 0;
            enum class Parse
            {
                End,
                Word,
                Comment
            };
            Parse parse = Parse::Word;
            size_t i = 0;
            while (parse != Parse::End)
            {
                // Get the next character.
                uint8_t c;
                io.read(&c, 1);

                switch (c)
                {
                case '#':
                    // Start of a comment.
                    parse = Parse::Comment;
                    break;
                case '\0':
                case '\n':
                case '\r':
                    // End of a comment or word.
                    parse = Parse::Word;
                case ' ':
                case '\t':
                    if (out[0])
                    {
                        parse = Parse::End;
                    }
                    break;
                default:
                    // Add the character to the word.
                    if (Parse::Word == parse && i < (maxLen - 1))
                    {
                        out[i++] = c;
                    }
                    break;
                }
            }
            out[i] = 0;
        }

        void FileIO::readLine(FileIO& io, char* out, size_t maxLen)
        {
            DJV_ASSERT(maxLen);
            size_t i = 0;
            if (!io.isEOF())
            {
                char c = 0;
                do
                {
                    io.read(&c, 1);
                    if (
                        c != '\n' &&
                        c != '\r')
                    {
                        out[i++] = c;
                    }
                } while (
                    c != '\n' &&
                    c != '\r' &&
                    !io.isEOF() &&
                    i < (maxLen - 1));
            }
            out[i] = 0;
        }

        std::vector<std::string> FileIO::readLines(const std::string& fileName)
        {
            std::vector<std::string> out;
            FileIO io;
            io.open(fileName, FileIO::Mode::Read);
            while (!io.isEOF())
            {
                char buf[String::cStringLength] = "";
                readLine(io, buf, String::cStringLength);
                out.push_back(buf);
            }
            return out;
        }

    } // namespace Core
} // namespace djv
