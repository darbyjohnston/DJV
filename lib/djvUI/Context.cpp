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

#include <djvUI/Context.h>

#include <djvUI/ProxyStyle.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>

#include <QApplication>

namespace djv
{
    namespace UI
    {
        struct Context::Private
        {
            std::shared_ptr<Style> style;
            QPointer<ProxyStyle> qStyle;
        };

        void Context::_init(int & argc, char ** argv)
        {
            AV::Context::_init(argc, argv);

            SettingsSystem::create(std::dynamic_pointer_cast<Context>(shared_from_this()));
            _p->style = Style::create(std::dynamic_pointer_cast<Context>(shared_from_this()));
            _p->qStyle = new ProxyStyle(std::dynamic_pointer_cast<Context>(shared_from_this()));
            qApp->setStyle(_p->qStyle);
        }

        Context::Context() :
            _p(new Private)
        {}

        Context::~Context()
        {}

        std::shared_ptr<Context> Context::create(int & argc, char ** argv)
        {
            auto out = std::shared_ptr<Context>(new Context);
            out->_init(argc, argv);
            return out;
        }

        const std::shared_ptr<Style> Context::getStyle() const
        {
            return _p->style;
        }

        QPointer<QStyle> Context::getQStyle() const
        {
            return _p->qStyle.data();
        }

    } // namespace UI
} // namespace djv

