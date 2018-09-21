
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

#include <djvUI/Core.h>

#include <djvCore/Util.h>
#include <djvCore/Vector.h>

#include <QWidget>

#include <memory>

namespace djv
{
    namespace UI
    {
        //! \class Vector2iEditWidget
        //!
        //! This class provides a two-dimensional integer vector widget.
        class Vector2iEditWidget : public QWidget
        {
            Q_OBJECT

                //! This property holds the value.    
                Q_PROPERTY(
                    glm::ivec2 value
                    READ       value
                    WRITE      setValue
                    NOTIFY     valueChanged)

                //! This property holds the minimum value.
                Q_PROPERTY(
                    glm::ivec2 min
                    READ       min
                    WRITE      setMin
                    NOTIFY     minChanged)

                //! This property holds the maximum value.
                Q_PROPERTY(
                    glm::ivec2 max
                    READ       max
                    WRITE      setMax
                    NOTIFY     maxChanged)

        public:
            explicit Vector2iEditWidget(QWidget * parent = nullptr);

            //! Get the value.
            glm::ivec2 value() const;

            //! Get the minimum value.
            glm::ivec2 min() const;

            //! Get the maximum value.
            glm::ivec2 max() const;

        public Q_SLOTS:
            //! Set the value.
            void setValue(const glm::ivec2 &);

            //! Set the minimum value.
            void setMin(const glm::ivec2 &);

            //! Set the maximum value.
            void setMax(const glm::ivec2 &);

            //! Set the value range.
            void setRange(const glm::ivec2 & min, const glm::ivec2 & max);

        Q_SIGNALS:
            //! This signal is emitted when the value is changed.
            void valueChanged(const glm::ivec2 &);

            //! This signal is emitted when the minimum value is changed.
            void minChanged(const glm::ivec2 &);

            //! This signal is emitted when the maximum value is changed.
            void maxChanged(const glm::ivec2 &);

            //! This signal is emitted when the value range is changed.
            void rangeChanged(const glm::ivec2 &, const glm::ivec2 &);

        private Q_SLOTS:
            void valueCallback();
            void rangeCallback();

        private:
            DJV_PRIVATE_COPY(Vector2iEditWidget);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv