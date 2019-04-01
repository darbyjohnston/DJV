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

#pragma once

#include <djvViewLib/AbstractPrefs.h>
#include <djvViewLib/DisplayProfile.h>
#include <djvViewLib/Enum.h>

#include <djvAV/Color.h>

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the annotations group preferences.
        class AnnotationsPrefs : public AbstractPrefs
        {
            Q_OBJECT

        public:
            explicit AnnotationsPrefs(const QPointer<ViewContext> &, QObject * parent = nullptr);
            ~AnnotationsPrefs() override;

            //! Get the current primitive.
            Enum::ANNOTATIONS_PRIMITIVE primitive() const;

            //! Get the current color.
            Enum::ANNOTATIONS_COLOR color() const;

            //! Get the current line width.
            size_t lineWidth() const;

        public Q_SLOTS:
            //! Set the current primitive.
            void setPrimitive(djv::ViewLib::Enum::ANNOTATIONS_PRIMITIVE);

            //! Set the current color.
            void setColor(djv::ViewLib::Enum::ANNOTATIONS_COLOR);

            //! Set the current line width.
            void setLineWidth(size_t);

        Q_SIGNALS:
            //! This signal is emitted when the current primitive is changed.
            void primitiveChanged(djv::ViewLib::Enum::ANNOTATIONS_PRIMITIVE);

            //! This signal is emitted when the current color is changed.
            void colorChanged(djv::ViewLib::Enum::ANNOTATIONS_COLOR);

            //! This signal is emitted when the current line width is changed.
            void lineWidthChanged(size_t);

        private:
            Enum::ANNOTATIONS_PRIMITIVE _primitive;
            Enum::ANNOTATIONS_COLOR _color;
            size_t _lineWidth;
        };

    } // namespace ViewLib
} // namespace djv
