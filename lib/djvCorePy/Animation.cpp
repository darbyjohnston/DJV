// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

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

