//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/JPEG.h>

#include <djvCore/Context.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace JPEG
            {
				struct Plugin::Private
				{
					Settings settings;
				};

                Plugin::Plugin() :
					_p(new Private)
                {}

				Plugin::~Plugin()
				{}

                std::shared_ptr<Plugin> Plugin::create(Context * context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        context->getText(DJV_TEXT("This plugin provides Joint Photographic Experts Group (JPEG) image I/O.")),
                        fileExtensions,
                        context);
                    return out;
                }

				picojson::value Plugin::getOptions() const
				{
					return toJSON(_p->settings);
				}

				void Plugin::setOptions(const picojson::value& value)
				{
					fromJSON(value, _p->settings);
				}

                std::shared_ptr<IRead> Plugin::read(
                    const std::string & fileName,
                    const std::shared_ptr<Queue> & queue) const
                {
                    return Read::create(fileName, queue, _context);
                }

                std::shared_ptr<IWrite> Plugin::write(
                    const std::string & fileName,
                    const Info & info,
                    const std::shared_ptr<Queue> & queue) const
                {
                    return Write::create(fileName, _p->settings, info, queue, _context);
                }

                extern "C"
                {
					void djvJPEGError(j_common_ptr in)
					{
						auto error = reinterpret_cast<JPEGErrorStruct *>(in->err);
						in->err->format_message(in, error->msg);
						::longjmp(error->jump, 1);
					}

					void djvJPEGWarning(j_common_ptr in, int level)
					{
						if (level > 0)
						{
							//! \todo Should we send "trace" messages to the debug log?
							return;
						}
						auto error = reinterpret_cast<JPEGErrorStruct *>(in->err);
						in->err->format_message(in, error->msg);
						::longjmp(error->jump, 1);
					}

                } // extern "C"

            } // namespace JPEG
        } // namespace IO
    } // namespace AV
} // namespace djv

