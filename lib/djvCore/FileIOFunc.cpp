// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileIOFunc.h>

#include <djvCore/FileIO.h>

#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            std::string readContents(const std::shared_ptr<FileIO>& io)
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

            void readWord(const std::shared_ptr<FileIO>& io, char* out, size_t maxLen)
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

            void readLine(const std::shared_ptr<FileIO>& io, char* out, size_t maxLen)
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
                auto io = FileIO::create();
                io->open(fileName, FileIO::Mode::Read);
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
                auto io = FileIO::create();
                io->open(fileName, FileIO::Mode::Write);
                for (const auto & line : lines)
                {
                    io->write(line);
                    io->write8('\n');
                }
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv
