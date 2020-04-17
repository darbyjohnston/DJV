// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv;
using namespace djv::Core;

namespace py = pybind11;

void wrapApplication(pybind11::module& m)
{
    py::class_<CmdLine::Application, std::shared_ptr<CmdLine::Application>, Context>(m, "Application")
        .def_static("create", &CmdLine::Application::create)
        .def("run", &CmdLine::Application::run)
        .def("exit", &CmdLine::Application::exit);
}
