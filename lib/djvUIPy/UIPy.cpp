// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvUIPy/UIPy.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(djvUIPy, m)
{
    //! \todo Base classes need to be wrapped first?
    wrapWidget(m);

    wrapLabel(m);
    wrapWindow(m);
}
