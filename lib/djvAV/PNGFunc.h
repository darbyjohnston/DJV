// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <png.h>

extern "C"
{
    void djvPngError(png_structp, png_const_charp);
    void djvPngWarning(png_structp, png_const_charp);

} // extern "C"
