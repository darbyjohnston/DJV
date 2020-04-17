// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCorePy/CorePy.h>

#include <djvCore/Path.h>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

using namespace djv::Core;

namespace py = pybind11;

void wrapPath(pybind11::module& m)
{
    py::enum_<FileSystem::PathSeparator>(m, "PathSeparator")
        .value("Unix", FileSystem::PathSeparator::Unix)
        .value("Windows", FileSystem::PathSeparator::Windows);

    py::enum_<FileSystem::ResourcePath>(m, "ResourcePath")
        .value("Application", FileSystem::ResourcePath::Application)
        .value("Documents", FileSystem::ResourcePath::Documents)
        .value("LogFile", FileSystem::ResourcePath::LogFile)
        .value("SettingsFile", FileSystem::ResourcePath::SettingsFile)
        .value("Audio", FileSystem::ResourcePath::Audio)
        .value("Fonts", FileSystem::ResourcePath::Fonts)
        .value("Icons", FileSystem::ResourcePath::Icons)
        .value("Images", FileSystem::ResourcePath::Images)
        .value("Models", FileSystem::ResourcePath::Models)
        .value("Shaders", FileSystem::ResourcePath::Shaders)
        .value("Text", FileSystem::ResourcePath::Text);

    py::class_<FileSystem::Path>(m, "Path")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def(py::init<const FileSystem::Path&, const std::string&>())
        .def(py::init<const std::string&, const std::string&>())
        .def("get", &FileSystem::Path::get)
        .def("set", &FileSystem::Path::set)
        .def("append", &FileSystem::Path::append, py::arg("value"), py::arg("separator") = FileSystem::Path::getCurrentSeparator())
        .def("isEmpty", &FileSystem::Path::isEmpty)
        .def("isRoot", &FileSystem::Path::isRoot)
        .def("cdUp", &FileSystem::Path::cdUp, py::arg("separator") = FileSystem::Path::getCurrentSeparator())
        .def("getDirectoryName", &FileSystem::Path::getDirectoryName)
        .def("getFileName", &FileSystem::Path::getFileName)
        .def("getBaseName", &FileSystem::Path::getBaseName)
        .def("getNumber", &FileSystem::Path::getNumber)
        .def("getExtension", &FileSystem::Path::getExtension)
        .def("setDirectoryName", &FileSystem::Path::setDirectoryName)
        .def("setFileName", &FileSystem::Path::setFileName)
        .def("setBaseName", &FileSystem::Path::setBaseName)
        .def("setNumber", &FileSystem::Path::setNumber)
        .def("setExtension", &FileSystem::Path::setExtension)
        .def_static("isSeparator", &FileSystem::Path::isSeparator)
        .def_static("getSeparator", &FileSystem::Path::getSeparator)
        .def_static("getCurrentSeparator", &FileSystem::Path::getCurrentSeparator)
        .def_static("split", &FileSystem::Path::split)
        .def_static("splitDir", &FileSystem::Path::splitDir)
        .def_static("joinDirs", &FileSystem::Path::joinDirs)
        .def_static("mkdir", &FileSystem::Path::mkdir)
        .def_static("getAbsolute", &FileSystem::Path::getAbsolute)
        .def_static("getCWD", &FileSystem::Path::getCWD)
        .def_static("getTemp", &FileSystem::Path::getTemp)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self);
}
