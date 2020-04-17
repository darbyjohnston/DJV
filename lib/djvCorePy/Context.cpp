// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCorePy/CorePy.h>

#include <djvCore/Context.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv::Core;

namespace py = pybind11;

void wrapContext(pybind11::module& m)
{
    py::class_<Context, std::shared_ptr<Context> >(m, "Context")
        .def_static("create", &Context::create)
        .def("getArgs", &Context::getArgs)
        .def("getName", &Context::getName);
}
