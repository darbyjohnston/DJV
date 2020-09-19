// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCorePy/CorePy.h>

#include <djvSystem/Context.h>
#include <djvCore/IObject.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv::Core;

namespace py = pybind11;

class IObjectWrapper : public IObject
{
public:
    using IObject::_init;
};

void wrapIObject(pybind11::module& m)
{
    py::class_<IObject, std::shared_ptr<IObject> >(m, "IObject")
        .def("_init", &IObjectWrapper::_init)
        .def("addChild", &IObject::addChild);
}
