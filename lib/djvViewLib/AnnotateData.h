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

#include <djvViewLib/Enum.h>
#include <djvViewLib/ViewLib.h>

#include <djvAV/Color.h>

#include <djvCore/Vector.h>

#include <QObject>

#include <memory>

class QPainter;

namespace djv
{
    namespace ViewLib
    {
        namespace Annotate
        {
            //! This class provides the base functionality for annotation primitives.
            class AbstractPrimitive
            {
            public:
                AbstractPrimitive(const AV::Color &, size_t lineWidth);
                virtual ~AbstractPrimitive() = 0;

                virtual void draw(QPainter &, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom) = 0;
                virtual void mouse(const glm::ivec2 &) = 0;

            protected:
                AV::Color _color;
                size_t _lineWidth = 1;
            };

            //! This class provides a pen annotation primitive.
            class PenPrimitive : public AbstractPrimitive
            {
            public:
                PenPrimitive(const AV::Color &, size_t lineWidth);
                ~PenPrimitive() override;

                void draw(QPainter &, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom) override;
                void mouse(const glm::ivec2 &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This class provides a rectangle annotation primitive.
            class RectanglePrimitive : public AbstractPrimitive
            {
            public:
                RectanglePrimitive(const AV::Color &, size_t lineWidth);
                ~RectanglePrimitive() override;

                void draw(QPainter &, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom) override;
                void mouse(const glm::ivec2 &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This class provides an ellipse annotation primitive.
            class EllipsePrimitive : public AbstractPrimitive
            {
            public:
                EllipsePrimitive(const AV::Color &, size_t lineWidth);
                ~EllipsePrimitive() override;

                void draw(QPainter &, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom) override;
                void mouse(const glm::ivec2 &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This struct provides the data for an annotation.
            class Data : public QObject
            {
                Q_OBJECT

            public:
                Data(qint64 frame, QObject * parent = nullptr);

                qint64 frame() const;

                const QString & text() const;

                const std::vector<AbstractPrimitive *> & primitives();
                void addPrimitive(Enum::ANNOTATE_PRIMITIVE, const AV::Color &, size_t lineWidth);

            public Q_SLOTS:
                void setText(const QString &);
                void mouse(const glm::ivec2 &);
                void clearPrimitives();

            Q_SIGNALS:
                void textChanged(const QString &);
                void primitivesChanged();

            private:
                qint64 _frame = 0;
                QString _text;
                std::vector<AbstractPrimitive *> _primitives;
            };

        } // namespace Annotate
    } // namespace ViewLib
} // namespace djv
