// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAV/PNG.h>

#include <djvAV/PNGFunc.h>

extern "C"
{
    void djvPngError(png_structp in, png_const_charp msg)
    {
        auto error = reinterpret_cast<djv::AV::IO::PNG::ErrorStruct *>(png_get_error_ptr(in));
        error->messages.push_back(msg);
        longjmp(png_jmpbuf(in), 1);
    }

    void djvPngWarning(png_structp in, png_const_charp msg)
    {
        auto error = reinterpret_cast<djv::AV::IO::PNG::ErrorStruct *>(png_get_error_ptr(in));
        error->messages.push_back(msg);
    }

} // extern "C"

