//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvSystemTest.cpp

#include <djvSystemTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvSystem.h>

#include <QCoreApplication>
#include <QStringList>

void djvSystemTest::run(int & argc, char ** argv)
{
    DJV_DEBUG("djvSystemTest::run");
    
    QCoreApplication app(argc, argv);
    
    searchPath();
    drives();
}

void djvSystemTest::searchPath()
{
    DJV_DEBUG("djvSystemTest::searchPath");

    const QString djvPathValue = djvSystem::env(djvSystem::djvPathEnv());
    
    if (! djvPathValue.length())
    {
        djvSystem::setEnv(djvSystem::djvPathEnv(), "djvPath1:djvPath2");
    }
    
    const QString ldLibPathValue = djvSystem::env(djvSystem::ldLibPathEnv());
    
    if (! ldLibPathValue.length())
    {
        djvSystem::setEnv(djvSystem::ldLibPathEnv(), "ldLibPath1:ldLibPath2");
    }
    
    DJV_DEBUG_PRINT(djvSystem::searchPath());
    
    djvSystem::setEnv(djvSystem::ldLibPathEnv(), ldLibPathValue);
    djvSystem::setEnv(djvSystem::ldLibPathEnv(), ldLibPathValue);
}

void djvSystemTest::drives()
{
    DJV_DEBUG("djvSystemTest::drives");
    
    DJV_DEBUG_PRINT("drives = " << djvSystem::drives());
}

