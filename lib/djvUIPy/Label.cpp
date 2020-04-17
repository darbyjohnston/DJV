// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Label.h>

#include <djvCore/Context.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv;
using namespace djv::Core;

namespace py = pybind11;

void wrapLabel(pybind11::module& m)
{
    py::class_<UI::Label, std::shared_ptr<UI::Label>, UI::Widget>(m, "Label")
        .def_static("create", &UI::Label::create)
        .def("setText", &UI::Label::setText);
}

