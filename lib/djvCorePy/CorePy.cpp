// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCorePy/CorePy.h>

#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(djvCorePy, m)
{
    wrapAnimation(m);
    wrapBBox(m);
    wrapContext(m);
    wrapIObject(m);
    wrapFrame(m);
    wrapVector(m);

    auto mFileSystem = m.def_submodule("FileSystem");
    wrapFileInfo(mFileSystem);
    wrapPath(mFileSystem);
}
