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

#pragma once

#include <djvCore/ISystem.h>

namespace djv
{
    namespace Core
    {
        class IObject;

        namespace Event
        {
            struct PointerInfo;

            class Locale;
            class PointerMove;
            class Update;

            class IEventSystem : public ISystem
            {
                DJV_NON_COPYABLE(IEventSystem);

            protected:
                void _init(const std::string & systemName, Context *);
                IEventSystem();

            public:
                virtual ~IEventSystem() = 0;

                const std::shared_ptr<IObject> & getRootObject() const;
                void setRootObject(const std::shared_ptr<IObject> &);

            protected:
                void _tick(float dt) override;

            private:
                void _getFirstTick(const std::shared_ptr<IObject> &, std::vector<std::shared_ptr<IObject> > &);
                void _updateRecursive(const std::shared_ptr<IObject> &, Update &);
                void _locale(Locale &);
                void _localeRecursive(const std::shared_ptr<IObject> &, Locale &);

                DJV_PRIVATE();
            };

        } // namespace Event
    } // namespace Core
} // namespace djv
