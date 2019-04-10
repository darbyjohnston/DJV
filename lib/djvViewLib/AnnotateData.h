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

#include <djvCore/PicoJSON.h>
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
            //! This struct provides data for drawing annotation primitives.
            struct DrawData
            {
                glm::ivec2 viewSize = glm::ivec2(0, 0);
                glm::ivec2 viewPos = glm::ivec2(0, 0);
                float viewZoom = 1.f;
                bool selected = false;
            };

            //! This class provides the base functionality for annotation primitives.
            class AbstractPrimitive
            {
            public:
                virtual ~AbstractPrimitive() = 0;

                void setColor(const AV::Color &);
                void setLineWidth(size_t);

                virtual void draw(QPainter &, const DrawData &) = 0;
                virtual void mouse(const glm::ivec2 &) = 0;
                virtual picojson::value toJSON() const = 0;
                //! Throws:
                //! - Error
                virtual void fromJSON(const picojson::value &) = 0;

            protected:
                AV::Color getDrawColor(bool selected) const;

                AV::Color _color;
                size_t _lineWidth = 1;
            };

            //! This class provides a freehand line annotation primitive.
            class FreehandLinePrimitive : public AbstractPrimitive
            {
            public:
                ~FreehandLinePrimitive() override;

                void draw(QPainter &, const DrawData &) override;
                void mouse(const glm::ivec2 &) override;
                picojson::value toJSON() const override;
                void fromJSON(const picojson::value &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This class provides a line annotation primitive.
            class LinePrimitive : public AbstractPrimitive
            {
            public:
                ~LinePrimitive() override;

                void draw(QPainter &, const DrawData &) override;
                void mouse(const glm::ivec2 &) override;
                picojson::value toJSON() const override;
                void fromJSON(const picojson::value &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This class provides a rectangle annotation primitive.
            class RectanglePrimitive : public AbstractPrimitive
            {
            public:
                ~RectanglePrimitive() override;

                void draw(QPainter &, const DrawData &) override;
                void mouse(const glm::ivec2 &) override;
                picojson::value toJSON() const override;
                void fromJSON(const picojson::value &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This class provides an ellipse annotation primitive.
            class EllipsePrimitive : public AbstractPrimitive
            {
            public:
                ~EllipsePrimitive() override;

                void draw(QPainter &, const DrawData &) override;
                void mouse(const glm::ivec2 &) override;
                picojson::value toJSON() const override;
                void fromJSON(const picojson::value &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This struct provides the data for an annotation.
            class Data : public QObject
            {
                Q_OBJECT

            public:
                Data(qint64 frameIndex, qint64 frameNumber, const QString & text = QString(), QObject * parent = nullptr);

                qint64 frameIndex() const;
                qint64 frameNumber() const;

                const QString & text() const;

                const std::vector<AbstractPrimitive *> & primitives();
                void setPrimitives(const std::vector<AbstractPrimitive *> &);
                void addPrimitive(AbstractPrimitive *);

            public Q_SLOTS:
                void setText(const QString &);
                void mouse(const glm::ivec2 &);
                void clearPrimitives();

            Q_SIGNALS:
                void textChanged(const QString &);
                void primitivesChanged();

            private:
                qint64 _frameIndex = 0;
                qint64 _frameNumber = 0;
                QString _text;
                std::vector<AbstractPrimitive *> _primitives;
            };

        } // namespace Annotate
    } // namespace ViewLib

} // namespace djv
