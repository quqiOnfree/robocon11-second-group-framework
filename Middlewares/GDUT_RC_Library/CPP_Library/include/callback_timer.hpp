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

#ifndef GDUT_CALLBACK_TIMER_INCLUDED
#define GDUT_CALLBACK_TIMER_INCLUDED

#include "platform.hpp"
#include "algorithm.hpp"
#include "nullptr.hpp"
#include "function.hpp"
#include "static_assert.hpp"
#include "timer.hpp"
#include "atomic.hpp"
#include "error_handler.hpp"
#include "placement_new.hpp"
#include "delegate.hpp"

#include <stdint.h>

#if defined(GDUT_IN_UNIT_TEST) && GDUT_NOT_USING_STL
  #define GDUT_DISABLE_TIMER_UPDATES
  #define GDUT_ENABLE_TIMER_UPDATES
  #define GDUT_TIMER_UPDATES_ENABLED true

  #undef GDUT_CALLBACK_TIMER_USE_ATOMIC_LOCK
  #undef GDUT_CALLBACK_TIMER_USE_INTERRUPT_LOCK
#else
  #if !defined(GDUT_CALLBACK_TIMER_USE_ATOMIC_LOCK) && !defined(GDUT_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
    #error GDUT_CALLBACK_TIMER_USE_ATOMIC_LOCK or GDUT_CALLBACK_TIMER_USE_INTERRUPT_LOCK not defined
  #endif

  #if defined(GDUT_CALLBACK_TIMER_USE_ATOMIC_LOCK) && defined(GDUT_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
    #error Only define one of GDUT_CALLBACK_TIMER_USE_ATOMIC_LOCK or GDUT_CALLBACK_TIMER_USE_INTERRUPT_LOCK
  #endif

  #if defined(GDUT_CALLBACK_TIMER_USE_ATOMIC_LOCK)
    #define GDUT_DISABLE_TIMER_UPDATES (++process_semaphore)
    #define GDUT_ENABLE_TIMER_UPDATES  (--process_semaphore)
    #define GDUT_TIMER_UPDATES_ENABLED (process_semaphore.load() == 0)
  #endif
#endif

#if defined(GDUT_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
  #if !defined(GDUT_CALLBACK_TIMER_DISABLE_INTERRUPTS) || !defined(GDUT_CALLBACK_TIMER_ENABLE_INTERRUPTS)
    #error GDUT_CALLBACK_TIMER_DISABLE_INTERRUPTS and/or GDUT_CALLBACK_TIMER_ENABLE_INTERRUPTS not defined
  #endif

  #define GDUT_DISABLE_TIMER_UPDATES GDUT_CALLBACK_TIMER_DISABLE_INTERRUPTS
  #define GDUT_ENABLE_TIMER_UPDATES  GDUT_CALLBACK_TIMER_ENABLE_INTERRUPTS
  #define GDUT_TIMER_UPDATES_ENABLED true
#endif

namespace gdut
{
  //***************************************************************************
  /// Interface for callback timer
  //***************************************************************************
  class icallback_timer
  {
  public:

    typedef gdut::delegate<void(void)> callback_type;

    typedef gdut::delegate<void(gdut::timer::id::type)> event_callback_type;

    //*******************************************
    /// Register a timer.
    //*******************************************
    gdut::timer::id::type register_timer(void     (*p_callback_)(),
                                        uint32_t period_,
                                        bool     repeating_)
    {
      gdut::timer::id::type id = gdut::timer::id::NO_TIMER;

      bool is_space = (registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // Search for the free space.
        for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
        {
          timer_data& timer = timer_array[i];

          if (timer.id == gdut::timer::id::NO_TIMER)
          {
            // Create in-place.
            new (&timer) timer_data(i, p_callback_, period_, repeating_);
            ++registered_timers;
            id = i;
            break;
          }
        }
      }

      return id;
    }

    //*******************************************
    /// Register a timer.
    //*******************************************
    gdut::timer::id::type register_timer(gdut::ifunction<void>& callback_,
                                        uint32_t              period_,
                                        bool                  repeating_)
    {
      gdut::timer::id::type id = gdut::timer::id::NO_TIMER;

      bool is_space = (registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // Search for the free space.
        for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
        {
          timer_data& timer = timer_array[i];

          if (timer.id == gdut::timer::id::NO_TIMER)
          {
            // Create in-place.
            new (&timer) timer_data(i, callback_, period_, repeating_);
            ++registered_timers;
            id = i;
            break;
          }
        }
      }

      return id;
    }

      //*******************************************
      /// Register a timer.
      //*******************************************
#if GDUT_USING_CPP11
      gdut::timer::id::type register_timer(callback_type& callback_,
                                          uint32_t       period_,
                                          bool           repeating_)
      {
          gdut::timer::id::type id = gdut::timer::id::NO_TIMER;

          bool is_space = (registered_timers < MAX_TIMERS);

          if (is_space)
          {
              // Search for the free space.
              for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
              {
                  timer_data& timer = timer_array[i];

                  if (timer.id == gdut::timer::id::NO_TIMER)
                  {
                      // Create in-place.
                      new (&timer) timer_data(i, callback_, period_, repeating_);
                      ++registered_timers;
                      id = i;
                      break;
                  }
              }
          }

          return id;
      }
#endif

    //*******************************************
    /// Unregister a timer.
    //*******************************************
    bool unregister_timer(gdut::timer::id::type id_)
    {
      bool result = false;

      if (id_ != gdut::timer::id::NO_TIMER)
      {
        timer_data& timer = timer_array[id_];

        if (timer.id != gdut::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            GDUT_DISABLE_TIMER_UPDATES;
            active_list.remove(timer.id, false);
            remove_callback.call_if(timer.id);
            GDUT_ENABLE_TIMER_UPDATES;
          }

          // Reset in-place.
          new (&timer) timer_data();
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

      for (int i = 0; i < MAX_TIMERS; ++i)
      {
        ::new (&timer_array[i]) timer_data();
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
              timer_data& timer = active_list.front();

              count -= timer.delta;

              active_list.remove(timer.id, true);
              remove_callback.call_if(timer.id);

              if (timer.repeating)
              {
                // Reinsert the timer.
                timer.delta = timer.period;
                active_list.insert(timer.id);
                insert_callback.call_if(timer.id);
              }

              if (timer.p_callback != GDUT_NULLPTR)
              {
                if (timer.cbk_type == timer_data::C_CALLBACK)
                {
                  // Call the C callback.
                  reinterpret_cast<void(*)()>(timer.p_callback)();
                }
                else if(timer.cbk_type == timer_data::IFUNCTION)
                {
                  // Call the function wrapper callback.
                  (*reinterpret_cast<gdut::ifunction<void>*>(timer.p_callback))();
                }
                else if(timer.cbk_type == timer_data::DELEGATE)
                {
                    // Call the delegate callback.
                    (*reinterpret_cast<callback_type*>(timer.p_callback))();
                }
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
        timer_data& timer = timer_array[id_];

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
        timer_data& timer = timer_array[id_];

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
      return !active_list.empty();
    }

    //*******************************************
    /// Get the time to the next timer event.
    /// Returns gdut::timer::interval::No_Active_Interval if there is no active timer.
    //*******************************************
    uint32_t time_to_next() const
    {
      uint32_t delta = static_cast<uint32_t>(gdut::timer::interval::No_Active_Interval);

      if (has_active_timer())
      {
        delta = active_list.front().delta;
      }

      return delta;
    }

    //*******************************************
    /// Checks if a timer is currently active.
    /// Returns <b>true</b> if the timer is active, otherwise <b>false</b>.
    //*******************************************
    bool is_active(gdut::timer::id::type id_) const
    {
      // Valid timer id?
      if (is_valid_timer_id(id_))
      {
        if (has_active_timer())
        {
          const timer_data& timer = timer_array[id_];

          // Registered timer?
          if (timer.id != gdut::timer::id::NO_TIMER)
          {
            return timer.is_active();
          }
        }
      }

      return false;
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

    //*************************************************************************
    /// The configuration of a timer.
    struct timer_data
    {
      typedef gdut::delegate<void(void)> callback_type;

      enum callback_type_id
      {
        C_CALLBACK,
        IFUNCTION,
        DELEGATE
      };

      //*******************************************
      timer_data()
        : p_callback(GDUT_NULLPTR)
        , period(0)
        , delta(gdut::timer::state::Inactive)
        , id(gdut::timer::id::NO_TIMER)
        , previous(gdut::timer::id::NO_TIMER)
        , next(gdut::timer::id::NO_TIMER)
        , repeating(true)
        , cbk_type(IFUNCTION)
      {
      }

      //*******************************************
      /// C function callback
      //*******************************************
      timer_data(gdut::timer::id::type id_,
                 void                 (*p_callback_)(),
                 uint32_t             period_,
                 bool                 repeating_)
        : p_callback(reinterpret_cast<void*>(p_callback_))
        , period(period_)
        , delta(gdut::timer::state::Inactive)
        , id(id_)
        , previous(gdut::timer::id::NO_TIMER)
        , next(gdut::timer::id::NO_TIMER)
        , repeating(repeating_)
        , cbk_type(C_CALLBACK)
      {
      }

      //*******************************************
      /// ETL function callback
      //*******************************************
      timer_data(gdut::timer::id::type  id_,
                 gdut::ifunction<void>& callback_,
                 uint32_t              period_,
                 bool                  repeating_)
        : p_callback(reinterpret_cast<void*>(&callback_))
        , period(period_)
        , delta(gdut::timer::state::Inactive)
        , id(id_)
        , previous(gdut::timer::id::NO_TIMER)
        , next(gdut::timer::id::NO_TIMER)
        , repeating(repeating_)
        , cbk_type(IFUNCTION)
      {
      }

      //*******************************************
      /// ETL delegate callback
      //*******************************************
      timer_data(gdut::timer::id::type id_,
                 callback_type&       callback_,
                 uint32_t             period_,
                 bool                 repeating_)
        : p_callback(reinterpret_cast<void*>(&callback_)),
        period(period_),
        delta(gdut::timer::state::Inactive),
        id(id_),
        previous(gdut::timer::id::NO_TIMER),
        next(gdut::timer::id::NO_TIMER),
        repeating(repeating_),
        cbk_type(DELEGATE)
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

      void*                 p_callback;
      uint32_t              period;
      uint32_t              delta;
      gdut::timer::id::type  id;
      uint_least8_t         previous;
      uint_least8_t         next;
      bool                  repeating;
      callback_type_id      cbk_type;

    private:

      // Disabled.
      timer_data(const timer_data& other);
      timer_data& operator =(const timer_data& other);
    };

    //*******************************************
    /// Constructor.
    //*******************************************
    icallback_timer(timer_data* const timer_array_, const uint_least8_t  Max_Timers_)
      : timer_array(timer_array_),
        active_list(timer_array_),
        enabled(false),
#if defined(GDUT_CALLBACK_TIMER_USE_ATOMIC_LOCK)
        process_semaphore(0),
#endif
        registered_timers(0),
        MAX_TIMERS(Max_Timers_)
    {
    }

  private:

    //*******************************************
    /// Check that the timer id is valid.
    //*******************************************
    bool is_valid_timer_id(gdut::timer::id::type id_) const
    {
      return (id_ < MAX_TIMERS);
    }

    //*************************************************************************
    class timer_list
    {
    public:

      //*******************************
      timer_list(timer_data* ptimers_)
        : head(gdut::timer::id::NO_TIMER)
        , tail(gdut::timer::id::NO_TIMER)
        , current(gdut::timer::id::NO_TIMER)
        , ptimers(ptimers_)
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
        timer_data& timer = ptimers[id_];

        if (head == gdut::timer::id::NO_TIMER)
        {
          // No entries yet.
          head = id_;
          tail = id_;
          timer.previous = gdut::timer::id::NO_TIMER;
          timer.next = gdut::timer::id::NO_TIMER;
        }
        else
        {
          // We already have entries.
          gdut::timer::id::type test_id = begin();

          while (test_id != gdut::timer::id::NO_TIMER)
          {
            timer_data& test = ptimers[test_id];

            // Find the correct place to insert.
            if (timer.delta <= test.delta)
            {
              if (test.id == head)
              {
                head = timer.id;
              }

              // Insert before test.
              timer.previous = test.previous;
              test.previous = timer.id;
              timer.next = test.id;

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
            timer.previous = tail;
            timer.next = gdut::timer::id::NO_TIMER;
            tail = timer.id;
          }
        }
      }

      //*******************************
      void remove(gdut::timer::id::type id_, bool has_expired)
      {
        timer_data& timer = ptimers[id_];

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
        timer.next = gdut::timer::id::NO_TIMER;
        timer.delta = gdut::timer::state::Inactive;
      }

      //*******************************
      timer_data& front()
      {
        return ptimers[head];
      }

      //*******************************
      const timer_data& front() const
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
          timer_data& timer = ptimers[id];
          id = next(id);
          timer.next = gdut::timer::id::NO_TIMER;
        }

        head = gdut::timer::id::NO_TIMER;
        tail = gdut::timer::id::NO_TIMER;
        current = gdut::timer::id::NO_TIMER;
      }

    private:

      gdut::timer::id::type head;
      gdut::timer::id::type tail;
      gdut::timer::id::type current;

      timer_data* const ptimers;
    };

    // The array of timer data structures.
    timer_data* const timer_array;

    // The list of active timers.
    timer_list active_list;

    volatile bool enabled;
#if defined(GDUT_CALLBACK_TIMER_USE_ATOMIC_LOCK)

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

    const uint_least8_t MAX_TIMERS;
  };

  //***************************************************************************
  /// The callback timer
  //***************************************************************************
  template <const uint_least8_t Max_Timers_>
  class callback_timer : public gdut::icallback_timer
  {
  public:

    GDUT_STATIC_ASSERT(Max_Timers_ <= 254, "No more than 254 timers are allowed");

    //*******************************************
    /// Constructor.
    //*******************************************
    callback_timer()
      : icallback_timer(timer_array, Max_Timers_)
    {
    }

  private:

    timer_data timer_array[Max_Timers_];
  };
}

#undef GDUT_DISABLE_TIMER_UPDATES
#undef GDUT_ENABLE_TIMER_UPDATES
#undef GDUT_TIMER_UPDATES_ENABLED

#endif
