// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCorePy/CorePy.h>

#include <djvCore/Frame.h>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

using namespace djv::Core;

namespace py = pybind11;

void wrapFrame(pybind11::module& m)
{
    auto mFrame = m.def_submodule("Frame");
    
    //! \todo How do we make this a constant?
    mFrame.attr("invalid") = Frame::invalid;

    py::class_<Frame::Range>(mFrame, "Range")
        .def(py::init<>())
        .def(py::init<Frame::Number>())
        .def(py::init<Frame::Number, Frame::Number>())
        .def_readwrite("min", &Frame::Range::min)
        .def_readwrite("max", &Frame::Range::max)
        .def("zero", &Frame::Range::zero)
        .def("contains", &Frame::Range::contains)
        .def("intersects", &Frame::Range::intersects)
        .def("expand", (void(Frame::Range::*)(Frame::Number))&Frame::Range::expand)
        .def("expand", (void(Frame::Range::*)(const Frame::Range&))&Frame::Range::expand)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self);

    //! \todo How do we make this a constant?
    mFrame.attr("invalidRange") = Frame::invalidRange;

    py::class_<Frame::Sequence>(mFrame, "Sequence")
        .def(py::init<>())
        .def(py::init<const Frame::Range&, size_t>(), py::arg("range"), py::arg("pad") = 0)
        .def(py::init<const std::vector<Frame::Range>&, size_t>(), py::arg("ranges"), py::arg("pad") = 0)
        .def_readwrite("ranges", &Frame::Sequence::ranges)
        .def_readwrite("pad", &Frame::Sequence::pad)
        .def("isValid", &Frame::Sequence::isValid)
        .def("getSize", &Frame::Sequence::getSize)
        .def("getFrame", &Frame::Sequence::getFrame)
        .def("sort", &Frame::Sequence::sort)
        .def(py::self == py::self)
        .def(py::self != py::self);

    mFrame.def("isValid", &Frame::isValid);
    mFrame.def("sort", &Frame::sort);
    mFrame.def("toFrames", (std::vector<Frame::Number>(*)(const Frame::Range&))&Frame::toFrames);
    mFrame.def("toFrames", (std::vector<Frame::Number>(*)(const Frame::Sequence&))&Frame::toFrames);
    mFrame.def("fromFrames", &Frame::fromFrames);
    mFrame.def("toString", (std::string(*)(Frame::Number, size_t))&Frame::toString, py::arg("number"), py::arg("pad") = 0);
    mFrame.def("toString", (std::string(*)(const Frame::Range&, size_t))&Frame::toString, py::arg("range"), py::arg("pad") = 0);
    mFrame.def("toString", (std::string(*)(const Frame::Sequence&))&Frame::toString, py::arg("sequence"));
    mFrame.def("fromString", (void(*)(const std::string&, Frame::Range&, size_t&))&Frame::fromString);
    mFrame.def("fromString", (void(*)(const std::string&, Frame::Sequence&))&Frame::fromString);
}
