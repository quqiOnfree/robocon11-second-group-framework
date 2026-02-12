///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2020 John Wellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef GDUT_REFERENCE_COUNTED_MESSAGE_POOL_INCLUDED
#define GDUT_REFERENCE_COUNTED_MESSAGE_POOL_INCLUDED

#include "atomic.hpp"
#include "error_handler.hpp"
#include "imemory_block_allocator.hpp"
#include "ireference_counted_message_pool.hpp"
#include "largest.hpp"
#include "memory.hpp"
#include "message.hpp"
#include "platform.hpp"
#include "reference_counted_message.hpp"
#include "static_assert.hpp"
#include "utility.hpp"

namespace gdut {
//***************************************************************************
/// Exception type for gdut::reference_counted_message_pool
//***************************************************************************
class reference_counted_message_pool_exception : public gdut::exception {
public:
  reference_counted_message_pool_exception(string_type reason_,
                                           string_type file_name_,
                                           numeric_type line_number_)
      : exception(reason_, file_name_, line_number_) {}
};

//***************************************************************************
/// Exception if the allocation failed.
//***************************************************************************
class reference_counted_message_pool_allocation_failure
    : public gdut::reference_counted_message_pool_exception {
public:
  reference_counted_message_pool_allocation_failure(string_type file_name_,
                                                    numeric_type line_number_)
      : reference_counted_message_pool_exception(
            GDUT_ERROR_TEXT("reference_counted_message_pool:allocation failure",
                            GDUT_REFERENCE_COUNTER_MESSAGE_POOL_FILE_ID "A"),
            file_name_, line_number_) {}
};

//***************************************************************************
/// Exception if the release failed.
//***************************************************************************
class reference_counted_message_pool_release_failure
    : public gdut::reference_counted_message_pool_exception {
public:
  reference_counted_message_pool_release_failure(string_type file_name_,
                                                 numeric_type line_number_)
      : reference_counted_message_pool_exception(
            GDUT_ERROR_TEXT("reference_counted_message_pool:release failure",
                            GDUT_REFERENCE_COUNTER_MESSAGE_POOL_FILE_ID "B"),
            file_name_, line_number_) {}
};

//***************************************************************************
/// A pool for allocating reference counted messages.
//***************************************************************************
template <typename TCounter>
class reference_counted_message_pool
    : public gdut::ireference_counted_message_pool {
public:
  //*************************************************************************
  /// Constructor
  //*************************************************************************
  reference_counted_message_pool(
      gdut::imemory_block_allocator &memory_block_allocator_)
      : memory_block_allocator(memory_block_allocator_) {}

#if GDUT_USING_CPP11
  //*************************************************************************
  /// Allocate a reference counted message from the pool.
  //*************************************************************************
  template <typename TMessage, typename... TArgs>
  gdut::reference_counted_message<TMessage, TCounter> *
  allocate(TArgs &&...args) {
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage>::value),
                       "Not a message type");

    typedef gdut::reference_counted_message<TMessage, TCounter> rcm_t;
    typedef rcm_t *prcm_t;

    prcm_t p = GDUT_NULLPTR;

    lock();
    p = static_cast<prcm_t>(memory_block_allocator.allocate(
        sizeof(rcm_t), gdut::alignment_of<rcm_t>::value));
    unlock();

    if (p != GDUT_NULLPTR) {
      ::new (p) rcm_t(*this, gdut::forward<TArgs>(args)...);
    }

    GDUT_ASSERT(
        (p != GDUT_NULLPTR),
        GDUT_ERROR(gdut::reference_counted_message_pool_allocation_failure));

    return p;
  }
#endif

  //*************************************************************************
  /// Allocate a reference counted message from the pool.
  //*************************************************************************
  template <typename TMessage>
  gdut::reference_counted_message<TMessage, TCounter> *
  allocate(const TMessage &message) {
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage>::value),
                       "Not a message type");

    typedef gdut::reference_counted_message<TMessage, TCounter> rcm_t;
    typedef rcm_t *prcm_t;

    prcm_t p = GDUT_NULLPTR;

    lock();
    p = static_cast<prcm_t>(memory_block_allocator.allocate(
        sizeof(rcm_t), gdut::alignment_of<rcm_t>::value));
    unlock();

    if (p != GDUT_NULLPTR) {
      ::new (p) rcm_t(message, *this);
    }

    GDUT_ASSERT(
        (p != GDUT_NULLPTR),
        GDUT_ERROR(gdut::reference_counted_message_pool_allocation_failure));

    return p;
  }

  //*************************************************************************
  /// Allocate a reference counted message from the pool.
  //*************************************************************************
  template <typename TMessage>
  gdut::reference_counted_message<TMessage, TCounter> *allocate() {
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage>::value),
                       "Not a message type");

    typedef gdut::reference_counted_message<TMessage, TCounter> rcm_t;
    typedef rcm_t *prcm_t;

    prcm_t p = GDUT_NULLPTR;

    lock();
    p = static_cast<prcm_t>(memory_block_allocator.allocate(
        sizeof(rcm_t), gdut::alignment_of<rcm_t>::value));
    unlock();

    if (p != GDUT_NULLPTR) {
      ::new (p) rcm_t(*this);
    }

    GDUT_ASSERT(
        (p != GDUT_NULLPTR),
        GDUT_ERROR(gdut::reference_counted_message_pool_allocation_failure));

    return p;
  }

  //*************************************************************************
  /// Destruct a message and send it back to the allocator.
  //*************************************************************************
  void release(const gdut::ireference_counted_message &rcmessage) {
    bool released = false;

    lock();
    if (memory_block_allocator.is_owner_of(&rcmessage)) {
      rcmessage.~ireference_counted_message();
      released = memory_block_allocator.release(&rcmessage);
    }
    unlock();

    GDUT_ASSERT(
        released,
        GDUT_ERROR(gdut::reference_counted_message_pool_release_failure));
  }

#if GDUT_USING_CPP11
  //*****************************************************
  template <typename TMessage1, typename... TMessages>
  struct pool_message_parameters {
  private:
    // Size of the first pool message type.
    static constexpr size_t size1 =
        sizeof(gdut::reference_counted_message<TMessage1, TCounter>);

    // Maximum size of the rest of the pool message types.
    static constexpr size_t size2 =
        pool_message_parameters<TMessages...>::max_size;

    // Size of the first pool message type.
    static constexpr size_t alignment1 = gdut::alignment_of<
        gdut::reference_counted_message<TMessage1, TCounter>>::value;

    // Maximum size of the rest of the pool message types.
    static constexpr size_t alignment2 =
        pool_message_parameters<TMessages...>::max_alignment;

  public:
    // The maximum size.
    static constexpr size_t max_size = (size1 < size2) ? size2 : size1;

    // The maximum alignment.
    static constexpr size_t max_alignment =
        (alignment1 < alignment2) ? alignment2 : alignment1;
  };

  //*****************************************************
  template <typename TMessage1> struct pool_message_parameters<TMessage1> {
  public:
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage not derived from gdut::imessage");

    // The size of this pool message type.
    static constexpr size_t max_size =
        sizeof(gdut::reference_counted_message<TMessage1, TCounter>);

    // The maximum alignment.
    static constexpr size_t max_alignment = gdut::alignment_of<
        gdut::reference_counted_message<TMessage1, TCounter>>::value;
  };

#else
  template <typename TMessage1, typename TMessage2 = TMessage1,
            typename TMessage3 = TMessage1, typename TMessage4 = TMessage1,
            typename TMessage5 = TMessage1, typename TMessage6 = TMessage1,
            typename TMessage7 = TMessage1, typename TMessage8 = TMessage1>
  struct pool_message_parameters {
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage1 not derived from gdut::imessage");
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage2 not derived from gdut::imessage");
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage3 not derived from gdut::imessage");
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage4 not derived from gdut::imessage");
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage5 not derived from gdut::imessage");
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage6 not derived from gdut::imessage");
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage7 not derived from gdut::imessage");
    GDUT_STATIC_ASSERT((gdut::is_base_of<gdut::imessage, TMessage1>::value),
                       "TMessage8 not derived from gdut::imessage");

    static GDUT_CONSTANT size_t max_size = gdut::largest<
        gdut::reference_counted_message<TMessage1, TCounter>,
        gdut::reference_counted_message<TMessage2, TCounter>,
        gdut::reference_counted_message<TMessage3, TCounter>,
        gdut::reference_counted_message<TMessage4, TCounter>,
        gdut::reference_counted_message<TMessage5, TCounter>,
        gdut::reference_counted_message<TMessage6, TCounter>,
        gdut::reference_counted_message<TMessage7, TCounter>,
        gdut::reference_counted_message<TMessage8, TCounter>>::size;

    static GDUT_CONSTANT size_t max_alignment = gdut::largest<
        gdut::reference_counted_message<TMessage1, TCounter>,
        gdut::reference_counted_message<TMessage2, TCounter>,
        gdut::reference_counted_message<TMessage3, TCounter>,
        gdut::reference_counted_message<TMessage4, TCounter>,
        gdut::reference_counted_message<TMessage5, TCounter>,
        gdut::reference_counted_message<TMessage6, TCounter>,
        gdut::reference_counted_message<TMessage7, TCounter>,
        gdut::reference_counted_message<TMessage8, TCounter>>::alignment;
  };

#endif

private:
  /// The raw memory block pool.
  imemory_block_allocator &memory_block_allocator;

  // Should not be copied.
  reference_counted_message_pool(const reference_counted_message_pool &)
      GDUT_DELETE;
  reference_counted_message_pool &
  operator=(const reference_counted_message_pool &) GDUT_DELETE;
};

#if GDUT_USING_CPP11

template <typename TCounter>
template <typename TMessage1, typename... TMessages>
constexpr size_t reference_counted_message_pool<
    TCounter>::pool_message_parameters<TMessage1, TMessages...>::max_size;

template <typename TCounter>
template <typename TMessage1, typename... TMessages>
constexpr size_t reference_counted_message_pool<
    TCounter>::pool_message_parameters<TMessage1, TMessages...>::max_alignment;

#else

template <typename TCounter>
template <typename TMessage1, typename TMessage2, typename TMessage3,
          typename TMessage4, typename TMessage5, typename TMessage6,
          typename TMessage7, typename TMessage8>
const size_t reference_counted_message_pool<TCounter>::pool_message_parameters<
    TMessage1, TMessage2, TMessage3, TMessage4, TMessage5, TMessage6, TMessage7,
    TMessage8>::max_size;

template <typename TCounter>
template <typename TMessage1, typename TMessage2, typename TMessage3,
          typename TMessage4, typename TMessage5, typename TMessage6,
          typename TMessage7, typename TMessage8>
const size_t reference_counted_message_pool<TCounter>::pool_message_parameters<
    TMessage1, TMessage2, TMessage3, TMessage4, TMessage5, TMessage6, TMessage7,
    TMessage8>::max_alignment;

#endif

#if GDUT_USING_CPP11 && GDUT_HAS_ATOMIC
using atomic_counted_message_pool =
    reference_counted_message_pool<gdut::atomic_int>;
#endif
} // namespace gdut

#endif