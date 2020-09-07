// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/Annotate.h>

#include <djvViewApp/Media.h>

#include <djvAV/Render2D.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        float getAnnotateLineSize(AnnotateLineSize value)
        {
            const std::vector<float> data =
            {
                1.F,
                5.F,
                10.F
            };
            return data[static_cast<size_t>(value)];
        }

        struct AnnotatePrimitive::Private
        {
            std::weak_ptr<Context> context;
            AnnotateOptions options;
        };
        
        void AnnotatePrimitive::_init(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;
            p.options = options;
        }
        
        AnnotatePrimitive::AnnotatePrimitive() :
            _p(new Private)
        {}
        
        AnnotatePrimitive::~AnnotatePrimitive()
        {}
        
        const AnnotateOptions& AnnotatePrimitive::getOptions() const
        {
            return _p->options;
        }
        
        const std::weak_ptr<Context>& AnnotatePrimitive::getContext() const
        {
            return _p->context;
        }

        struct AnnotatePolyline::Private
        {
            std::vector<glm::vec2> points;
        };
        
        void AnnotatePolyline::_init(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            AnnotatePrimitive::_init(options, context);
        }
        
        AnnotatePolyline::AnnotatePolyline() :
            _p(new Private)
        {}
            
        AnnotatePolyline::~AnnotatePolyline()
        {}
        
        std::shared_ptr<AnnotatePolyline> AnnotatePolyline::create(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<AnnotatePolyline>(new AnnotatePolyline);
            out->_init(options, context);
            return out;
        }
            
        void AnnotatePolyline::draw(const std::shared_ptr<AV::Render2D::Render>& render)
        {
            DJV_PRIVATE_PTR();
            const auto& options = getOptions();
            render->setFillColor(options.color);
            render->setLineWidth(options.lineSize);
            render->drawPolyline(p.points);
        }
        
        void AnnotatePolyline::addPoint(const glm::vec2& value)
        {
            _p->points.push_back(value);
        }

        struct AnnotateLine::Private
        {
            std::vector<glm::vec2> points;
        };

        void AnnotateLine::_init(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            AnnotatePrimitive::_init(options, context);
        }

        AnnotateLine::AnnotateLine() :
            _p(new Private)
        {}

        AnnotateLine::~AnnotateLine()
        {}

        std::shared_ptr<AnnotateLine> AnnotateLine::create(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<AnnotateLine>(new AnnotateLine);
            out->_init(options, context);
            return out;
        }

        void AnnotateLine::draw(const std::shared_ptr<AV::Render2D::Render>& render)
        {
            DJV_PRIVATE_PTR();
            const auto& options = getOptions();
            render->setFillColor(options.color);
            render->setLineWidth(options.lineSize);
            render->drawPolyline(p.points);
        }

        void AnnotateLine::addPoint(const glm::vec2& value)
        {
            while (_p->points.size() > 1)
            {
                _p->points.pop_back();
            }
            _p->points.push_back(value);
        }

        struct AnnotateRectangle::Private
        {
            std::vector<glm::vec2> points;
        };

        void AnnotateRectangle::_init(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            AnnotatePrimitive::_init(options, context);
        }

        AnnotateRectangle::AnnotateRectangle() :
            _p(new Private)
        {}

        AnnotateRectangle::~AnnotateRectangle()
        {}

        std::shared_ptr<AnnotateRectangle> AnnotateRectangle::create(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<AnnotateRectangle>(new AnnotateRectangle);
            out->_init(options, context);
            return out;
        }

        void AnnotateRectangle::draw(const std::shared_ptr<AV::Render2D::Render>& render)
        {
            DJV_PRIVATE_PTR();
            const auto& options = getOptions();
            render->setFillColor(options.color);
            render->setLineWidth(options.lineSize);
            render->drawPolyline(p.points);
        }

        void AnnotateRectangle::addPoint(const glm::vec2& value)
        {
            _p->points.push_back(value);
        }

        struct AnnotateEllipse::Private
        {
            std::vector<glm::vec2> points;
        };

        void AnnotateEllipse::_init(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            AnnotatePrimitive::_init(options, context);
        }

        AnnotateEllipse::AnnotateEllipse() :
            _p(new Private)
        {}

        AnnotateEllipse::~AnnotateEllipse()
        {}

        std::shared_ptr<AnnotateEllipse> AnnotateEllipse::create(const AnnotateOptions& options, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<AnnotateEllipse>(new AnnotateEllipse);
            out->_init(options, context);
            return out;
        }

        void AnnotateEllipse::draw(const std::shared_ptr<AV::Render2D::Render>& render)
        {
            DJV_PRIVATE_PTR();
            const auto& options = getOptions();
            render->setFillColor(options.color);
            render->setLineWidth(options.lineSize);
            render->drawPolyline(p.points);
        }

        void AnnotateEllipse::addPoint(const glm::vec2& value)
        {
            _p->points.push_back(value);
        }
        
        struct AnnotateCommand::Private
        {
            std::shared_ptr<AnnotatePrimitive> primitive;
            std::weak_ptr<Media> media;
            std::shared_ptr<ValueSubject<bool> > undo;
        };

        void AnnotateCommand::_init(
            const std::shared_ptr<AnnotatePrimitive>& primitive,
            const std::shared_ptr<Media>& media)
        {
            DJV_PRIVATE_PTR();
            p.primitive = primitive;
            p.media = media;
            p.undo = ValueSubject<bool>::create();
        }

        AnnotateCommand::AnnotateCommand() :
            _p(new Private)
        {}

        std::shared_ptr<AnnotateCommand> AnnotateCommand::create(
            const std::shared_ptr<AnnotatePrimitive>& primitive,
            const std::shared_ptr<Media>& media)
        {
            auto out = std::shared_ptr<AnnotateCommand>(new AnnotateCommand);
            out->_init(primitive, media);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<bool> > AnnotateCommand::observeUndo() const
        {
            return _p->undo;
        }

        const std::shared_ptr<AnnotatePrimitive>& AnnotateCommand::getPrimitive() const
        {
            return _p->primitive;
        }

        const std::weak_ptr<Media>& AnnotateCommand::getMedia() const
        {
            return _p->media;
        }

        void AnnotateCommand::exec()
        {
            DJV_PRIVATE_PTR();
            if (auto media = p.media.lock())
            {
                media->addAnnotation(p.primitive);
            }
        }

        void AnnotateCommand::undo()
        {
            DJV_PRIVATE_PTR();
            if (auto media = p.media.lock())
            {
                media->removeAnnotation(p.primitive);
            }
            p.undo->setAlways(true);
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(AnnotateTool);
        DJV_ENUM_HELPERS_IMPLEMENTATION(AnnotateLineSize);

    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        AnnotateTool,
        DJV_TEXT("annotate_tool_polyline"),
        DJV_TEXT("annotate_tool_line"),
        DJV_TEXT("annotate_tool_rectangle"),
        DJV_TEXT("annotate_tool_ellipse"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        AnnotateLineSize,
        DJV_TEXT("annotate_line_size_small"),
        DJV_TEXT("annotate_line_size_medium"),
        DJV_TEXT("annotate_line_size_large"));

    rapidjson::Value toJSON(ViewApp::AnnotateTool value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(ViewApp::AnnotateLineSize value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::AnnotateTool& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::AnnotateLineSize& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
