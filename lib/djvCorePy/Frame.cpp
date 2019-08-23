//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
