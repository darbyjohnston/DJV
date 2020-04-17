// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCorePy/CorePy.h>

#include <glm/vec2.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void wrapVector(pybind11::module& m)
{
    py::class_<glm::ivec2>(m, "ivec2")
        .def(py::init<>())
        .def(py::init<int, int>())
        .def_readwrite("x", &glm::ivec2::x)
        .def_readwrite("y", &glm::ivec2::y)
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::class_<glm::vec2>(m, "vec2")
        .def(py::init<>())
        .def(py::init<float, float>())
        .def_readwrite("x", &glm::vec2::x)
        .def_readwrite("y", &glm::vec2::y)
        .def(py::self == py::self)
        .def(py::self != py::self);
    
    
}
