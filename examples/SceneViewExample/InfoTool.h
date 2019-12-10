//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include "ITool.h"

#include <djvUI/Label.h>
#include <djvUI/FormLayout.h>

class InfoTool : public ITool
{
    DJV_NON_COPYABLE(InfoTool);

protected:
    void _init(const std::shared_ptr<djv::Core::Context>&);
    InfoTool();

public:
    virtual ~InfoTool();

    static std::shared_ptr<InfoTool> create(const std::shared_ptr<djv::Core::Context>&);

    void setBBox(const djv::Core::BBox3f&);
    void setPrimitivesCount(size_t);
    void setTriangleCount(size_t);
    void setFPS(float);

protected:
    void _initEvent(djv::Core::Event::Init&) override;

private:
    void _textUpdate();

    djv::Core::BBox3f _bbox = djv::Core::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);
    size_t _primitivesCount = 0;
    size_t _triangleCount = 0;
    float _fps = 0.F;
    std::shared_ptr<djv::UI::Label> _sceneSizeLabel;
    std::shared_ptr<djv::UI::Label> _sceneXRangeLabel;
    std::shared_ptr<djv::UI::Label> _sceneYRangeLabel;
    std::shared_ptr<djv::UI::Label> _sceneZRangeLabel;
    std::shared_ptr<djv::UI::Label> _primitivesCountLabel;
    std::shared_ptr<djv::UI::Label> _triangleCountLabel;
    std::shared_ptr<djv::UI::Label> _fpsLabel;
    std::shared_ptr<djv::UI::FormLayout> _formLayout;
};
