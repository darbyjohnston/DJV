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

#include <djvCore/FileInfo.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace djv::Core;

namespace py = pybind11;

void wrapFileInfo(pybind11::module& m)
{
    py::enum_<FileSystem::FileType>(m, "FileType")
        .value("File", FileSystem::FileType::File)
        .value("Sequence", FileSystem::FileType::Sequence)
        .value("Directory", FileSystem::FileType::Directory);

    py::enum_<FileSystem::DirectoryListSort>(m, "DirectoryListSort")
        .value("Name", FileSystem::DirectoryListSort::Name)
        .value("Size", FileSystem::DirectoryListSort::Size)
        .value("Time", FileSystem::DirectoryListSort::Time);

    py::class_<FileSystem::DirectoryListOptions>(m, "DirectoryListOptions")
        .def(py::init<>())
        .def_readwrite("fileSequences", &FileSystem::DirectoryListOptions::fileSequences)
        .def_readwrite("fileSequenceExtensions", &FileSystem::DirectoryListOptions::fileSequenceExtensions)
        .def_readwrite("showHidden", &FileSystem::DirectoryListOptions::showHidden)
        .def_readwrite("sort", &FileSystem::DirectoryListOptions::sort)
        .def_readwrite("reverseSort", &FileSystem::DirectoryListOptions::reverseSort)
        .def_readwrite("sortDirectoriesFirst", &FileSystem::DirectoryListOptions::sortDirectoriesFirst)
        .def_readwrite("filter", &FileSystem::DirectoryListOptions::filter);

    py::class_<FileSystem::FileInfo>(m, "FileInfo")
        .def(py::init<>())
        .def(py::init<const FileSystem::Path&>())
        .def(py::init<const FileSystem::Path&, bool>())
        .def(py::init<const FileSystem::Path&, FileSystem::FileType>())
        .def(py::init<const FileSystem::Path&, FileSystem::FileType, bool>())
        .def(py::init<const std::string&>())
        .def(py::init<const std::string&, bool>())
        .def("getPath", &FileSystem::FileInfo::getPath)
        .def("isEmpty", &FileSystem::FileInfo::isEmpty)
        .def("setPath", (void(FileSystem::FileInfo::*)(const FileSystem::Path&, bool))&FileSystem::FileInfo::setPath, py::arg("path"), py::arg("stat") = true)
        .def("setPath", (void(FileSystem::FileInfo::*)(const FileSystem::Path&, FileSystem::FileType, bool))&FileSystem::FileInfo::setPath, py::arg("path"), py::arg("fileType"), py::arg("stat") = true)
        .def("getFileName", &FileSystem::FileInfo::getFileName, py::arg("frame") = Frame::invalid, py::arg("path") = true)
        .def("doesExist", &FileSystem::FileInfo::doesExist)
        .def("getType", &FileSystem::FileInfo::getType)
        .def("getSize", &FileSystem::FileInfo::getSize)
        .def("getUser", &FileSystem::FileInfo::getUser)
        .def("getPermissions", &FileSystem::FileInfo::getPermissions)
        .def("getTime", &FileSystem::FileInfo::getTime)
        .def("stat", &FileSystem::FileInfo::stat)
        .def("getSequence", &FileSystem::FileInfo::getSequence)
        .def("setSequence", &FileSystem::FileInfo::setSequence)
        .def("evalSequence", &FileSystem::FileInfo::evalSequence)
        .def("sortSequence", &FileSystem::FileInfo::sortSequence)
        .def("isSequenceValid", &FileSystem::FileInfo::isSequenceValid)
        .def("isSequenceWildcard", &FileSystem::FileInfo::isSequenceWildcard)
        .def("sequenceContains", &FileSystem::FileInfo::sequenceContains)
        .def("addToSequence", &FileSystem::FileInfo::addToSequence)
        .def_static("directoryList", &FileSystem::FileInfo::directoryList, py::arg("path"), py::arg("options") = FileSystem::DirectoryListOptions())
        .def_static("getFileSequence", &FileSystem::FileInfo::getFileSequence);
}
