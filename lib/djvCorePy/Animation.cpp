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

#include <djvCorePy/CorePy.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>

#include <pybind11/pybind11.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

using namespace djv::Core;

namespace py = pybind11;

void wrapAnimation(pybind11::module& m)
{
    auto mAnimation = m.def_submodule("Animation");

    py::enum_<Animation::Type>(mAnimation, "Type")
        .value("Linear", Animation::Type::Linear)
        .value("EaseIn", Animation::Type::EaseIn)
        .value("EaseOut", Animation::Type::EaseOut)
        .value("SmoothStep", Animation::Type::SmoothStep)
        .value("Sine", Animation::Type::Sine);

    py::class_<Animation::Animation, std::shared_ptr<Animation::Animation> >(mAnimation, "Animation")
        .def_static("create", &Animation::Animation::create)
        .def("getType", &Animation::Animation::getType)
        .def("setType", &Animation::Animation::setType)
        .def("isRepeating", &Animation::Animation::isRepeating)
        .def("setRepeating", &Animation::Animation::setRepeating)
        .def("isActive", &Animation::Animation::isActive)
        .def("start", &Animation::Animation::start)
        .def("stop", &Animation::Animation::stop);
}
