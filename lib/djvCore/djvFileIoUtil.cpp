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

//! \file djvFileIoUtil.cpp

//#undef DJV_MMAP

#include <djvFileIoUtil.h>

#include <djvAssert.h>
#include <djvFileIo.h>

//------------------------------------------------------------------------------
// djvFileIoUtil
//------------------------------------------------------------------------------

djvFileIoUtil::~djvFileIoUtil()
{}

void djvFileIoUtil::word(djvFileIo & in, char * out, int maxLen) throw (djvError)
{
    DJV_ASSERT(maxLen > 0);

    //DJV_DEBUG("djvFileIoUtil::word");

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

void djvFileIoUtil::line(djvFileIo & in, char * out, int maxLen) throw (djvError)
{
    DJV_ASSERT(maxLen > 0);

    //DJV_DEBUG("djvFileIoUtil::line");

    //! \todo Should we handle comments here like djvFileIoUtil::word()?
    
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
    }
    while (
        c != '\n' &&
        c != '\r' &&
        in.isValid() &&
        i < (maxLen - 1));

    out[i] = 0;

    //DJV_DEBUG_PRINT("out = \"" << out << "\"");
}

QStringList djvFileIoUtil::lines(const QString & fileName) throw (djvError)
{
    QStringList out;

    djvFileIo io;
    
    io.open(fileName, djvFileIo::READ);

    while (io.isValid())
    {
        char buf[djvStringUtil::cStringLength] = "";
        
        line(io, buf, djvStringUtil::cStringLength);
        
        out += buf;
    }

    return out;
}

