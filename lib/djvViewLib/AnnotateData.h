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

#include <djvCore/FileInfo.h>
#include <djvCore/Vector.h>

#include <map>
#include <memory>

class QPainter;

namespace djv
{
    namespace ViewLib
    {
        namespace Annotate
        {
            //! This class provides the base functionality for annotation primitives.
            class AbstractPrimitive : public std::enable_shared_from_this<AbstractPrimitive>
            {
            protected:
                void _init(const AV::Color &, size_t lineWidth);

            public:
                virtual ~AbstractPrimitive() = 0;

                virtual void draw(QPainter &, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom) = 0;
                virtual void mouse(const glm::ivec2 &) = 0;

            protected:
                AV::Color _color;
                size_t _lineWidth = 1;
            };

            //! This class provides a factory to create annotation primitives.
            class PrimitiveFactory
            {
            public:
                static std::shared_ptr<AbstractPrimitive> create(Enum::ANNOTATE_PRIMITIVE, const AV::Color &, size_t lineWidth);
            };

            //! This class provides a pen annotation primitive.
            class PenPrimitive : public AbstractPrimitive
            {
            protected:
                PenPrimitive() {}

            public:
                ~PenPrimitive() override;

                static std::shared_ptr<PenPrimitive> create(const AV::Color &, size_t lineWidth);

                void draw(QPainter &, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom) override;
                void mouse(const glm::ivec2 &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This class provides a rectangle annotation primitive.
            class RectanglePrimitive : public AbstractPrimitive
            {
            protected:
                RectanglePrimitive() {}

            public:
                ~RectanglePrimitive() override;

                static std::shared_ptr<RectanglePrimitive> create(const AV::Color &, size_t lineWidth);

                void draw(QPainter &, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom) override;
                void mouse(const glm::ivec2 &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This class provides a ellipse annotation primitive.
            class EllipsePrimitive : public AbstractPrimitive
            {
            protected:
                EllipsePrimitive() {}

            public:
                ~EllipsePrimitive() override;

                static std::shared_ptr<EllipsePrimitive> create(const AV::Color &, size_t lineWidth);

                void draw(QPainter &, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom) override;
                void mouse(const glm::ivec2 &) override;

            private:
                std::vector<glm::ivec2> _points;
            };

            //! This struct provides the data for an annotation.
            class Data
            {
                Data() {}

            public:
                static std::shared_ptr<Data> create(qint64 frame);

                qint64 frame() const;

                const QString & text() const;
                void setText(const QString &);

                const std::vector<std::shared_ptr<AbstractPrimitive> > & primitives();
                void addPrimitive(const std::shared_ptr<AbstractPrimitive> &);
                void clearPrimitives();

            private:
                qint64 _frame = 0;
                QString _text;
                std::vector<std::shared_ptr<AbstractPrimitive> > _primitives;
            };

            //! This class provides a collection of annotations.
            class Collection
            {
                Collection() {}

            public:
                static std::shared_ptr<Collection> create(const Core::FileInfo &);

                const Core::FileInfo & fileInfo() const;

                const QString & summary() const;
                void setSummary(const QString &);

                const std::vector<std::shared_ptr<Data> > & data() const;
                size_t index(const std::shared_ptr<Data> &) const;
                size_t addData(const std::shared_ptr<Data> &);
                void removeData(const std::shared_ptr<Data> &);

                void clear();

            private:
                Core::FileInfo _fileInfo;
                QString _summary;
                std::vector<std::shared_ptr<Data>> _data;
            };

        } // namespace Annotate
    } // namespace ViewLib
} // namespace djv
