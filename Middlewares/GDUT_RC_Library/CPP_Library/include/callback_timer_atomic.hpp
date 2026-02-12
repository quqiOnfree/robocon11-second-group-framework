/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2021 John Wellbelove

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

#ifndef GDUT_CALLBACK_TIMER_ATOMIC_INCLUDED
#define GDUT_CALLBACK_TIMER_ATOMIC_INCLUDED

#include "algorithm.hpp"
#include "delegate.hpp"
#include "error_handler.hpp"
#include "function.hpp"
#include "nullptr.hpp"
#include "placement_new.hpp"
#include "platform.hpp"
#include "static_assert.hpp"
#include "timer.hpp"

#include <stdint.h>

namespace gdut {
//***************************************************************************
/// Interface for callback timer
//***************************************************************************
template <typename TSemaphore> class icallback_timer_atomic {
public:
  typedef gdut::delegate<void(void)> callback_type;

  typedef gdut::delegate<void(gdut::timer::id::type)> event_callback_type;

  //*******************************************
  /// Register a timer.
  //*******************************************
  gdut::timer::id::type register_timer(callback_type callback_,
                                       uint32_t period_, bool repeating_) {
    gdut::timer::id::type id = gdut::timer::id::NO_TIMER;

    bool is_space = (number_of_registered_timers < Max_Timers);

    if (is_space) {
      // Search for the free space.
      for (uint_least8_t i = 0U; i < Max_Timers; ++i) {
        timer_data &timer = timer_array[i];

        if (timer.id == gdut::timer::id::NO_TIMER) {
          // Create in-place.
          new (&timer) timer_data(i, callback_, period_, repeating_);
          ++number_of_registered_timers;
          id = i;
          break;
        }
      }
    }

    return id;
  }

  //*******************************************
  /// Unregister a timer.
  //*******************************************
  bool unregister_timer(gdut::timer::id::type id_) {
    bool result = false;

    if (id_ != gdut::timer::id::NO_TIMER) {
      timer_data &timer = timer_array[id_];

      if (timer.id != gdut::timer::id::NO_TIMER) {
        if (timer.is_active()) {
          ++process_semaphore;
          active_list.remove(timer.id, false);
          remove_callback.call_if(timer.id);
          --process_semaphore;
        }

        // Reset in-place.
        new (&timer) timer_data();
        --number_of_registered_timers;

        result = true;
      }
    }

    return result;
  }

  //*******************************************
  /// Enable/disable the timer.
  //*******************************************
  void enable(bool state_) { enabled = state_; }

  //*******************************************
  /// Get the enable/disable state.
  //*******************************************
  bool is_running() const { return enabled; }

  //*******************************************
  /// Clears the timer of data.
  //*******************************************
  void clear() {
    ++process_semaphore;
    active_list.clear();
    --process_semaphore;

    for (uint8_t i = 0U; i < Max_Timers; ++i) {
      ::new (&timer_array[i]) timer_data();
    }

    number_of_registered_timers = 0;
  }

  //*******************************************
  // Called by the timer service to indicate the
  // amount of time that has elapsed since the last successful call to 'tick'.
  // Returns true if the tick was processed,
  // false if not.
  //*******************************************
  bool tick(uint32_t count) {
    if (enabled) {
      if (process_semaphore == 0U) {
        // We have something to do?
        bool has_active = !active_list.empty();

        if (has_active) {
          while (has_active && (count >= active_list.front().delta)) {
            timer_data &timer = active_list.front();

            count -= timer.delta;

            active_list.remove(timer.id, true);
            remove_callback.call_if(timer.id);

            if (timer.callback.is_valid()) {
              // Call the delegate callback.
              timer.callback();
            }

            if (timer.repeating) {
              // Reinsert the timer.
              timer.delta = timer.period;
              active_list.insert(timer.id);
              insert_callback.call_if(timer.id);
            }

            has_active = !active_list.empty();
          }

          if (has_active) {
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
  bool start(gdut::timer::id::type id_, bool immediate_ = false) {
    bool result = false;

    // Valid timer id?
    if (id_ != gdut::timer::id::NO_TIMER) {
      timer_data &timer = timer_array[id_];

      // Registered timer?
      if (timer.id != gdut::timer::id::NO_TIMER) {
        // Has a valid period.
        if (timer.period != gdut::timer::state::Inactive) {
          ++process_semaphore;
          if (timer.is_active()) {
            active_list.remove(timer.id, false);
            remove_callback.call_if(timer.id);
          }

          timer.delta = immediate_ ? 0U : timer.period;
          active_list.insert(timer.id);
          insert_callback.call_if(timer.id);
          --process_semaphore;

          result = true;
        }
      }
    }

    return result;
  }

  //*******************************************
  /// Stops a timer.
  //*******************************************
  bool stop(gdut::timer::id::type id_) {
    bool result = false;

    // Valid timer id?
    if (id_ != gdut::timer::id::NO_TIMER) {
      timer_data &timer = timer_array[id_];

      // Registered timer?
      if (timer.id != gdut::timer::id::NO_TIMER) {
        if (timer.is_active()) {
          ++process_semaphore;
          active_list.remove(timer.id, false);
          remove_callback.call_if(timer.id);
          --process_semaphore;
        }

        result = true;
      }
    }

    return result;
  }

  //*******************************************
  /// Sets a timer's period.
  //*******************************************
  bool set_period(gdut::timer::id::type id_, uint32_t period_) {
    if (stop(id_)) {
      timer_array[id_].period = period_;
      return true;
    }

    return false;
  }

  //*******************************************
  /// Sets a timer's mode.
  //*******************************************
  bool set_mode(gdut::timer::id::type id_, bool repeating_) {
    if (stop(id_)) {
      timer_array[id_].repeating = repeating_;
      return true;
    }

    return false;
  }

  //*******************************************
  /// Check if there is an active timer.
  //*******************************************
  bool has_active_timer() const {
    ++process_semaphore;
    bool result = !active_list.empty();
    --process_semaphore;

    return result;
  }

  //*******************************************
  /// Get the time to the next timer event.
  /// Returns gdut::timer::interval::No_Active_Interval if there is no active
  /// timer.
  //*******************************************
  uint32_t time_to_next() const {
    uint32_t delta =
        static_cast<uint32_t>(gdut::timer::interval::No_Active_Interval);

    ++process_semaphore;
    if (!active_list.empty()) {
      delta = active_list.front().delta;
    }
    --process_semaphore;

    return delta;
  }

  //*******************************************
  /// Checks if a timer is currently active.
  /// Returns <b>true</b> if the timer is active, otherwise <b>false</b>.
  //*******************************************
  bool is_active(gdut::timer::id::type id_) const {
    bool result = false;

    // Valid timer id?
    if (is_valid_timer_id(id_)) {
      ++process_semaphore;
      if (has_active_timer()) {
        const timer_data &timer = timer_array[id_];

        // Registered timer?
        if (timer.id != gdut::timer::id::NO_TIMER) {
          result = timer.is_active();
        }
      }
      --process_semaphore;
    }

    return result;
  }

  //*******************************************
  /// Set a callback when a timer is inserted on list
  //*******************************************
  void set_insert_callback(event_callback_type insert_) {
    insert_callback = insert_;
  }

  //*******************************************
  /// Set a callback when a timer is removed from list
  //*******************************************
  void set_remove_callback(event_callback_type remove_) {
    remove_callback = remove_;
  }

  //*******************************************
  void clear_insert_callback() { insert_callback.clear(); }

  //*******************************************
  void clear_remove_callback() { remove_callback.clear(); }

protected:
  //*************************************************************************
  /// The configuration of a timer.
  struct timer_data {
    //*******************************************
    timer_data()
        : callback(), period(0U), delta(gdut::timer::state::Inactive),
          id(gdut::timer::id::NO_TIMER), previous(gdut::timer::id::NO_TIMER),
          next(gdut::timer::id::NO_TIMER), repeating(true) {}

    //*******************************************
    /// ETL delegate callback
    //*******************************************
    timer_data(gdut::timer::id::type id_, callback_type callback_,
               uint32_t period_, bool repeating_)
        : callback(callback_), period(period_),
          delta(gdut::timer::state::Inactive), id(id_),
          previous(gdut::timer::id::NO_TIMER), next(gdut::timer::id::NO_TIMER),
          repeating(repeating_) {}

    //*******************************************
    /// Returns true if the timer is active.
    //*******************************************
    bool is_active() const { return delta != gdut::timer::state::Inactive; }

    //*******************************************
    /// Sets the timer to the inactive state.
    //*******************************************
    void set_inactive() { delta = gdut::timer::state::Inactive; }

    callback_type callback;
    uint32_t period;
    uint32_t delta;
    gdut::timer::id::type id;
    uint_least8_t previous;
    uint_least8_t next;
    bool repeating;

  private:
    // Disabled.
    timer_data(const timer_data &other) GDUT_DELETE;
    timer_data &operator=(const timer_data &other) GDUT_DELETE;
  };

  //*******************************************
  /// Constructor.
  //*******************************************
  icallback_timer_atomic(timer_data *const timer_array_,
                         const uint_least8_t Max_Timers_)
      : timer_array(timer_array_), active_list(timer_array_), enabled(false),
        process_semaphore(0U), number_of_registered_timers(0U),
        Max_Timers(Max_Timers_) {}

private:
  //*******************************************
  /// Check that the timer id is valid.
  //*******************************************
  bool is_valid_timer_id(gdut::timer::id::type id_) const {
    return (id_ < Max_Timers);
  }

  //*************************************************************************
  /// A specialised intrusive linked list for timer data.
  //*************************************************************************
  class timer_list {
  public:
    //*******************************
    timer_list(timer_data *ptimers_)
        : head(gdut::timer::id::NO_TIMER), tail(gdut::timer::id::NO_TIMER),
          current(gdut::timer::id::NO_TIMER), ptimers(ptimers_) {}

    //*******************************
    bool empty() const { return head == gdut::timer::id::NO_TIMER; }

    //*******************************
    // Inserts the timer at the correct delta position
    //*******************************
    void insert(gdut::timer::id::type id_) {
      timer_data &timer = ptimers[id_];

      if (head == gdut::timer::id::NO_TIMER) {
        // No entries yet.
        head = id_;
        tail = id_;
        timer.previous = gdut::timer::id::NO_TIMER;
        timer.next = gdut::timer::id::NO_TIMER;
      } else {
        // We already have entries.
        gdut::timer::id::type test_id = begin();

        while (test_id != gdut::timer::id::NO_TIMER) {
          timer_data &test = ptimers[test_id];

          // Find the correct place to insert.
          if (timer.delta <= test.delta) {
            if (test.id == head) {
              head = timer.id;
            }

            // Insert before test.
            timer.previous = test.previous;
            test.previous = timer.id;
            timer.next = test.id;

            // Adjust the next delta to compensate.
            test.delta -= timer.delta;

            if (timer.previous != gdut::timer::id::NO_TIMER) {
              ptimers[timer.previous].next = timer.id;
            }
            break;
          } else {
            timer.delta -= test.delta;
          }

          test_id = next(test_id);
        }

        // Reached the end?
        if (test_id == gdut::timer::id::NO_TIMER) {
          // Tag on to the tail.
          ptimers[tail].next = timer.id;
          timer.previous = tail;
          timer.next = gdut::timer::id::NO_TIMER;
          tail = timer.id;
        }
      }
    }

    //*******************************
    void remove(gdut::timer::id::type id_, bool has_expired) {
      timer_data &timer = ptimers[id_];

      if (head == id_) {
        head = timer.next;
      } else {
        ptimers[timer.previous].next = timer.next;
      }

      if (tail == id_) {
        tail = timer.previous;
      } else {
        ptimers[timer.next].previous = timer.previous;
      }

      if (!has_expired) {
        // Adjust the next delta.
        if (timer.next != gdut::timer::id::NO_TIMER) {
          ptimers[timer.next].delta += timer.delta;
        }
      }

      timer.previous = gdut::timer::id::NO_TIMER;
      timer.next = gdut::timer::id::NO_TIMER;
      timer.delta = gdut::timer::state::Inactive;
    }

    //*******************************
    timer_data &front() { return ptimers[head]; }

    //*******************************
    const timer_data &front() const { return ptimers[head]; }

    //*******************************
    gdut::timer::id::type begin() {
      current = head;
      return current;
    }

    //*******************************
    gdut::timer::id::type previous(gdut::timer::id::type last) {
      current = ptimers[last].previous;
      return current;
    }

    //*******************************
    gdut::timer::id::type next(gdut::timer::id::type last) {
      current = ptimers[last].next;
      return current;
    }

    //*******************************
    void clear() {
      gdut::timer::id::type id = begin();

      while (id != gdut::timer::id::NO_TIMER) {
        timer_data &timer = ptimers[id];
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

    timer_data *const ptimers;
  };

  // The array of timer data structures.
  timer_data *const timer_array;

  // The list of active timers.
  timer_list active_list;

  bool enabled;
  mutable TSemaphore process_semaphore;
  uint_least8_t number_of_registered_timers;

  event_callback_type insert_callback;
  event_callback_type remove_callback;

public:
  const uint_least8_t Max_Timers;
};

//***************************************************************************
/// The callback timer
//***************************************************************************
template <uint_least8_t Max_Timers_, typename TSemaphore>
class callback_timer_atomic : public gdut::icallback_timer_atomic<TSemaphore> {
public:
  GDUT_STATIC_ASSERT(Max_Timers_ <= 254U,
                     "No more than 254 timers are allowed");

  //*******************************************
  /// Constructor.
  //*******************************************
  callback_timer_atomic()
      : icallback_timer_atomic<TSemaphore>(timer_array, Max_Timers_) {}

private:
  typename gdut::icallback_timer_atomic<TSemaphore>::timer_data
      timer_array[Max_Timers_];
};
} // namespace gdut

#endif
