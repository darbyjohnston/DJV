// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/OpenEXR.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace OpenEXR
            {
                struct Write::Private
                {
                    Options options;
                };

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {
                    _finish();
                }

                std::shared_ptr<Write> Write::create(
                    const FileSystem::FileInfo& fileInfo,
                    const Info& info,
                    const WriteOptions& writeOptions,
                    const Options& options,
                    const std::shared_ptr<TextSystem>& textSystem,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_p->options = options;
                    out->_init(fileInfo, info, writeOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                namespace
                {
                    struct File
                    {
                        ~File()
                        {
                        }
                    };
                }

                Image::Type Write::_getImageType(Image::Type value) const
                {
                    Image::Type out = Image::Type::None;
                    switch (value)
                    {
                    case Image::Type::L_U8:
                    case Image::Type::L_U16:    out = Image::Type::L_F16; break;
                    case Image::Type::L_F16:
                    case Image::Type::L_U32:
                    case Image::Type::L_F32:    out = value; break;
                    case Image::Type::LA_U8:    out = Image::Type::LA_F16; break;
                    case Image::Type::LA_U16:
                    case Image::Type::LA_U32:
                    case Image::Type::LA_F32:   out = value; break;
                    case Image::Type::RGB_U8:
                    case Image::Type::RGB_U10:
                    case Image::Type::RGB_U16:  out = Image::Type::RGB_F16; break;
                    case Image::Type::RGB_U32:
                    case Image::Type::RGB_F32:  out = value; break;
                    case Image::Type::RGBA_U8:
                    case Image::Type::RGBA_U16: out = Image::Type::RGBA_F16; break;
                    case Image::Type::RGBA_U32:
                    case Image::Type::RGBA_F32: out = value; break;
                    default: break;
                    }
                    return out;
                }

                Image::Layout Write::_getImageLayout() const
                {
                    Image::Layout out;
                    return out;
                }

                void Write::_write(const std::string& fileName, const std::shared_ptr<Image::Image>& image)
                {
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV
} // namespace djv

