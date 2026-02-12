/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2017 John Wellbelove

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

#ifndef GDUT_MESSAGE_TIMER_INCLUDED
#define GDUT_MESSAGE_TIMER_INCLUDED

#include "platform.hpp"
#include "nullptr.hpp"
#include "message_types.hpp"
#include "message.hpp"
#include "message_router.hpp"
#include "message_bus.hpp"
#include "static_assert.hpp"
#include "timer.hpp"
#include "atomic.hpp"
#include "algorithm.hpp"
#include "delegate.hpp"

#include <stdint.h>

#if defined(GDUT_IN_UNIT_TEST) && GDUT_NOT_USING_STL
  #define GDUT_DISABLE_TIMER_UPDATES
  #define GDUT_ENABLE_TIMER_UPDATES
  #define GDUT_TIMER_UPDATES_ENABLED true

  #undef GDUT_MESSAGE_TIMER_USE_ATOMIC_LOCK
  #undef GDUT_MESSAGE_TIMER_USE_INTERRUPT_LOCK
#else
  #if !defined(GDUT_MESSAGE_TIMER_USE_ATOMIC_LOCK) && !defined(GDUT_MESSAGE_TIMER_USE_INTERRUPT_LOCK)
    #error GDUT_MESSAGE_TIMER_USE_ATOMIC_LOCK or GDUT_MESSAGE_TIMER_USE_INTERRUPT_LOCK not defined
  #endif

  #if defined(GDUT_MESSAGE_TIMER_USE_ATOMIC_LOCK) && defined(GDUT_MESSAGE_TIMER_USE_INTERRUPT_LOCK)
    #error Only define one of GDUT_MESSAGE_TIMER_USE_ATOMIC_LOCK or GDUT_MESSAGE_TIMER_USE_INTERRUPT_LOCK
  #endif

  #if defined(GDUT_MESSAGE_TIMER_USE_ATOMIC_LOCK)
    #define GDUT_DISABLE_TIMER_UPDATES (++process_semaphore)
    #define GDUT_ENABLE_TIMER_UPDATES  (--process_semaphore)
    #define GDUT_TIMER_UPDATES_ENABLED (process_semaphore.load() == 0)
  #endif

  #if defined(GDUT_MESSAGE_TIMER_USE_INTERRUPT_LOCK)
    #if !defined(GDUT_MESSAGE_TIMER_DISABLE_INTERRUPTS) || !defined(GDUT_MESSAGE_TIMER_ENABLE_INTERRUPTS)
      #error GDUT_MESSAGE_TIMER_DISABLE_INTERRUPTS and/or GDUT_MESSAGE_TIMER_ENABLE_INTERRUPTS not defined
    #endif

    #define GDUT_DISABLE_TIMER_UPDATES GDUT_MESSAGE_TIMER_DISABLE_INTERRUPTS
    #define GDUT_ENABLE_TIMER_UPDATES  GDUT_MESSAGE_TIMER_ENABLE_INTERRUPTS
    #define GDUT_TIMER_UPDATES_ENABLED true
  #endif
#endif

namespace gdut
{
  //*************************************************************************
  /// The configuration of a timer.
  struct message_timer_data
  {
    //*******************************************
    message_timer_data()
      : p_message(GDUT_NULLPTR),
        p_router(GDUT_NULLPTR),
        period(0),
        delta(gdut::timer::state::Inactive),
        destination_router_id(gdut::imessage_bus::ALL_MESSAGE_ROUTERS),
        id(gdut::timer::id::NO_TIMER),
        previous(gdut::timer::id::NO_TIMER),
        next(gdut::timer::id::NO_TIMER),
        repeating(true)
    {
    }

    //*******************************************
    message_timer_data(gdut::timer::id::type     id_,
                       const gdut::imessage&     message_,
                       gdut::imessage_router&    irouter_,
                       uint32_t                 period_,
                       bool                     repeating_,
                       gdut::message_router_id_t destination_router_id_ = gdut::imessage_bus::ALL_MESSAGE_ROUTERS)
      : p_message(&message_),
        p_router(&irouter_),
        period(period_),
        delta(gdut::timer::state::Inactive),
        destination_router_id(destination_router_id_),
        id(id_),
        previous(gdut::timer::id::NO_TIMER),
        next(gdut::timer::id::NO_TIMER),
        repeating(repeating_)
    {
    }

    //*******************************************
    /// Returns true if the timer is active.
    //*******************************************
    bool is_active() const
    {
      return delta != gdut::timer::state::Inactive;
    }

    //*******************************************
    /// Sets the timer to the inactive state.
    //*******************************************
    void set_inactive()
    {
      delta = gdut::timer::state::Inactive;
    }

    const gdut::imessage*     p_message;
    gdut::imessage_router*    p_router;
    uint32_t                 period;
    uint32_t                 delta;
    gdut::message_router_id_t destination_router_id;
    gdut::timer::id::type     id;
    uint_least8_t            previous;
    uint_least8_t            next;
    bool                     repeating;

  private:

    // Disabled.
    message_timer_data(const message_timer_data& other);
    message_timer_data& operator =(const message_timer_data& other);
  };

  namespace private_message_timer
  {
    //*************************************************************************
    /// A specialised intrusive linked list for timer data.
    //*************************************************************************
    class list
    {
    public:

      //*******************************
      list(gdut::message_timer_data* ptimers_)
        : head(gdut::timer::id::NO_TIMER),
          tail(gdut::timer::id::NO_TIMER),
          current(gdut::timer::id::NO_TIMER),
          ptimers(ptimers_)
      {
      }

      //*******************************
      bool empty() const
      {
        return head == gdut::timer::id::NO_TIMER;
      }

      //*******************************
      // Inserts the timer at the correct delta position
      //*******************************
      void insert(gdut::timer::id::type id_)
      {
        gdut::message_timer_data& timer = ptimers[id_];

        if (head == gdut::timer::id::NO_TIMER)
        {
          // No entries yet.
          head = id_;
          tail = id_;
          timer.previous = gdut::timer::id::NO_TIMER;
          timer.next     = gdut::timer::id::NO_TIMER;
        }
        else
        {
          // We already have entries.
          gdut::timer::id::type test_id = begin();

          while (test_id != gdut::timer::id::NO_TIMER)
          {
            gdut::message_timer_data& test = ptimers[test_id];

            // Find the correct place to insert.
            if (timer.delta <= test.delta)
            {
              if (test.id == head)
              {
                head = timer.id;
              }

              // Insert before test.
              timer.previous = test.previous;
              test.previous  = timer.id;
              timer.next     = test.id;

              // Adjust the next delta to compensate.
              test.delta -= timer.delta;

              if (timer.previous != gdut::timer::id::NO_TIMER)
              {
                ptimers[timer.previous].next = timer.id;
              }
              break;
            }
            else
            {
              timer.delta -= test.delta;
            }

            test_id = next(test_id);
          }

          // Reached the end?
          if (test_id == gdut::timer::id::NO_TIMER)
          {
            // Tag on to the tail.
            ptimers[tail].next = timer.id;
            timer.previous     = tail;
            timer.next         = gdut::timer::id::NO_TIMER;
            tail               = timer.id;
          }
        }
      }

      //*******************************
      void remove(gdut::timer::id::type id_, bool has_expired)
      {
        gdut::message_timer_data& timer = ptimers[id_];

        if (head == id_)
        {
          head = timer.next;
        }
        else
        {
          ptimers[timer.previous].next = timer.next;
        }

        if (tail == id_)
        {
          tail = timer.previous;
        }
        else
        {
          ptimers[timer.next].previous = timer.previous;
        }

        if (!has_expired)
        {
          // Adjust the next delta.
          if (timer.next != gdut::timer::id::NO_TIMER)
          {
            ptimers[timer.next].delta += timer.delta;
          }
        }

        timer.previous = gdut::timer::id::NO_TIMER;
        timer.next     = gdut::timer::id::NO_TIMER;
        timer.delta    = gdut::timer::state::Inactive;
      }

      //*******************************
      gdut::message_timer_data& front()
      {
        return ptimers[head];
      }

      //*******************************
      const gdut::message_timer_data& front() const
      {
        return ptimers[head];
      }

      //*******************************
      gdut::timer::id::type begin()
      {
        current = head;
        return current;
      }

      //*******************************
      gdut::timer::id::type previous(gdut::timer::id::type last)
      {
        current = ptimers[last].previous;
        return current;
      }

      //*******************************
      gdut::timer::id::type next(gdut::timer::id::type last)
      {
        current = ptimers[last].next;
        return current;
      }

      //*******************************
      void clear()
      {
        gdut::timer::id::type id = begin();

        while (id != gdut::timer::id::NO_TIMER)
        {
          gdut::message_timer_data& timer = ptimers[id];
          id = next(id);
          timer.next = gdut::timer::id::NO_TIMER;
        }

        head    = gdut::timer::id::NO_TIMER;
        tail    = gdut::timer::id::NO_TIMER;
        current = gdut::timer::id::NO_TIMER;
      }

    private:

      gdut::timer::id::type head;
      gdut::timer::id::type tail;
      gdut::timer::id::type current;

      gdut::message_timer_data* const ptimers;
    };
  }

  //***************************************************************************
  /// Interface for message timer
  //***************************************************************************
  class imessage_timer
  {
  public:

    typedef gdut::delegate<void(gdut::timer::id::type)> event_callback_type;

    //*******************************************
    /// Register a timer.
    //*******************************************
    gdut::timer::id::type register_timer(const gdut::imessage&     message_,
                                        gdut::imessage_router&    router_,
                                        uint32_t                 period_,
                                        bool                     repeating_,
                                        gdut::message_router_id_t destination_router_id_ = gdut::imessage_router::ALL_MESSAGE_ROUTERS)
    {
      gdut::timer::id::type id = gdut::timer::id::NO_TIMER;

      bool is_space = (registered_timers < Max_Timers);

      if (is_space)
      {
        // There's no point adding null message routers.
        if (!router_.is_null_router())
        {
          // Search for the free space.
          for (uint_least8_t i = 0U; i < Max_Timers; ++i)
          {
            gdut::message_timer_data& timer = timer_array[i];

            if (timer.id == gdut::timer::id::NO_TIMER)
            {
              // Create in-place.
              new (&timer) message_timer_data(i, message_, router_, period_, repeating_, destination_router_id_);
              ++registered_timers;
              id = i;
              break;
            }
          }
        }
      }

      return id;
    }

    //*******************************************
    /// Unregister a timer.
    //*******************************************
    bool unregister_timer(gdut::timer::id::type id_)
    {
      bool result = false;

      if (id_ != gdut::timer::id::NO_TIMER)
      {
        gdut::message_timer_data& timer = timer_array[id_];

        if (timer.id != gdut::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            GDUT_DISABLE_TIMER_UPDATES;
            active_list.remove(timer.id, true);
            remove_callback.call_if(timer.id);
            GDUT_ENABLE_TIMER_UPDATES;
          }

          // Reset in-place.
          new (&timer) message_timer_data();
          --registered_timers;

          result = true;
        }
      }

      return result;
    }

    //*******************************************
    /// Enable/disable the timer.
    //*******************************************
    void enable(bool state_)
    {
      enabled = state_;
    }

    //*******************************************
    /// Get the enable/disable state.
    //*******************************************
    bool is_running() const
    {
      return enabled;
    }

    //*******************************************
    /// Clears the timer of data.
    //*******************************************
    void clear()
    {
      GDUT_DISABLE_TIMER_UPDATES;
      active_list.clear();
      GDUT_ENABLE_TIMER_UPDATES;

      for (int i = 0; i < Max_Timers; ++i)
      {
        new (&timer_array[i]) message_timer_data();
      }

      registered_timers = 0;
    }

    //*******************************************
    // Called by the timer service to indicate the
    // amount of time that has elapsed since the last successful call to 'tick'.
    // Returns true if the tick was processed,
    // false if not.
    //*******************************************
    bool tick(uint32_t count)
    {
      if (enabled)
      {
        if (GDUT_TIMER_UPDATES_ENABLED)
        {
          // We have something to do?
          bool has_active = !active_list.empty();

          if (has_active)
          {
            while (has_active && (count >= active_list.front().delta))
            {
              gdut::message_timer_data& timer = active_list.front();

              count -= timer.delta;

              active_list.remove(timer.id, true);
              remove_callback.call_if(timer.id);

              if (timer.repeating)
              {
                timer.delta = timer.period;
                active_list.insert(timer.id);
                insert_callback.call_if(timer.id);
              }

              if (timer.p_router != GDUT_NULLPTR)
              {
                timer.p_router->receive(timer.destination_router_id, *(timer.p_message));
              }

              has_active = !active_list.empty();
            }

            if (has_active)
            {
              // Subtract any remainder from the next due timeout.
              active_list.front().delta -= count;
            }
          }

          return true;
        }
      }

      return false;
    }

    //*******************************************
    /// Starts a timer.
    //*******************************************
    bool start(gdut::timer::id::type id_, bool immediate_ = false)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != gdut::timer::id::NO_TIMER)
      {
        gdut::message_timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != gdut::timer::id::NO_TIMER)
        {
          // Has a valid period.
          if (timer.period != gdut::timer::state::Inactive)
          {
            GDUT_DISABLE_TIMER_UPDATES;
            if (timer.is_active())
            {
              active_list.remove(timer.id, false);
              remove_callback.call_if(timer.id);
            }

            timer.delta = immediate_ ? 0 : timer.period;
            active_list.insert(timer.id);
            insert_callback.call_if(timer.id);
            GDUT_ENABLE_TIMER_UPDATES;

            result = true;
          }
        }
      }

      return result;
    }

    //*******************************************
    /// Stops a timer.
    //*******************************************
    bool stop(gdut::timer::id::type id_)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != gdut::timer::id::NO_TIMER)
      {
        gdut::message_timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != gdut::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            GDUT_DISABLE_TIMER_UPDATES;
            active_list.remove(timer.id, false);
            remove_callback.call_if(timer.id);
            GDUT_ENABLE_TIMER_UPDATES;
          }

          result = true;
        }
      }

      return result;
    }

    //*******************************************
    /// Sets a timer's period.
    //*******************************************
    bool set_period(gdut::timer::id::type id_, uint32_t period_)
    {
      if (stop(id_))
      {
        timer_array[id_].period = period_;
        return true;
      }

      return false;
    }

    //*******************************************
    /// Sets a timer's mode.
    //*******************************************
    bool set_mode(gdut::timer::id::type id_, bool repeating_)
    {
      if (stop(id_))
      {
        timer_array[id_].repeating = repeating_;
        return true;
      }

      return false;
    }

    //*******************************************
    /// Check if there is an active timer.
    //*******************************************
    bool has_active_timer() const
    {
      GDUT_DISABLE_TIMER_UPDATES;
      bool result = !active_list.empty();
      GDUT_ENABLE_TIMER_UPDATES;

      return result;
    }

    //*******************************************
    /// Get the time to the next timer event.
    /// Returns gdut::timer::interval::No_Active_Interval if there is no active timer.
    //*******************************************
    uint32_t time_to_next() const
    {
      uint32_t delta = static_cast<uint32_t>(gdut::timer::interval::No_Active_Interval);

      GDUT_DISABLE_TIMER_UPDATES;
      if (!active_list.empty())
      {        
        delta = active_list.front().delta;
      }
      GDUT_ENABLE_TIMER_UPDATES;

      return delta;
    }

    //*******************************************
    /// Set a callback when a timer is inserted on list
    //*******************************************
    void set_insert_callback(event_callback_type insert_)
    {
      insert_callback = insert_;
    }

    //*******************************************
    /// Set a callback when a timer is removed from list
    //*******************************************
    void set_remove_callback(event_callback_type remove_)
    {
      remove_callback = remove_;
    }

    //*******************************************
    void clear_insert_callback()
    {
      insert_callback.clear();
    }

    //*******************************************
    void clear_remove_callback()
    {
      remove_callback.clear();
    }

  protected:

    //*******************************************
    /// Constructor.
    //*******************************************
    imessage_timer(message_timer_data* const timer_array_, const uint_least8_t  Max_Timers_)
      : timer_array(timer_array_),
        active_list(timer_array_),
        enabled(false),
#if defined(GDUT_MESSAGE_TIMER_USE_ATOMIC_LOCK)
        process_semaphore(0),
#endif
        registered_timers(0),
        Max_Timers(Max_Timers_)
    {
    }

    //*******************************************
    /// Destructor.
    //*******************************************
    ~imessage_timer()
    {
    }

  private:

    // The array of timer data structures.
    message_timer_data* const timer_array;

    // The list of active timers.
    private_message_timer::list active_list;

    bool enabled;

#if defined(GDUT_MESSAGE_TIMER_USE_ATOMIC_LOCK)
  
#if defined(GDUT_TIMER_SEMAPHORE_TYPE)
  typedef GDUT_TIMER_SEMAPHORE_TYPE timer_semaphore_t;
#else
  #if GDUT_HAS_ATOMIC
    typedef gdut::atomic_uint16_t timer_semaphore_t;
  #else
    #error No atomic type available
  #endif
#endif

    mutable gdut::timer_semaphore_t process_semaphore;
#endif
    uint_least8_t registered_timers;

    event_callback_type insert_callback;
    event_callback_type remove_callback;

  public:

    const uint_least8_t Max_Timers;
  };

  //***************************************************************************
  /// The message timer
  //***************************************************************************
  template <uint_least8_t Max_Timers_>
  class message_timer : public gdut::imessage_timer
  {
  public:

    GDUT_STATIC_ASSERT(Max_Timers_ <= 254, "No more than 254 timers are allowed");

    //*******************************************
    /// Constructor.
    //*******************************************
    message_timer()
      : imessage_timer(timer_array, Max_Timers_)
    {
    }

  private:

    message_timer_data timer_array[Max_Timers_];
  };
}

#undef GDUT_DISABLE_TIMER_UPDATES
#undef GDUT_ENABLE_TIMER_UPDATES
#undef GDUT_TIMER_UPDATES_ENABLED

#endif
