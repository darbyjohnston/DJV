//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

//#undef DJV_MMAP

#include <djvCore/FileIOUtil.h>

#include <djvCore/Assert.h>
#include <djvCore/FileIO.h>

namespace djv
{
    namespace Core
    {
        FileIOUtil::~FileIOUtil()
        {}

        void FileIOUtil::word(FileIO & in, char * out, int maxLen)
        {
            DJV_ASSERT(maxLen > 0);

            //DJV_DEBUG("FileIOUtil::word");

            //! \todo Document how comments are handled.
            out[0] = 0;
            enum
            {
                END,
                WORD,
                COMMENT
            }
            parse = WORD;
            int i = 0;
            while (parse)
            {
                //DJV_DEBUG_PRINT("parse = " << parse);
                //DJV_DEBUG_PRINT("i = " << i);

                // Get the next character.
                quint8 c;
                in.get(&c, 1);
                //DJV_DEBUG_PRINT("c = \'" << c << "\'");

                switch (c)
                {
                    // Start of a comment.
                case '#':
                    parse = COMMENT;
                    break;
                    // End of a comment or word.
                case '\0':
                case '\n':
                case '\r':
                    parse = WORD;
                case ' ':
                case '\t':
                    if (out[0])
                    {
                        parse = END;
                    }
                    break;
                    // Add the character to the word.
                default:
                    if (WORD == parse && i < (maxLen - 1))
                    {
                        out[i++] = c;
                        //out[i] = 0;
                        //DJV_DEBUG_PRINT("out = \"" << out << "\"");
                    }
                    break;
                }
            }

            out[i] = 0;
            //DJV_DEBUG_PRINT("out = \"" << out << "\"");
        }

        void FileIOUtil::line(FileIO & in, char * out, int maxLen)
        {
            DJV_ASSERT(maxLen > 0);

            //DJV_DEBUG("FileIOUtil::line");

            //! \todo Should we handle comments here like FileIOUtil::word()?
            int  i = 0;
            char c = 0;
            do
            {
                in.get(&c, 1);
                if (
                    c != '\n' &&
                    c != '\r')
                {
                    out[i++] = c;
                    //DJV_DEBUG_PRINT(QString(out, i));
                }
            } while (
                c != '\n' &&
                c != '\r' &&
                in.isValid() &&
                i < (maxLen - 1));

            out[i] = 0;
            //DJV_DEBUG_PRINT("out = \"" << out << "\"");
        }

        QStringList FileIOUtil::lines(const QString & fileName)
        {
            QStringList out;
            FileIO io;
            io.open(fileName, FileIO::READ);
            while (io.isValid())
            {
                char buf[StringUtil::cStringLength] = "";
                line(io, buf, StringUtil::cStringLength);
                out += buf;
            }
            return out;
        }

    } // namespace Core
} // namespace djv
