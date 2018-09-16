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

#include <djvCore/Util.h>

#include <QVector>

#include <memory>

class QString;
class QStringList;

namespace djv
{
    namespace Core
    {
        //! \class Debug
        //!
        //! This class provides debugging messages.
        class Debug
        {
        public:
            Debug(const QString & prefix, const QString &);
            ~Debug();

            //! Add a message.
            void add(const QString &);

            //! This enumeration provides the line beginning and ending.
            enum LINE
            {
                LINE_BEGIN,
                LINE_END
            };

            //! Convert bits to a string.
            static QString bitsU8(quint8);

            //! Convert bits to a string.
            static QString bitsU16(quint16);

            //! Convert bits to a string.
            static QString bitsU32(quint32);

            void lineBegin();
            void lineEnd();

        private:
            void init(const QString &);

            DJV_PRIVATE_COPY(Debug);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace Core

      //! Start a debugging message block.
#define DJV_DEBUG(in) \
    djv::Core::Debug _debug(__FILE__, in)

      //! Print a debugging message.
#define DJV_DEBUG_PRINT(in) \
    _debug << djv::Core::Debug::LINE_BEGIN << in << djv::Core::Debug::LINE_END

      //! Convenience macro for declaring debugging operators.
#define DJV_DEBUG_OPERATOR(TYPE) \
    djv::Core::Debug & operator << (djv::Core::Debug &, const TYPE &)

    Core::Debug & operator << (Core::Debug &, Core::Debug::LINE);
    Core::Debug & operator << (Core::Debug &, const char *);
    Core::Debug & operator << (Core::Debug &, bool);
    Core::Debug & operator << (Core::Debug &, int);
    Core::Debug & operator << (Core::Debug &, unsigned int);
    Core::Debug & operator << (Core::Debug &, qint64);
    Core::Debug & operator << (Core::Debug &, quint64);
    Core::Debug & operator << (Core::Debug &, float);
    Core::Debug & operator << (Core::Debug &, double);
    DJV_DEBUG_OPERATOR(QString);
    DJV_DEBUG_OPERATOR(QStringList);
    //template<class T>
    //inline Core::Debug & operator << (Core::Debug &, const QVector<T> &);

} // namespace djv

#include <djvCore/DebugInline.h>

