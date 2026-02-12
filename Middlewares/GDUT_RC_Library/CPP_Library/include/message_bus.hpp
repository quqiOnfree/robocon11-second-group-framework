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

#ifndef GDUT_MESSAGE_BUS_INCLUDED
#define GDUT_MESSAGE_BUS_INCLUDED

#include "platform.hpp"
#include "algorithm.hpp"
#include "vector.hpp"
#include "nullptr.hpp"
#include "error_handler.hpp"
#include "exception.hpp"
#include "message_types.hpp"
#include "message.hpp"
#include "message_router.hpp"

#include <stdint.h>

namespace gdut
{
  //***************************************************************************
  /// Base exception class for message bus
  //***************************************************************************
  class message_bus_exception : public gdut::exception
  {
  public:

    message_bus_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : gdut::exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Too many subscribers.
  //***************************************************************************
  class message_bus_too_many_subscribers : public gdut::message_bus_exception
  {
  public:

    message_bus_too_many_subscribers(string_type file_name_, numeric_type line_number_)
      : message_bus_exception(GDUT_ERROR_TEXT("message bus:too many subscribers", GDUT_MESSAGE_BUS_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Interface for message bus
  //***************************************************************************
  class imessage_bus : public gdut::imessage_router
  {
  private:

    typedef gdut::ivector<gdut::imessage_router*> router_list_t;

  public:

    using gdut::imessage_router::receive;

    //*******************************************
    /// Subscribe to the bus.
    //*******************************************
    bool subscribe(gdut::imessage_router& router)
    {
      bool ok = true;

      // There's no point adding routers that don't consume messages.
      if (router.is_consumer())
      {
        ok = !router_list.full();

        GDUT_ASSERT(ok, GDUT_ERROR(gdut::message_bus_too_many_subscribers));

        if (ok)
        {
          router_list_t::iterator irouter = gdut::upper_bound(router_list.begin(),
                                                             router_list.end(),
                                                             router.get_message_router_id(),
                                                             compare_router_id());

          router_list.insert(irouter, &router);
        }
      }

      return ok;
    }

    //*******************************************
    /// Unsubscribe from the bus.
    //*******************************************
    void unsubscribe(gdut::message_router_id_t id)
    {
      if (id == gdut::imessage_bus::ALL_MESSAGE_ROUTERS)
      {
        clear();
      }
      else
      {
        GDUT_OR_STD::pair<router_list_t::iterator, router_list_t::iterator> range = gdut::equal_range(router_list.begin(),
                                                                                                    router_list.end(),
                                                                                                    id,
                                                                                                    compare_router_id());

        router_list.erase(range.first, range.second);
      }
    }

    //*******************************************
    void unsubscribe(gdut::imessage_router& router)
    {
      router_list_t::iterator irouter = gdut::find(router_list.begin(),
                                                  router_list.end(),
                                                  &router);

      if (irouter != router_list.end())
      {
        router_list.erase(irouter);
      }
    }

    //*******************************************
    virtual void receive(const gdut::imessage& message) GDUT_OVERRIDE
    {
      receive(gdut::imessage_router::ALL_MESSAGE_ROUTERS, message);
    }

    //*******************************************
    virtual void receive(gdut::shared_message   shared_msg) GDUT_OVERRIDE
    {
      receive(gdut::imessage_router::ALL_MESSAGE_ROUTERS, shared_msg);
    }

    //*******************************************
    virtual void receive(gdut::message_router_id_t destination_router_id,
                         const gdut::imessage&     message) GDUT_OVERRIDE
    {
      switch (destination_router_id)
      {
        //*****************************
        // Broadcast to all routers.
        case gdut::imessage_router::ALL_MESSAGE_ROUTERS:
        {
          router_list_t::iterator irouter = router_list.begin();

          // Broadcast to everyone.
          while (irouter != router_list.end())
          {
            gdut::imessage_router& router = **irouter;

            if (router.accepts(message.get_message_id()))
            {
              router.receive(message);
            }

            ++irouter;
          }

          break;
        }

        //*****************************
        // Must be an addressed message.
        default:
        {
          router_list_t::iterator irouter = router_list.begin();

          // Find routers with the id.
          GDUT_OR_STD::pair<router_list_t::iterator, router_list_t::iterator> range = gdut::equal_range(router_list.begin(),
                                                                                                      router_list.end(),
                                                                                                      destination_router_id,
                                                                                                      compare_router_id());

          // Call all of them.
          while (range.first != range.second)
          {
            if ((*(range.first))->accepts(message.get_message_id()))
            {
              (*(range.first))->receive(message);
            }

            ++range.first;
          }

          // Do any message buses.
          // These are always at the end of the list.
          irouter = gdut::lower_bound(router_list.begin(),
                                     router_list.end(),
                                     gdut::imessage_bus::MESSAGE_BUS,
                                     compare_router_id());

          while (irouter != router_list.end())
          {
            // So pass it on.
            (*irouter)->receive(destination_router_id, message);

            ++irouter;
          }

          break;
        }
      }

      if (has_successor())
      {
        if (get_successor().accepts(message.get_message_id()))
        {
          get_successor().receive(destination_router_id, message);
        }
      }
    }

    //********************************************
    virtual void receive(gdut::message_router_id_t destination_router_id,
                         gdut::shared_message      shared_msg) GDUT_OVERRIDE
    {
      switch (destination_router_id)
      {
        //*****************************
        // Broadcast to all routers.
      case gdut::imessage_router::ALL_MESSAGE_ROUTERS:
      {
        router_list_t::iterator irouter = router_list.begin();

        // Broadcast to everyone.
        while (irouter != router_list.end())
        {
          gdut::imessage_router& router = **irouter;

          if (router.accepts(shared_msg.get_message().get_message_id()))
          {
            router.receive(shared_msg);
          }

          ++irouter;
        }

        break;
      }

      //*****************************
      // Must be an addressed message.
      default:
      {
        // Find routers with the id.
        GDUT_OR_STD::pair<router_list_t::iterator, router_list_t::iterator> range = gdut::equal_range(router_list.begin(),
                                                                                                    router_list.end(),
                                                                                                    destination_router_id,
                                                                                                    compare_router_id());

        // Call all of them.
        while (range.first != range.second)
        {
          if ((*(range.first))->accepts(shared_msg.get_message().get_message_id()))
          {
            (*(range.first))->receive(shared_msg);
          }

          ++range.first;
        }

        // Do any message buses.
        // These are always at the end of the list.
        router_list_t::iterator irouter = gdut::lower_bound(router_list.begin(),
                                                           router_list.end(),
                                                           gdut::imessage_bus::MESSAGE_BUS,
                                                           compare_router_id());

        while (irouter != router_list.end())
        {
          // So pass it on.
          (*irouter)->receive(destination_router_id, shared_msg);

          ++irouter;
        }

        break;
      }
      }

      if (has_successor())
      {
        if (get_successor().accepts(shared_msg.get_message().get_message_id()))
        {
          get_successor().receive(destination_router_id, shared_msg);
        }
      }
    }

    using imessage_router::accepts;

    //*******************************************
    /// Does this message bus accept the message id?
    /// Returns <b>true</b> on the first router that does.
    //*******************************************
    bool accepts(gdut::message_id_t id) const GDUT_OVERRIDE
    {
      // Check the list of subscribed routers.
      router_list_t::iterator irouter = router_list.begin();

      while (irouter != router_list.end())
      {
        gdut::imessage_router& router = **irouter;

        if (router.accepts(id))
        {
          return true;
        }

        ++irouter;
      }

      // Check any successor.
      if (has_successor())
      {
        if (get_successor().accepts(id))
        {
          return true;
        }
      }

      return false;
    }

    //*******************************************
    size_t size() const
    {
      return router_list.size();
    }

    //*******************************************
    void clear()
    {
      router_list.clear();
    }

    //********************************************
    GDUT_DEPRECATED bool is_null_router() const GDUT_OVERRIDE
    {
      return false;
    }

    //********************************************
    bool is_producer() const GDUT_OVERRIDE
    {
      return true;
    }

    //********************************************
    bool is_consumer() const GDUT_OVERRIDE
    {
      return true;
    }

  protected:

    //*******************************************
    /// Constructor.
    //*******************************************
    imessage_bus(router_list_t& list)
      : imessage_router(gdut::imessage_router::MESSAGE_BUS),
        router_list(list)
    {
    }

    //*******************************************
    /// Constructor.
    //*******************************************
    imessage_bus(router_list_t& router_list_, gdut::imessage_router& successor_)
      : imessage_router(gdut::imessage_router::MESSAGE_BUS, successor_),
      router_list(router_list_)
    {
    }

  private:

    //*******************************************
    // How to compare routers to router ids.
    //*******************************************
    struct compare_router_id
    {
      bool operator()(const gdut::imessage_router* prouter, gdut::message_router_id_t id) const
      {
        return prouter->get_message_router_id() < id;
      }

      bool operator()(gdut::message_router_id_t id, const gdut::imessage_router* prouter) const
      {
        return id < prouter->get_message_router_id();
      }
    };

    router_list_t& router_list;
  };

  //***************************************************************************
  /// The message bus
  //***************************************************************************
  template <uint_least8_t MAX_ROUTERS_>
  class message_bus : public gdut::imessage_bus
  {
  public:

    //*******************************************
    /// Constructor.
    //*******************************************
    message_bus()
      : imessage_bus(router_list)
    {
    }

    //*******************************************
    /// Constructor.
    //*******************************************
    message_bus(gdut::imessage_router& successor_)
      : imessage_bus(router_list, successor_)
    {
    }

  private:

    gdut::vector<gdut::imessage_router*, MAX_ROUTERS_> router_list;
  };
}

#endif
