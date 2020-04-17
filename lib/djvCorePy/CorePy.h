// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace pybind11
{
    class module;

} // pybind11

void wrapAnimation(pybind11::module&);
void wrapBBox(pybind11::module&);
void wrapContext(pybind11::module&);
void wrapFileInfo(pybind11::module&);
void wrapFrame(pybind11::module&);
void wrapIObject(pybind11::module&);
void wrapPath(pybind11::module&);
void wrapVector(pybind11::module&);
