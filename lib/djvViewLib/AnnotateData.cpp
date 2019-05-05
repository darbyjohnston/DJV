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
            AbstractPrimitive::AbstractPrimitive()
            {}

            AbstractPrimitive::~AbstractPrimitive()
            {}

            void AbstractPrimitive::setColor(const AV::Color & value)
            {
                _color = value;
            }

            void AbstractPrimitive::setLineWidth(size_t value)
            {
                _lineWidth = value;
            }

            AV::Color AbstractPrimitive::getDrawColor(bool selected) const
            {
                AV::Color color(AV::Pixel::RGBA_U8);
                AV::ColorUtil::convert(_color, color);
                color.setU8(selected ? 255 : 127, 3);
                return color;
            }

            picojson::value AbstractPrimitive::toJSON() const
            {
                picojson::value out(picojson::object_type, true);
                out.get<picojson::object>()["color"] = djv::toJSON(_color);
                out.get<picojson::object>()["lineWidth"] = djv::toJSON(static_cast<int>(_lineWidth));
                picojson::value points(picojson::array_type, true);
                for (const auto& i : _points)
                {
                    points.get<picojson::array>().push_back(djv::toJSON(i));
                }
                out.get<picojson::object>()["points"] = points;
                return out;
            }

            void AbstractPrimitive::fromJSON(const picojson::value& value)
            {
                if (value.is<picojson::object>())
                {
                    for (const auto& i : value.get<picojson::object>())
                    {
                        if ("color" == i.first)
                        {
                            djv::fromJSON(i.second, _color);
                        }
                        else if ("lineWidth" == i.first)
                        {
                            int lineWidth = 0;
                            djv::fromJSON(i.second, lineWidth);
                            _lineWidth = static_cast<size_t>(Math::clamp(lineWidth, 1, 100));
                        }
                        else if ("points" == i.first)
                        {
                            if (i.second.is<picojson::array>())
                            {
                                for (const auto& j : i.second.get<picojson::array>())
                                {
                                    glm::ivec2 point(0, 0);
                                    djv::fromJSON(j, point);
                                    _points.push_back(point);
                                }
                            }
                        }
                    }
                }
            }

            FreehandLinePrimitive::FreehandLinePrimitive()
            {}

            FreehandLinePrimitive::~FreehandLinePrimitive()
            {}

            std::shared_ptr<FreehandLinePrimitive> FreehandLinePrimitive::create()
            {
                return std::shared_ptr<FreehandLinePrimitive>(new FreehandLinePrimitive);
            }

            void FreehandLinePrimitive::draw(QPainter & painter, const DrawData & data)
            {
                const size_t pointsSize = _points.size();
                if (1 == pointsSize)
                {
                    painter.fillRect(
                        (_points[0].x - _lineWidth / 2.f) * data.viewZoom + data.viewPos.x,
                        data.viewSize.y - 1 - ((_points[0].y + _lineWidth / 2.f) * data.viewZoom + data.viewPos.y),
                        _lineWidth * data.viewZoom,
                        _lineWidth * data.viewZoom,
                        AV::ColorUtil::toQt(getDrawColor(data.selected)));
                }
                else if (pointsSize >= 2)
                {
                    std::vector<QPoint> points;
                    for (size_t i = 0; i < _points.size(); ++i)
                    {
                        points.push_back(QPoint(
                            _points[i].x * data.viewZoom + data.viewPos.x,
                            data.viewSize.y - 1 - (_points[i].y * data.viewZoom + data.viewPos.y)));
                    }
                    QPen pen(AV::ColorUtil::toQt(getDrawColor(data.selected)), _lineWidth * data.viewZoom);
                    pen.setJoinStyle(Qt::MiterJoin);
                    painter.setPen(pen);
                    painter.drawPolyline(points.data(), static_cast<int>(points.size()));
                }
            }

            void FreehandLinePrimitive::mouse(const glm::ivec2 & value)
            {
                _points.push_back(value);
            }

            picojson::value FreehandLinePrimitive::toJSON() const
            {
                picojson::value out = AbstractPrimitive::toJSON();
                out.get<picojson::object>()["type"] = picojson::value("freehandLine");
                return out;
            }

            LinePrimitive::LinePrimitive()
            {}

            LinePrimitive::~LinePrimitive()
            {}

            std::shared_ptr<LinePrimitive> LinePrimitive::create()
            {
                return std::shared_ptr<LinePrimitive>(new LinePrimitive);
            }

            void LinePrimitive::draw(QPainter & painter, const DrawData & data)
            {
                const size_t pointsSize = _points.size();
                if (1 == pointsSize)
                {
                    painter.fillRect(
                        (_points[0].x - _lineWidth / 2.f) * data.viewZoom + data.viewPos.x,
                        data.viewSize.y - 1 - ((_points[0].y + _lineWidth / 2.f) * data.viewZoom + data.viewPos.y),
                        _lineWidth * data.viewZoom,
                        _lineWidth * data.viewZoom,
                        AV::ColorUtil::toQt(getDrawColor(data.selected)));
                }
                else if (pointsSize >= 2)
                {
                    std::vector<QPoint> points;
                    points.push_back(QPoint(
                        _points[0].x * data.viewZoom + data.viewPos.x,
                        data.viewSize.y - 1 - (_points[0].y * data.viewZoom + data.viewPos.y)));
                    points.push_back(QPoint(
                        _points[1].x * data.viewZoom + data.viewPos.x,
                        data.viewSize.y - 1 - (_points[1].y * data.viewZoom + data.viewPos.y)));
                    QPen pen(AV::ColorUtil::toQt(getDrawColor(data.selected)), _lineWidth * data.viewZoom);
                    pen.setJoinStyle(Qt::MiterJoin);
                    painter.setPen(pen);
                    painter.drawPolyline(points.data(), static_cast<int>(points.size()));
                }
            }

            void LinePrimitive::mouse(const glm::ivec2 & value)
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

            picojson::value LinePrimitive::toJSON() const
            {
                picojson::value out = AbstractPrimitive::toJSON();
                out.get<picojson::object>()["type"] = picojson::value("line");
                return out;
            }

            RectanglePrimitive::RectanglePrimitive()
            {}

            RectanglePrimitive::~RectanglePrimitive()
            {}

            std::shared_ptr<RectanglePrimitive> RectanglePrimitive::create()
            {
                return std::shared_ptr<RectanglePrimitive>(new RectanglePrimitive);
            }

            void RectanglePrimitive::draw(QPainter & painter, const DrawData & data)
            {
                if (_points.size() >= 2)
                {
                    QPen pen(AV::ColorUtil::toQt(getDrawColor(data.selected)), _lineWidth * data.viewZoom);
                    pen.setJoinStyle(Qt::MiterJoin);
                    painter.setPen(pen);
                    const glm::ivec2 & pos = _points[0];
                    const glm::ivec2 & pos2 = _points[1];
                    painter.drawRect(QRect(
                        QPoint(
                            pos.x * data.viewZoom + data.viewPos.x,
                            data.viewSize.y - 1 - (pos.y * data.viewZoom + data.viewPos.y)),
                        QPoint(
                            pos2.x * data.viewZoom + data.viewPos.x,
                            data.viewSize.y - 1 - (pos2.y * data.viewZoom + data.viewPos.y))));
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

            picojson::value RectanglePrimitive::toJSON() const
            {
                picojson::value out = AbstractPrimitive::toJSON();
                out.get<picojson::object>()["type"] = picojson::value("rectangle");
                return out;
            }

            EllipsePrimitive::EllipsePrimitive()
            {}

            EllipsePrimitive::~EllipsePrimitive()
            {}

            std::shared_ptr<EllipsePrimitive> EllipsePrimitive::create()
            {
                return std::shared_ptr<EllipsePrimitive>(new EllipsePrimitive);
            }

            void EllipsePrimitive::draw(QPainter & painter, const DrawData & data)
            {
                if (_points.size() >= 2)
                {
                    painter.setPen(QPen(AV::ColorUtil::toQt(getDrawColor(data.selected)), _lineWidth * data.viewZoom));
                    const glm::ivec2 & pos = _points[0];
                    const glm::ivec2 & pos2 = _points[1];
                    painter.drawEllipse(QRect(
                        QPoint(
                            pos.x * data.viewZoom + data.viewPos.x,
                            data.viewSize.y - 1 - (pos.y * data.viewZoom + data.viewPos.y)),
                        QPoint(
                            pos2.x * data.viewZoom + data.viewPos.x,
                            data.viewSize.y - 1 - (pos2.y * data.viewZoom + data.viewPos.y))));
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

            picojson::value EllipsePrimitive::toJSON() const
            {
                picojson::value out = AbstractPrimitive::toJSON();
                out.get<picojson::object>()["type"] = picojson::value("ellipse");
                return out;
            }

            std::shared_ptr<AbstractPrimitive> create(Enum::ANNOTATE_PRIMITIVE value)
            {
                std::shared_ptr<AbstractPrimitive> out = nullptr;
                switch (value)
                {
                case Enum::ANNOTATE_FREEHAND_LINE: out = FreehandLinePrimitive::create(); break;
                case Enum::ANNOTATE_LINE:          out = LinePrimitive::create(); break;
                case Enum::ANNOTATE_RECTANGLE:     out = RectanglePrimitive::create(); break;
                case Enum::ANNOTATE_ELLIPSE:       out = EllipsePrimitive::create(); break;
                default: break;
                }
                return out;
            }

            Data::Data(qint64 frameIndex, qint64 frameNumber, const QString & text, QObject * parent) :
                QObject(parent),
                _frameIndex(frameIndex),
                _frameNumber(frameNumber),
                _text(text)
            {}

            qint64 Data::frameIndex() const
            {
                return _frameIndex;
            }

            qint64 Data::frameNumber() const
            {
                return _frameNumber;
            }

            const QString & Data::text() const
            {
                return _text;
            }

            const std::vector<std::shared_ptr<AbstractPrimitive> > & Data::primitives() const
            {
                return _primitives;
            }

            void Data::setPrimitives(const std::vector<std::shared_ptr<AbstractPrimitive> > & value)
            {
                _primitives = value;
                Q_EMIT primitivesChanged();
            }

            void Data::addPrimitive(const std::shared_ptr<AbstractPrimitive>& primitive)
            {
                _primitives.push_back(primitive);
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
                    _primitives.clear();
                    Q_EMIT primitivesChanged();
                }
            }

        } // namespace Annotate
    } // namespace ViewLib
} // namespace djv
