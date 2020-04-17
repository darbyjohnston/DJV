// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCorePy/CorePy.h>

#include <djvCore/BBox.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv::Core;

namespace py = pybind11;

void wrapBBox(pybind11::module& m)
{
    py::class_<BBox2i>(m, "BBox2i")
        .def(py::init<>())
        .def(py::init<const glm::ivec2&>())
        .def(py::init<const glm::ivec2&, const glm::ivec2&>())
        .def(py::init<int, int, int, int>())
        .def_readwrite("min", &BBox2i::min)
        .def_readwrite("max", &BBox2i::max)
        .def("x", &BBox2i::x)
        .def("y", &BBox2i::y)
        .def("w", &BBox2i::w)
        .def("h", &BBox2i::h)
        .def("isValid", &BBox2i::isValid)
        .def("getSize", &BBox2i::getSize)
        .def("getCenter", &BBox2i::getCenter)
        .def("getArea", &BBox2i::getArea)
        .def("getAspect", &BBox2i::getAspect)
        .def("zero", &BBox2i::zero)
        //! \todo Fix me!
        //.def("contains", (bool(BBox2i::*)(const BBox2i&))&BBox2i::contains)
        //.def("contains", (bool(BBox2i::*)(const glm::ivec2&))&BBox2i::contains)
        .def("intersects", &BBox2i::intersects)
        .def("intersect", &BBox2i::intersect)
        //! \todo Fix me!
        //.def("expand", (void(BBox2i::*)(const BBox2i&))&BBox2i::expand)
        //.def("expand", (void(BBox2i::*)(const glm::ivec2&))&BBox2i::expand)
        //.def("margin", (BBox2i(BBox2i::*)(const glm::ivec2&))&BBox2i::expand)
        //.def("margin", (BBox2i(BBox2i::*)(int))&BBox2i::expand)
        //.def("margin", (BBox2i(BBox2i::*)(int, int, int, int))&BBox2i::expand)
        .def("__eq__", &BBox2i::operator==, py::is_operator())
        .def("__ne__", &BBox2i::operator!=, py::is_operator())
        ;

    py::class_<BBox2f>(m, "BBox2f")
        .def(py::init<>())
        .def(py::init<const glm::vec2&>())
        .def(py::init<const glm::vec2&, const glm::vec2&>())
        .def(py::init<float, float, float, float>())
        .def_readwrite("min", &BBox2f::min)
        .def_readwrite("max", &BBox2f::max)
        .def("x", &BBox2f::x)
        .def("y", &BBox2f::y)
        .def("w", &BBox2f::w)
        .def("h", &BBox2f::h)
        .def("isValid", &BBox2f::isValid)
        .def("getSize", &BBox2f::getSize)
        .def("getCenter", &BBox2f::getCenter)
        .def("getArea", &BBox2f::getArea)
        .def("getAspect", &BBox2f::getAspect)
        .def("zero", &BBox2f::zero)
        //! \todo Fix me!
        //.def("contains", (bool(BBox2f::*)(const BBox2f&))&BBox2f::contains)
        //.def("contains", (bool(BBox2f::*)(const glm::vec2&))&BBox2f::contains)
        .def("intersects", &BBox2f::intersects)
        .def("intersect", &BBox2f::intersect)
        //! \todo Fix me!
        //.def("expand", (void(BBox2f::*)(const BBox2f&))&BBox2f::expand)
        //.def("expand", (void(BBox2f::*)(const glm::vec2&))&BBox2f::expand)
        //.def("margin", (BBox2f(BBox2f::*)(const glm::vec2&))&BBox2f::expand)
        //.def("margin", (BBox2f(BBox2f::*)(float))&BBox2f::expand)
        //.def("margin", (BBox2f(BBox2f::*)(float, float, float, float))&BBox2f::expand)
        .def("__eq__", &BBox2f::operator==, py::is_operator())
        .def("__ne__", &BBox2f::operator!=, py::is_operator())
        ;

    py::class_<BBox3f>(m, "BBox3f")
        .def(py::init<>())
        .def(py::init<const glm::vec3&>())
        .def(py::init<const glm::vec3&, const glm::vec3&>())
        .def(py::init<float, float, float, float, float, float>())
        .def_readwrite("min", &BBox3f::min)
        .def_readwrite("max", &BBox3f::max)
        .def("x", &BBox3f::x)
        .def("y", &BBox3f::y)
        .def("z", &BBox3f::z)
        .def("w", &BBox3f::w)
        .def("h", &BBox3f::h)
        .def("d", &BBox3f::d)
        .def("isValid", &BBox3f::isValid)
        .def("getSize", &BBox3f::getSize)
        .def("getCenter", &BBox3f::getCenter)
        .def("zero", &BBox3f::zero)
        //! \todo Fix me!
        //.def("contains", (bool(BBox3f::*)(const BBox3f&))&BBox3f::contains)
        //.def("contains", (bool(BBox3f::*)(const glm::vec3&))&BBox3f::contains)
        .def("intersects", &BBox3f::intersects)
        //! \todo Fix me!
        //.def("intersect", (BBox3f(BBox3f::*)(const BBox3f&))&BBox3f::intersect)
        //.def("intersect", (bool(BBox3f::*)(const glm::vec3&, const glm::vec3&, glm::vec3&))&BBox3f::intersect)
        //.def("expand", (void(BBox3f::*)(const BBox3f&))&BBox3f::expand)
        //.def("expand", (void(BBox3f::*)(const glm::vec3&))&BBox3f::expand)
        //.def("margin", (BBox3f(BBox3f::*)(const glm::vec3&))&BBox3f::expand)
        //.def("margin", (BBox3f(BBox3f::*)(float))&BBox3f::expand)
        //.def("margin", (BBox3f(BBox3f::*)(float, float, float, float))&BBox3f::expand)
        .def("__eq__", &BBox3f::operator==, py::is_operator())
        .def("__ne__", &BBox3f::operator!=, py::is_operator())
        ;
}
