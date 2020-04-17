// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv;
using namespace djv::Core;

namespace py = pybind11;

void wrapApplication(pybind11::module& m)
{
    py::class_<Desktop::Application, std::shared_ptr<Desktop::Application>, Context>(m, "Application")
        .def_static("create", &Desktop::Application::create)
        .def("run", &Desktop::Application::run)
        .def("exit", &Desktop::Application::exit);
}
