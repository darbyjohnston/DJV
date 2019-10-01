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
#include <memory>
#include <vector>

namespace djv
{
    namespace Core
    {
        template<typename T>
        class IListSubject;

        //! This value represents an invalid index.
        static const size_t invalidListIndex = static_cast<size_t>(-1);

        //! This class provides a list observer.
        template<typename T>
        class ListObserver : public std::enable_shared_from_this<ListObserver<T> >
        {
            DJV_NON_COPYABLE(ListObserver);

        protected:
            void _init(
                const std::weak_ptr<IListSubject<T> > &,
                const std::function<void(const std::vector<T> &)> &);

            ListObserver();

        public:
            ~ListObserver();

            //! Create a new list observer.
            static std::shared_ptr<ListObserver<T> > create(
                const std::weak_ptr<IListSubject<T> > &,
                const std::function<void(const std::vector<T> &)> &);

            //! Execute the callback.
            void doCallback(const std::vector<T> &);

        private:
            std::function<void(const std::vector<T> &)> _callback;
            std::weak_ptr<IListSubject<T> > _subject;
        };

        //! This class provides an interface for a list subject.
        template<typename T>
        class IListSubject
        {
        public:
            virtual ~IListSubject() = 0;

            //! Get the list.
            virtual const std::vector<T> & get() const = 0;

            //! Get the list size.
            virtual size_t getSize() const = 0;

            //! Get whether the list is empty.
            virtual bool isEmpty() const = 0;

            //! Get a list item.
            virtual const T & getItem(size_t) const = 0;

            //! Get whether the list contains the given item.
            virtual bool contains(const T &) const = 0;

            //! Get the index of the given item.
            virtual size_t indexOf(const T &) const = 0;

        protected:
            void _add(const std::weak_ptr<ListObserver<T> > &);
            void _remove(ListObserver<T> *);

            std::vector<std::weak_ptr<ListObserver<T> > > _observers;

            friend ListObserver<T>;
        };

        //! This class provides a list subject.
        template<typename T>
        class ListSubject : public IListSubject<T>
        {
            DJV_NON_COPYABLE(ListSubject);
            
        protected:
            ListSubject();
            explicit ListSubject(const std::vector<T> &);

        public:
            //! Create a new list subject.
            static std::shared_ptr<ListSubject<T> > create();

            //! Create a new list subject with the given value.
            static std::shared_ptr<ListSubject<T> > create(const std::vector<T> &);
            
            //! Set the list.
            void setAlways(const std::vector<T> &);

            //! Set the list only if it has changed.
            bool setIfChanged(const std::vector<T> &);

            //! Clear the list.
            void clear();

            //! Set a list item.
            void setItem(size_t, const T &);

            //! Set a list item only if it has changed.
            void setItemOnlyIfChanged(size_t, const T &);

            //! Append a list item.
            void pushBack(const T &);

            //! Remove an item.
            void removeItem(size_t);

            const std::vector<T> & get() const override;
            size_t getSize() const override;
            bool isEmpty() const override;
            const T & getItem(size_t) const override;
            bool contains(const T &) const override;
            size_t indexOf(const T &) const override;

        private:
            std::vector<T> _value;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/ListObserverInline.h>
