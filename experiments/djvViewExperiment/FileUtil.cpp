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

#include <FileUtil.h>

#include <QDir>
#include <QRegularExpression>

#include <iostream>

namespace djv
{
    namespace ViewExperiment
    {
#if defined(DJV_WINDOWS)
        const QChar FileUtil::nativeSeparator = '\\';
#else
        const QChar FileUtil::nativeSeparator = '/';
#endif

        QStringList FileUtil::splitPath(const QString & value)
        {
            QStringList out;

            // Handle the root path.
            if (value.size() && '/' == value[0])
            {
                out.push_back("/");
            }

            // Split the path on seperators.
            static const QRegularExpression re("[/\\\\]");
            out.append(value.split(re, QString::SkipEmptyParts));

            return out;
        }

        QString FileUtil::joinPath(QStringList value)
        {
            QString out;

            // Handle the root path.
            if (value.size())
            {
                if (value[0] != "/")
                {
                    out.append(value[0]);
                }
                out.append('/');
                value.pop_front();
            }

            // Join the remaining pieces.
            out.append(value.join(nativeSeparator));

            return out;
        }

    } // namespace ViewExperiment
} // namespace djv
