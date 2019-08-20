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

#include <djvCore/Path.h>

#include <pybind11/pybind11.h>
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
        .value("LogFile", FileSystem::ResourcePath::LogFile)
        .value("SettingsFile", FileSystem::ResourcePath::SettingsFile)
        .value("AudioDirectory", FileSystem::ResourcePath::AudioDirectory)
        .value("FontsDirectory", FileSystem::ResourcePath::FontsDirectory)
        .value("IconsDirectory", FileSystem::ResourcePath::IconsDirectory)
        .value("ImagesDirectory", FileSystem::ResourcePath::ImagesDirectory)
        .value("ModelsDirectory", FileSystem::ResourcePath::ModelsDirectory)
        .value("ShadersDirectory", FileSystem::ResourcePath::ShadersDirectory)
        .value("TextDirectory", FileSystem::ResourcePath::TextDirectory);

    py::class_<FileSystem::Path>(m, "Path")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def(py::init<const FileSystem::Path&, const std::string&>())
        .def(py::init<const std::string&, const std::string&>())
        .def("get", &FileSystem::Path::get)
        .def("set", &FileSystem::Path::set)
        .def("append", &FileSystem::Path::append)
        .def("isEmpty", &FileSystem::Path::isEmpty)
        .def("isRoot", &FileSystem::Path::isRoot)
        .def("cdUp", &FileSystem::Path::cdUp)
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
        .def_static("getTemp", &FileSystem::Path::getTemp);
}
