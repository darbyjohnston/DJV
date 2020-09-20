// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/FileInfoPrivate.h>

#include <djvSystem/PathFunc.h>

#include <djvMath/FrameNumberFunc.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            Info::Info()
            {}

            Info::Info(const Path& path, bool stat)
            {
                setPath(path, stat);
            }

            Info::Info(const std::string& path, bool stat)
            {
                setPath(Path(path), stat);
            }

            Info::Info(const Path& path, Type type, const Math::Frame::Sequence& sequence, bool stat)
            {
                setPath(path, type, sequence, stat);
            }
            
            void Info::setPath(const Path& value, bool stat)
            {
                _path           = value;
                _exists         = false;
                _type           = Type::File;
                _size           = 0;
                _user           = 0;
                _permissions    = 0;
                _time           = 0;
                _sequence       = Math::Frame::Sequence();

                // Get information from the file system.
                if (stat)
                {
                    this->stat();
                }
            }

            void Info::setPath(const Path& value, Type type, const Math::Frame::Sequence& sequence, bool stat)
            {
                _path           = value;
                _exists         = false;
                _type           = type;
                _size           = 0;
                _user           = 0;
                _permissions    = 0;
                _time           = 0;
                _sequence       = sequence;

                // Get information from the file system.
                if (stat)
                {
                    this->stat();
                }
            }
            
            std::string Info::getFileName(Math::Frame::Number frame, bool path) const
            {
                std::stringstream ss;
                if (_path.isRoot())
                {
                    ss << _path;
                }
                else
                {
                    if (path)
                    {
                        ss << _path.getDirectoryName();
                    }
                    ss << _path.getBaseName();
                    if (Type::Sequence == _type && _sequence.isValid() && frame != Math::Frame::invalid)
                    {
                        ss << Math::Frame::toString(frame, _sequence.getPad());
                    }
                    else if (Type::Sequence == _type && _sequence.isValid())
                    {
                        ss << _sequence;
                    }
                    else
                    {
                        ss << _path.getNumber();
                    }
                    ss << _path.getExtension();
                }
                return ss.str();
            }

            void Info::setSequence(const Math::Frame::Sequence& in)
            {
                _sequence = in;
                std::stringstream s;
                s << _sequence;
                _path.setNumber(s.str());
            }

            bool Info::addToSequence(const Info& value)
            {
                if (isCompatible(value))
                {
                    if (_type != Type::Sequence)
                    {
                        _sequence = _parseSequence(_path.getNumber());
                        if (_sequence.isValid())
                        {
                            _type = Type::Sequence;
                        }
                    }
                    const Math::Frame::Sequence sequence = _parseSequence(value.getPath().getNumber());
                    for (const auto& range : sequence.getRanges())
                    {
                        _sequence.add(range);
                    }
                    {
                        std::stringstream ss;
                        ss << _sequence;
                        _path.setNumber(ss.str());
                    }
                    if (sequence.getPad() > _sequence.getPad())
                    {
                        _sequence.setPad(sequence.getPad());
                    }
                    _size += value._size;
                    if (value._user > _user)
                    {
                        _user = value._user;
                    }
                    if (value._time > _time)
                    {
                        _time = value._time;
                    }
                    return true;
                }
                return false;
            }
            
            Math::Frame::Sequence Info::_parseSequence(const std::string& number)
            {
                Math::Frame::Sequence out;
                std::stringstream ss(number);
                ss >> out;
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

