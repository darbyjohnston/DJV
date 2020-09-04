// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <iostream>

namespace djv
{
    namespace Test
    {
        struct ITest::Private
        {
            std::weak_ptr<Core::Context> context;
            std::shared_ptr<Core::TextSystem> textSystem;
            std::string name;
        };
        
        ITest::ITest(const std::string& name, const std::shared_ptr<Core::Context>& context) :
            _p(new Private)
        {
            _p->context = context;
            _p->textSystem = context->getSystemT<Core::TextSystem>();
            _p->name = name;
        }
        
        ITest::~ITest()
        {}
        
        const std::weak_ptr<Core::Context>& ITest::getContext() const
        {
            return _p->context;
        }

        const std::string& ITest::getName() const
        {
            return _p->name;
        }

        std::string ITest::_getText(const std::string& id) const
        {
            DJV_PRIVATE_PTR();
            return p.textSystem ? p.textSystem->getText(id) : id;
        }

        void ITest::_print(const std::string& value)
        {
            std::cout << _p->name << ": " << value << std::endl;
        }
        
    } // namespace Test
} // namespace djv
