// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Window.h>

#include <djvSystem/Context.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv;
using namespace djv::Core;

namespace py = pybind11;

void wrapWindow(pybind11::module& m)
{
    py::class_<UI::Window, std::shared_ptr<UI::Window>, UI::Widget>(m, "Window")
        .def_static("create", &UI::Window::create);
}

