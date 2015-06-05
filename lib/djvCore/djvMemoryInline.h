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

//! \file djvMemoryInline.h

//------------------------------------------------------------------------------
// djvMemory
//------------------------------------------------------------------------------

template<typename T>
inline void djvMemory::fill(T value, void * out, quint64 size)
{
    T * p = reinterpret_cast<T *>(out);

    for (quint64 i = 0; i < size; ++i)
    {
        p[i] = value;
    }
}

inline djvMemory::ENDIAN djvMemory::endianOpposite(ENDIAN in)
{
    return MSB == in ? LSB : MSB;
}

inline void djvMemory::convertEndian(
    void *  in,
    quint64 size,
    int     wordSize)
{
    quint8 * p = reinterpret_cast<quint8 *>(in);
    
    quint8 tmp;

    switch (wordSize)
    {
        case 2:
        
            while (size--)
            {
                tmp  = p[0];
                p[0] = p[1];
                p[1] = tmp;

                p += 2;
            }

            break;

        case 4:
        
            while (size--)
            {
                tmp  = p[0];
                p[0] = p[3];
                p[3] = tmp;

                tmp  = p[1];
                p[1] = p[2];
                p[2] = tmp;

                p += 4;
            }

            break;

        case 8:
        
            while (size--)
            {
                tmp  = p[0];
                p[0] = p[7];
                p[7] = tmp;

                tmp  = p[1];
                p[1] = p[6];
                p[6] = tmp;

                tmp  = p[2];
                p[2] = p[5];
                p[5] = tmp;

                tmp  = p[3];
                p[3] = p[4];
                p[4] = tmp;

                tmp  = p[4];
                p[4] = p[3];
                p[3] = tmp;

                tmp  = p[5];
                p[5] = p[2];
                p[2] = tmp;

                tmp  = p[6];
                p[6] = p[1];
                p[1] = tmp;

                tmp  = p[7];
                p[7] = p[0];
                p[0] = tmp;

                p += 8;
            }

            break;
    }
}

inline void djvMemory::convertEndian(
    const void * in,
    void *       out,
    quint64      size,
    int          wordSize)
{
    const quint8 * inP = reinterpret_cast<const quint8 *>(in);
    
    quint8 * outP = reinterpret_cast<quint8 *>(out);

    switch (wordSize)
    {
        case 2:
        
            while (size--)
            {
                outP[0] = inP[1];
                outP[1] = inP[0];

                inP  += 2;
                outP += 2;
            }

            break;

        case 4:
        
            while (size--)
            {
                outP[0] = inP[3];
                outP[1] = inP[2];
                outP[2] = inP[1];
                outP[3] = inP[0];

                inP  += 4;
                outP += 4;
            }

            break;

        case 8:
        
            while (size--)
            {
                outP[0] = inP[7];
                outP[1] = inP[6];
                outP[2] = inP[5];
                outP[3] = inP[4];
                outP[4] = inP[3];
                outP[5] = inP[2];
                outP[6] = inP[1];
                outP[7] = inP[0];

                inP  += 8;
                outP += 8;
            }

            break;

        default:
        
            djvMemory::copy(in, out, size * wordSize);
            
            break;
    }
}

