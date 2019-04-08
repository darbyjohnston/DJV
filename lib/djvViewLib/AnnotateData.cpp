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
            AbstractPrimitive::AbstractPrimitive(const AV::Color & color, size_t lineWidth) :
                _color(color),
                _lineWidth(lineWidth)
            {}

            AbstractPrimitive::~AbstractPrimitive()
            {}

            PenPrimitive::PenPrimitive(const AV::Color & color, size_t lineWidth) :
                AbstractPrimitive(color, lineWidth)
            {}

            PenPrimitive::~PenPrimitive()
            {}

            void PenPrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
            {
                const size_t pointsSize = _points.size();
                if (1 == pointsSize)
                {
                    painter.fillRect(
                        (_points[0].x - _lineWidth / 2.f) * viewZoom + viewPos.x,
                        size.y - 1 - ((_points[0].y + _lineWidth / 2.f) * viewZoom + viewPos.y),
                        _lineWidth * viewZoom,
                        _lineWidth * viewZoom,
                        AV::ColorUtil::toQt(_color));
                }
                else if (pointsSize >= 2)
                {
                    std::vector<QPoint> points;
                    for (size_t i = 0; i < _points.size(); ++i)
                    {
                        points.push_back(QPoint(
                            _points[i].x * viewZoom + viewPos.x,
                            size.y - 1 - (_points[i].y * viewZoom + viewPos.y)));
                    }
                    QPen pen(AV::ColorUtil::toQt(_color), _lineWidth * viewZoom);
                    pen.setJoinStyle(Qt::MiterJoin);
                    painter.setPen(pen);
                    painter.drawPolyline(points.data(), static_cast<int>(points.size()));
                }
            }

            void PenPrimitive::mouse(const glm::ivec2 & value)
            {
                _points.push_back(value);
            }

            RectanglePrimitive::RectanglePrimitive(const AV::Color & color, size_t lineWidth) :
                AbstractPrimitive(color, lineWidth)
            {}

            RectanglePrimitive::~RectanglePrimitive()
            {}

            void RectanglePrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
            {
                if (_points.size() >= 2)
                {
                    QPen pen(AV::ColorUtil::toQt(_color), _lineWidth * viewZoom);
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

            void RectanglePrimitive::mouse(const glm::ivec2 & value)
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

            EllipsePrimitive::EllipsePrimitive(const AV::Color & color, size_t lineWidth) :
                AbstractPrimitive(color, lineWidth)
            {}

            EllipsePrimitive::~EllipsePrimitive()
            {}

            void EllipsePrimitive::draw(QPainter & painter, const glm::ivec2 & size, const glm::ivec2 & viewPos, float viewZoom)
            {
                if (_points.size() >= 2)
                {
                    painter.setPen(QPen(AV::ColorUtil::toQt(_color), _lineWidth * viewZoom));
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

            Data::Data(qint64 frame, const QString & text, QObject * parent) :
                QObject(parent),
                _frame(frame),
                _text(text)
            {}

            qint64 Data::frame() const
            {
                return _frame;
            }

            const QString & Data::text() const
            {
                return _text;
            }

            const std::vector<AbstractPrimitive *> & Data::primitives()
            {
                return _primitives;
            }

            void Data::addPrimitive(Enum::ANNOTATE_PRIMITIVE primitive, const AV::Color & color, size_t lineWidth)
            {
                AbstractPrimitive * p = nullptr;
                switch (primitive)
                {
                case Enum::ANNOTATE_PEN:       p = new PenPrimitive(color, lineWidth); break;
                case Enum::ANNOTATE_RECTANGLE: p = new RectanglePrimitive(color, lineWidth); break;
                case Enum::ANNOTATE_ELLIPSE:   p = new EllipsePrimitive(color, lineWidth); break;
                default: break;
                }
                _primitives.push_back(p);
                Q_EMIT primitivesChanged();
            }

            void Data::setText(const QString & value)
            {
                if (value == _text)
                    return;
                _text = value;
                Q_EMIT textChanged(_text);
            }

            void Data::mouse(const glm::ivec2 & value)
            {
                if (_primitives.size())
                {
                    _primitives.back()->mouse(value);
                    Q_EMIT primitivesChanged();
                }
            }

            void Data::clearPrimitives()
            {
                if (_primitives.size())
                {
                    for (auto i : _primitives)
                    {
                        delete i;
                    }
                    _primitives.clear();
                    Q_EMIT primitivesChanged();
                }
            }

        } // namespace Annotate
    } // namespace ViewLib
} // namespace djv
