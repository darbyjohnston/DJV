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

#include <djvUI/SequencePrefs.h>

#include <djvUI/Prefs.h>

#include <djvCore/Debug.h>
#include <djvCore/Sequence.h>

#include <QApplication>

namespace djv
{
    namespace UI
    {
        struct SequencePrefs::Private
        {
        };

        SequencePrefs::SequencePrefs(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("SequencePrefs::SequencePrefs");
            Prefs prefs("djv::UI::SequencePrefs", Prefs::SYSTEM);
            Core::Sequence::COMPRESS compress = Core::Sequence::compress();
            if (prefs.get("compress", compress))
            {
                Core::Sequence::setCompress(compress);
            }
            bool autoEnabled = Core::Sequence::isAutoEnabled();
            if (prefs.get("autoEnabled", autoEnabled))
            {
                Core::Sequence::setAutoEnabled(autoEnabled);
            }
            qint64 maxSize = Core::Sequence::maxSize();
            if (prefs.get("maxSize", maxSize))
            {
                Core::Sequence::setMaxSize(maxSize);
            }
        }

        SequencePrefs::~SequencePrefs()
        {
            //DJV_DEBUG("SequencePrefs::~SequencePrefs");
            Prefs prefs("djv::UI::SequencePrefs", Prefs::SYSTEM);
            prefs.set("compress", Core::Sequence::compress());
            prefs.set("autoEnabled", Core::Sequence::isAutoEnabled());
            prefs.set("maxSize", Core::Sequence::maxSize());
        }

        Core::Sequence::COMPRESS SequencePrefs::compress() const
        {
            return Core::Sequence::compress();
        }

        bool SequencePrefs::isAutoEnabled() const
        {
            return Core::Sequence::isAutoEnabled();
        }

        qint64 SequencePrefs::maxSize() const
        {
            return Core::Sequence::maxSize();
        }

        void SequencePrefs::setCompress(Core::Sequence::COMPRESS value)
        {
            if (value == Core::Sequence::compress())
                return;
            Core::Sequence::setCompress(value);
            Q_EMIT compressChanged(Core::Sequence::compress());
            Q_EMIT prefChanged();
        }

        void SequencePrefs::setAutoEnabled(bool value)
        {
            if (value == Core::Sequence::isAutoEnabled())
                return;
            Core::Sequence::setAutoEnabled(value);
            Q_EMIT autoEnabledChanged(Core::Sequence::isAutoEnabled());
            Q_EMIT prefChanged();
        }

        void SequencePrefs::setMaxSize(qint64 size)
        {
            if (size == Core::Sequence::maxSize())
                return;
            Core::Sequence::setMaxSize(size);
            Q_EMIT maxSizeChanged(Core::Sequence::maxSize());
            Q_EMIT prefChanged();
        }

    } // namespace UI
} // namespace djv
