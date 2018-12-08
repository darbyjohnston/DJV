//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/IO.h>
#include <djvAV/System.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Vector.h>

#include <QGuiApplication>

using namespace djv;

class Context : public Core::Context
{
    DJV_NON_COPYABLE(Context);

protected:
    void _init(int & argc, char ** argv)
    {
        Core::Context::_init(argc, argv);

        if (argc != 3)
        {
            throw std::runtime_error(DJV_TEXT("Usage: djv_convert (input) (output)"));
        }

        auto system = AV::System::create(shared_from_this());
        auto io = getSystemT<AV::IO::System>();
        const auto locale = getSystemT<Core::TextSystem>()->getCurrentLocale();

        auto queue = AV::IO::Queue::create();
        auto read = io->read(argv[1], queue);
        const auto info = read->getInfo().get();

        auto writeQueue = AV::IO::Queue::create();
        auto write = io->write(argv[2], info, queue);
        write->wait();
    }

    Context()
    {}

public:
    static std::shared_ptr<Context> create(int & argc, char ** argv)
    {
        auto out = std::shared_ptr<Context>(new Context);
        out->_init(argc, argv);
        return out;
    }
};

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        QGuiApplication app(argc, argv);
        Context::create(argc, argv)->exit();
    }
    catch (const std::exception & error)
    {
        std::cout << Core::format(error) << std::endl;
    }
    return r;
}
