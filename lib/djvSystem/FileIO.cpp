// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/FileIO.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            IO::IO()
            {}

            IO::~IO()
            {
                close();
            }

            std::shared_ptr<IO> IO::create()
            {
                return std::shared_ptr<IO>(new IO);
            }

            void IO::setPos(size_t in)
            {
                _setPos(in, false);
            }

            void IO::seek(size_t in)
            {
                _setPos(in, true);
            }
            
            void IO::read8(int8_t* value, size_t size)
            {
                return read(value, size, 1);
            }

            void IO::readU8(uint8_t* value, size_t size)
            {
                return read(value, size, 1);
            }

            void IO::read16(int16_t* value, size_t size)
            {
                return read(value, size, 2);
            }

            void IO::readU16(uint16_t* value, size_t size)
            {
                return read(value, size, 2);
            }

            void IO::read32(int32_t* value, size_t size)
            {
                return read(value, size, 4);
            }

            void IO::readU32(uint32_t* value, size_t size)
            {
                return read(value, size, 4);
            }

            void IO::readF32(float* value, size_t size)
            {
                return read(value, size, 4);
            }

            void IO::write8(const int8_t* value, size_t size)
            {
                write(value, size, 1);
            }

            void IO::writeU8(const uint8_t* value, size_t size)
            {
                write(value, size, 1);
            }

            void IO::write16(const int16_t* value, size_t size)
            {
                write(value, size, 2);
            }

            void IO::writeU16(const uint16_t* value, size_t size)
            {
                write(value, size, 2);
            }

            void IO::write32(const int32_t* value, size_t size)
            {
                return write(value, size, 4);
            }

            void IO::writeU32(const uint32_t* value, size_t size)
            {
                return write(value, size, 4);
            }

            void IO::writeF32(const float* value, size_t size)
            {
                write(value, size, 4);
            }

            void IO::write8(int8_t value)
            {
                write8(&value, 1);
            }

            void IO::writeU8(uint8_t value)
            {
                writeU8(&value, 1);
            }

            void IO::write16(int16_t value)
            {
                write16(&value, 1);
            }

            void IO::writeU16(uint16_t value)
            {
                writeU16(&value, 1);
            }

            void IO::write32(int32_t value)
            {
                write32(&value, 1);
            }

            void IO::writeU32(uint32_t value)
            {
                writeU32(&value, 1);
            }

            void IO::writeF32(float value)
            {
                writeF32(&value, 1);
            }

            void IO::write(const std::string& value)
            {
                write8(reinterpret_cast<const int8_t*>(value.c_str()), value.size());
            }

            std::string readContents(const std::shared_ptr<IO>& io)
            {
#ifdef DJV_MMAP
                const uint8_t* p = io->mmapP();
                const uint8_t* end = io->mmapEnd();
                return std::string(reinterpret_cast<const char*>(p), end - p);
#else // DJV_MMAP
                const size_t fileSize = io->getSize();
                std::string out;
                out.resize(fileSize);
                io->read(reinterpret_cast<void*>(&out[0]), fileSize);
                return out;
#endif // DJV_MMAP
            }

            void readWord(const std::shared_ptr<IO>& io, char* out, size_t maxLen)
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
                    uint8_t c = 0;
                    io->read(&c, 1);

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

            void readLine(const std::shared_ptr<IO>& io, char* out, size_t maxLen)
            {
                DJV_ASSERT(maxLen);
                size_t i = 0;
                if (!io->isEOF())
                {
                    char c = 0;
                    do
                    {
                        io->read(&c, 1);
                        if (
                            c != '\n' &&
                            c != '\r')
                        {
                            out[i++] = c;
                        }
                    } while (
                        c != '\n' &&
                        c != '\r' &&
                        !io->isEOF() &&
                        i < (maxLen - 1));
                }
                out[i] = 0;
            }

            std::vector<std::string> readLines(const std::string& fileName)
            {
                std::vector<std::string> out;
                auto io = IO::create();
                io->open(fileName, Mode::Read);
                while (!io->isEOF())
                {
                    char buf[String::cStringLength] = "";
                    readLine(io, buf, String::cStringLength);
                    out.push_back(buf);
                }
                return out;
            }

            void writeLines(const std::string& fileName, const std::vector<std::string>& lines)
            {
                auto io = IO::create();
                io->open(fileName, Mode::Write);
                for (const auto & line : lines)
                {
                    io->write(line);
                    io->write8('\n');
                }
            }

        } // namespace File
    } // namespace System
} // namespace djv
