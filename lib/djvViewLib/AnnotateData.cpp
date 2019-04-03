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

#include <djvViewLib/AnnotateData.h>

#include <djvAV/Color.h>
#include <djvAV/ColorUtil.h>

#include <QApplication>
#include <QPainter>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace Annotate
        {
            void AbstractPrimitive::_init(Enum::ANNOTATE_COLOR color, Enum::ANNOTATE_LINE_WIDTH lineWidth)
            {
                _color = color;
                _lineWidth = lineWidth;
            }

            AbstractPrimitive::~AbstractPrimitive()
            {}

            std::shared_ptr<AbstractPrimitive> PrimitiveFactory::create(
                Enum::ANNOTATE_PRIMITIVE primitive,
                Enum::ANNOTATE_COLOR color,
                Enum::ANNOTATE_LINE_WIDTH lineWidth)
            {
                std::shared_ptr<AbstractPrimitive> out;
                switch (primitive)
                {
                case Enum::ANNOTATE_PEN:    out = PenPrimitive::create   (color, lineWidth); break;
                case Enum::ANNOTATE_SQUARE: out = SquarePrimitive::create(color, lineWidth); break;
                case Enum::ANNOTATE_CIRCLE: out = CirclePrimitive::create(color, lineWidth); break;
                default: break;
                }
                return out;
            }

            PenPrimitive::~PenPrimitive()
            {}

            std::shared_ptr<PenPrimitive> PenPrimitive::create(
                Enum::ANNOTATE_COLOR color,
                Enum::ANNOTATE_LINE_WIDTH lineWidth)
            {
                auto out = std::shared_ptr<PenPrimitive>(new PenPrimitive);
                out->_init(color, lineWidth);
                return out;
            }

            void PenPrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
            {
                if (_points.size() >= 2)
                {
                    std::vector<QPoint> points;
                    for (size_t i = 0; i < _points.size(); ++i)
                    {
                        points.push_back(QPoint(
                            _points[i].x * viewZoom + viewPos.x,
                            size.y - 1 - (_points[i].y * viewZoom + viewPos.y)));
                    }
                    const AV::Color & color = Enum::annotateColors()[_color];
                    const size_t lineWidth = Enum::annotateLineWidths()[_lineWidth];
                    painter.setPen(QPen(AV::ColorUtil::toQt(color), lineWidth * viewZoom));
                    painter.drawPolyline(points.data(), static_cast<int>(points.size()));
                }
            }

            void PenPrimitive::mouse(const glm::ivec2 & value)
            {
                _points.push_back(value);
            }

            SquarePrimitive::~SquarePrimitive()
            {}

            std::shared_ptr<SquarePrimitive> SquarePrimitive::create(Enum::ANNOTATE_COLOR color, Enum::ANNOTATE_LINE_WIDTH lineWidth)
            {
                auto out = std::shared_ptr<SquarePrimitive>(new SquarePrimitive);
                out->_init(color, lineWidth);
                return out;
            }

            void SquarePrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
            {
                if (_points.size() >= 2)
                {
                    const AV::Color & color = Enum::annotateColors()[_color];
                    const size_t lineWidth = Enum::annotateLineWidths()[_lineWidth];
                    QPen pen(AV::ColorUtil::toQt(color), lineWidth * viewZoom);
                    pen.setJoinStyle(Qt::MiterJoin);
                    painter.setPen(pen);
                    const glm::ivec2 & pos = _points[0];
                    const glm::ivec2 & pos2 = _points[1];
                    painter.drawRect(QRect(
                        QPoint(
                            pos.x * viewZoom + viewPos.x,
                            size.y - 1 - (pos.y * viewZoom + viewPos.y)),
                        QPoint(
                            pos2.x * viewZoom + viewPos.x,
                            size.y - 1 - (pos2.y * viewZoom + viewPos.y))));
                }
            }

            void SquarePrimitive::mouse(const glm::ivec2 & value)
            {
                if (_points.size() < 2)
                {
                    _points.push_back(value);
                }
                else
                {
                    _points[1] = value;
                }
            }

            CirclePrimitive::~CirclePrimitive()
            {}

            std::shared_ptr<CirclePrimitive> CirclePrimitive::create(Enum::ANNOTATE_COLOR color, Enum::ANNOTATE_LINE_WIDTH lineWidth)
            {
                auto out = std::shared_ptr<CirclePrimitive>(new CirclePrimitive);
                out->_init(color, lineWidth);
                return out;
            }

            void CirclePrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
            {
                if (_points.size() >= 2)
                {
                    const AV::Color & color = Enum::annotateColors()[_color];
                    const size_t lineWidth = Enum::annotateLineWidths()[_lineWidth];
                    painter.setPen(QPen(AV::ColorUtil::toQt(color), lineWidth * viewZoom));
                    const glm::ivec2 & pos = _points[0];
                    const glm::ivec2 & pos2 = _points[1];
                    painter.drawEllipse(QRect(
                        QPoint(
                            pos.x * viewZoom + viewPos.x,
                            size.y - 1 - (pos.y * viewZoom + viewPos.y)),
                        QPoint(
                            pos2.x * viewZoom + viewPos.x,
                            size.y - 1 - (pos2.y * viewZoom + viewPos.y))));
                }
            }

            void CirclePrimitive::mouse(const glm::ivec2 & value)
            {
                if (_points.size() < 2)
                {
                    _points.push_back(value);
                }
                else
                {
                    _points[1] = value;
                }
            }

            std::shared_ptr<Data> Data::create(qint64 frame)
            {
                auto out = std::shared_ptr<Data>(new Data);
                out->_frame = frame;
                out->_text = qApp->translate("djv::ViewLib::Annotate::Data", "New annotation");
                return out;
            }

            qint64 Data::frame() const
            {
                return _frame;
            }

            const QString & Data::text() const
            {
                return _text;
            }

            void Data::setText(const QString & value)
            {
                _text = value;
            }

            const std::vector<std::shared_ptr<AbstractPrimitive> > & Data::primitives()
            {
                return _primitives;
            }

            void Data::addPrimitive(const std::shared_ptr<AbstractPrimitive> & value)
            {
                _primitives.push_back(value);
            }

            void Data::clearPrimitives()
            {
                _primitives.clear();
            }

            std::shared_ptr<Collection> Collection::create(const FileInfo & fileInfo)
            {
                auto out = std::shared_ptr<Collection>(new Collection);
                out->_fileInfo = fileInfo;
                return out;
            }

            const QString & Collection::summary() const
            {
                return _summary;
            }

            void Collection::setSummary(const QString & value)
            {
                _summary = value;
            }

            const std::vector<std::shared_ptr<Data>> & Collection::data() const
            {
                return _data;
            }

            size_t Collection::index(const std::shared_ptr<Data> & value) const
            {
                const auto i = std::find(_data.begin(), _data.end(), value);
                return i != _data.end() ? (i - _data.begin()) : size_t(-1);
            }

            size_t Collection::addData(const std::shared_ptr<Data> & data)
            {
                size_t out = 0;
                _data.push_back(data);
                std::stable_sort(_data.begin(), _data.end(),
                    [](const std::shared_ptr<Data> & a, const std::shared_ptr<Data> & b)
                {
                    return a->frame() < b->frame();
                });
                const auto i = std::find(_data.begin(), _data.end(), data);
                if (i != _data.end())
                {
                    out = i - _data.begin();
                }
                return out;
            }

            void Collection::removeData(const std::shared_ptr<Data> & value)
            {
                const auto i = std::find(_data.begin(), _data.end(), value);
                if (i != _data.end())
                {
                    _data.erase(i);
                }
            }

            void Collection::clear()
            {
                _summary = QString();
                _data.clear();
            }

        } // namespace Annotate
    } // namespace ViewLib
} // namespace djv
