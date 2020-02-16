//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
            
        void AnnotateLine::draw(const std::shared_ptr<AV::Render::Render2D>& render)
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
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

