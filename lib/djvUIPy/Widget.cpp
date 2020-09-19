// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Widget.h>

#include <djvSystem/Context.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv;
using namespace djv::Core;

namespace py = pybind11;

void wrapWidget(pybind11::module& m)
{
    py::class_<UI::Widget, std::shared_ptr<UI::Widget>, Core::IObject>(m, "Widget")
        .def_static("create", &UI::Widget::create)
        .def("setVisible", &UI::Widget::setVisible)
        .def("show", &UI::Widget::show)
        .def("hide", &UI::Widget::hide);
}

