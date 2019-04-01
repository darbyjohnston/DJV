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

#include <djvViewLib/AnnotationsData.h>

#include <djvAV/Color.h>
#include <djvAV/ColorUtil.h>

#include <QPainter>

namespace djv
{
    namespace ViewLib
    {
        namespace Annotations
        {
            void AbstractPrimitive::_init(Enum::ANNOTATIONS_COLOR color, size_t lineWidth)
            {
                _color = color;
                _lineWidth = lineWidth;
            }

            AbstractPrimitive::~AbstractPrimitive()
            {}

            std::shared_ptr<AbstractPrimitive> PrimitiveFactory::create(Enum::ANNOTATIONS_PRIMITIVE primitive, Enum::ANNOTATIONS_COLOR color, size_t lineWidth)
            {
                std::shared_ptr<AbstractPrimitive> out;
                switch (primitive)
                {
                case Enum::ANNOTATIONS_POLYLINE: out = PolylinePrimitive::create(color, lineWidth); break;
                case Enum::ANNOTATIONS_RECT:     out = RectPrimitive::create    (color, lineWidth); break;
                case Enum::ANNOTATIONS_ELLIPSE:  out = EllipsePrimitive::create (color, lineWidth); break;
                default: break;
                }
                return out;
            }

            PolylinePrimitive::~PolylinePrimitive()
            {}

            std::shared_ptr<PolylinePrimitive> PolylinePrimitive::create(Enum::ANNOTATIONS_COLOR color, size_t lineWidth)
            {
                auto out = std::shared_ptr<PolylinePrimitive>(new PolylinePrimitive);
                out->_init(color, lineWidth);
                return out;
            }

            void PolylinePrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
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
                    painter.setPen(QPen(AV::ColorUtil::toQt(Enum::annotationsColors()[_color]), _lineWidth * viewZoom));
                    painter.drawPolyline(points.data(), static_cast<int>(points.size()));
                }
            }

            void PolylinePrimitive::mouse(const glm::ivec2 & value)
            {
                _points.push_back(value);
            }

            RectPrimitive::~RectPrimitive()
            {}

            std::shared_ptr<RectPrimitive> RectPrimitive::create(Enum::ANNOTATIONS_COLOR color, size_t lineWidth)
            {
                auto out = std::shared_ptr<RectPrimitive>(new RectPrimitive);
                out->_init(color, lineWidth);
                return out;
            }

            void RectPrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
            {
                if (_points.size() >= 2)
                {
                    QPen pen(AV::ColorUtil::toQt(Enum::annotationsColors()[_color]), _lineWidth * viewZoom);
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

            void RectPrimitive::mouse(const glm::ivec2 & value)
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

            EllipsePrimitive::~EllipsePrimitive()
            {}

            std::shared_ptr<EllipsePrimitive> EllipsePrimitive::create(Enum::ANNOTATIONS_COLOR color, size_t lineWidth)
            {
                auto out = std::shared_ptr<EllipsePrimitive>(new EllipsePrimitive);
                out->_init(color, lineWidth);
                return out;
            }

            void EllipsePrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
            {
                if (_points.size() >= 2)
                {
                    painter.setPen(QPen(AV::ColorUtil::toQt(Enum::annotationsColors()[_color]), _lineWidth * viewZoom));
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

            void EllipsePrimitive::mouse(const glm::ivec2 & value)
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

        } // namespace Annotations
    } // namespace ViewLib
} // namespace djv
