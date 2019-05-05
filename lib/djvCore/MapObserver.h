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

#pragma once

#include <djvCore/Core.h>

#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace djv
{
    namespace Core
    {
        template<typename T, typename U>
        class IMapSubject;

        //! This class provides a map observer.
        template<typename T, typename U>
        class MapObserver : public std::enable_shared_from_this<MapObserver<T, U> >
        {
            DJV_NON_COPYABLE(MapObserver);

            inline void _init(
                const std::weak_ptr<IMapSubject<T, U> > &,
                const std::function<void(const std::map<T, U> &)> &);

            inline MapObserver();

        public:
            inline ~MapObserver();

            //! Create a new map observer.
            static inline std::shared_ptr<MapObserver<T, U> > create(
                const std::weak_ptr<IMapSubject<T, U> > &,
                const std::function<void(const std::map<T, U> &)> &);

            //! Execute the callback.
            inline void doCallback(const std::map<T, U> &);

        private:
            std::function<void(const std::map<T, U> &)> _callback;
            std::weak_ptr<IMapSubject<T, U> > _subject;
        };

        //! This class provides an interface for a map subject.
        template<typename T, typename U>
        class IMapSubject
        {
        public:
            inline virtual ~IMapSubject() = 0;

            //! Get the map.
            virtual const std::map<T, U> & get() const = 0;

            //! Get the map size.
            virtual size_t getSize() const = 0;

            //! Get whether the map is empty.
            virtual bool isEmpty() const = 0;

            //! Get whether the given key exists.
            virtual bool hasKey(const T &) = 0;

            //! Get a map item.
            virtual const U & getItem(const T &) const = 0;

        protected:
            inline void _add(const std::weak_ptr<MapObserver<T, U> > &);
            inline void _remove(MapObserver<T, U> *);

            std::vector<std::weak_ptr<MapObserver<T, U> > > _observers;

            friend MapObserver<T, U>;
        };

        //! This class provides a map subject.
        template<typename T, typename U>
        class MapSubject : public IMapSubject<T, U>
        {
            DJV_NON_COPYABLE(MapSubject);

            inline MapSubject();
            inline MapSubject(const std::map<T, U> &);

        public:
            //! Create a new map subject.
            static inline std::shared_ptr<MapSubject<T, U> > create();

            //! Create a new map subject with the given value.
            static inline std::shared_ptr<MapSubject<T, U> > create(const std::map<T, U> &);

            //! Set the map.
            inline void setAlways(const std::map<T, U> &);

            //! Set the map only if it has changed.
            inline bool setIfChanged(const std::map<T, U> &);

            //! Clear the map.
            inline void clear();

            //! Set a map item.
            inline void setItem(const T &, const U &);

            //! Set a map item only if it has changed.
            inline void setItemOnlyIfChanged(const T &, const U &);

            inline const std::map<T, U> & get() const override;
            inline size_t getSize() const override;
            inline bool isEmpty() const override;
            inline bool hasKey(const T &) override;
            inline const U & getItem(const T &) const override;

        private:
            std::map<T, U> _value;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/MapObserverInline.h>
