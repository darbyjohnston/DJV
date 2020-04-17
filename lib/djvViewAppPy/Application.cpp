// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/Application.h>

#include <djvCore/Context.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv;
using namespace djv::Core;

namespace py = pybind11;

void wrapApplication(pybind11::module& m)
{
    py::class_<ViewApp::Application, std::shared_ptr<ViewApp::Application>, Context>(m, "Application")
        .def_static("create", &ViewApp::Application::create)
        .def("run", &ViewApp::Application::run)
        .def("exit", &ViewApp::Application::exit);
}
