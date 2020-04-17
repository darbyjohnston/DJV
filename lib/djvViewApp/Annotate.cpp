// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/Annotate.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
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
            render->setLineWidth(options.lineWidth);
            render->drawPolyline(p.points);
        }
        
        void AnnotateLine::addPoint(const glm::vec2& value)
        {
            _p->points.push_back(value);
        }
            
    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        AnnotateType,
        DJV_TEXT("annotate_type_polyline"),
        DJV_TEXT("annotate_type_line"),
        DJV_TEXT("annotate_type_rectangle"),
        DJV_TEXT("annotate_type_ellipse"));

    picojson::value toJSON(ViewApp::AnnotateType value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    void fromJSON(const picojson::value& value, ViewApp::AnnotateType& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

