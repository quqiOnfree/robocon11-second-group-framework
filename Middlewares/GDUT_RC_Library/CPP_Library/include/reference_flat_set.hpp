///\file

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

#ifndef GDUT_REFERENCE_FLAT_SET_INCLUDED
#define GDUT_REFERENCE_FLAT_SET_INCLUDED

#include "platform.hpp"
#include "algorithm.hpp"
#include "iterator.hpp"
#include "functional.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "nth_type.hpp"
#include "pool.hpp"
#include "error_handler.hpp"
#include "exception.hpp"
#include "vector.hpp"
#include "iterator.hpp"

#include "private/comparator_is_transparent.hpp"

#include <stddef.h>

namespace gdut
{
  //***************************************************************************
  ///\ingroup reference_flat_set
  /// Exception base for flat_sets
  //***************************************************************************
  class flat_set_exception : public exception
  {
  public:

    flat_set_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  ///\ingroup reference_flat_set
  /// Vector full exception.
  //***************************************************************************
  class flat_set_full : public flat_set_exception
  {
  public:

    flat_set_full(string_type file_name_, numeric_type line_number_)
      : flat_set_exception(GDUT_ERROR_TEXT("flat_set:full", GDUT_REFERENCE_FLAT_SET_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  ///\ingroup reference_flat_set
  /// Vector iterator exception.
  //***************************************************************************
  class flat_set_iterator : public flat_set_exception
  {
  public:

    flat_set_iterator(string_type file_name_, numeric_type line_number_)
      : flat_set_exception(GDUT_ERROR_TEXT("flat_set:iterator", GDUT_REFERENCE_FLAT_SET_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The base class for specifically sized reference_flat_sets.
  /// Can be used as a reference type for all reference_flat_sets containing a specific type.
  ///\ingroup reference_flat_set
  //***************************************************************************
  template <typename T, typename TKeyCompare = gdut::less<T> >
  class ireference_flat_set
  {
  public:

    typedef T                 key_type;
    typedef T                 value_type;
    typedef TKeyCompare       key_compare;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef size_t            size_type;

  protected:

    typedef gdut::ivector<value_type*> lookup_t;

  public:

    //*************************************************************************
    class iterator : public gdut::iterator<GDUT_OR_STD::bidirectional_iterator_tag, value_type>
    {
    public:

      friend class ireference_flat_set;
      friend class const_iterator;

      iterator()
      {
      }

      iterator(typename lookup_t::iterator ilookup_)
        : ilookup(ilookup_)
      {
      }

      iterator(const iterator& other)
        : ilookup(other.ilookup)
      {
      }

      iterator& operator =(const iterator& other)
      {
        ilookup = other.ilookup;
        return *this;
      }

      iterator& operator ++()
      {
        ++ilookup;
        return *this;
      }

      iterator operator ++(int)
      {
        iterator temp(*this);
        ++ilookup;
        return temp;
      }

      iterator& operator --()
      {
        --ilookup;
        return *this;
      }

      iterator operator --(int)
      {
        iterator temp(*this);
        --ilookup;
        return temp;
      }

      reference operator *() const
      {
        return *(*ilookup);
      }

      pointer operator &() const
      {
        return gdut::addressof(*(*ilookup));
      }

      pointer operator ->() const
      {
        return gdut::addressof(*(*ilookup));
      }

      friend bool operator == (const iterator& lhs, const iterator& rhs)
      {
        return lhs.ilookup == rhs.ilookup;
      }

      friend bool operator != (const iterator& lhs, const iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      typename lookup_t::iterator ilookup;
    };

    //*************************************************************************
    class const_iterator : public gdut::iterator<GDUT_OR_STD::bidirectional_iterator_tag, const value_type>
    {
    public:

      friend class ireference_flat_set;

      const_iterator()
      {
      }

      const_iterator(typename lookup_t::const_iterator ilookup_)
        : ilookup(ilookup_)
      {
      }

      const_iterator(const typename ireference_flat_set::iterator& other)
        : ilookup(other.ilookup)
      {
      }

      const_iterator(const const_iterator& other)
        : ilookup(other.ilookup)
      {
      }

      const_iterator& operator =(const iterator& other)
      {
        ilookup = other.ilookup;
        return *this;
      }

      const_iterator& operator =(const const_iterator& other)
      {
        ilookup = other.ilookup;
        return *this;
      }

      const_iterator& operator ++()
      {
        ++ilookup;
        return *this;
      }

      const_iterator operator ++(int)
      {
        const_iterator temp(*this);
        ++ilookup;
        return temp;
      }

      const_iterator& operator --()
      {
        --ilookup;
        return *this;
      }

      const_iterator operator --(int)
      {
        const_iterator temp(*this);
        --ilookup;
        return temp;
      }

      const_reference operator *() const
      {
        return *(*ilookup);
      }

      const_pointer operator &() const
      {
        return gdut::addressof(*(*ilookup));
      }

      const_pointer operator ->() const
      {
        return gdut::addressof(*(*ilookup));
      }

      friend bool operator == (const const_iterator& lhs, const const_iterator& rhs)
      {
        return lhs.ilookup == rhs.ilookup;
      }

      friend bool operator != (const const_iterator& lhs, const const_iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      typename lookup_t::const_iterator ilookup;
    };

  protected:

    typedef typename gdut::parameter_type<T>::type parameter_t;

  public:

    typedef GDUT_OR_STD::reverse_iterator<iterator>       reverse_iterator;
    typedef GDUT_OR_STD::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef typename gdut::iterator_traits<iterator>::difference_type difference_type;

    //*********************************************************************
    /// Returns an iterator to the beginning of the reference_flat_set.
    ///\return An iterator to the beginning of the reference_flat_set.
    //*********************************************************************
    iterator begin()
    {
      return iterator(lookup.begin());
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the reference_flat_set.
    ///\return A const iterator to the beginning of the reference_flat_set.
    //*********************************************************************
    const_iterator begin() const
    {
      return const_iterator(lookup.begin());
    }

    //*********************************************************************
    /// Returns an iterator to the end of the reference_flat_set.
    ///\return An iterator to the end of the reference_flat_set.
    //*********************************************************************
    iterator end()
    {
      return iterator(lookup.end());
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the reference_flat_set.
    ///\return A const iterator to the end of the reference_flat_set.
    //*********************************************************************
    const_iterator end() const
    {
      return const_iterator(lookup.end());
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the reference_flat_set.
    ///\return A const iterator to the beginning of the reference_flat_set.
    //*********************************************************************
    const_iterator cbegin() const
    {
      return const_iterator(lookup.cbegin());
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the reference_flat_set.
    ///\return A const iterator to the end of the reference_flat_set.
    //*********************************************************************
    const_iterator cend() const
    {
      return const_iterator(lookup.cend());
    }

    //*********************************************************************
    /// Returns an reverse iterator to the reverse beginning of the reference_flat_set.
    ///\return Iterator to the reverse beginning of the reference_flat_set.
    //*********************************************************************
    reverse_iterator rbegin()
    {
      return reverse_iterator(lookup.rbegin());
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the reference_flat_set.
    ///\return Const iterator to the reverse beginning of the reference_flat_set.
    //*********************************************************************
    const_reverse_iterator rbegin() const
    {
      return const_reverse_iterator(lookup.rbegin());
    }

    //*********************************************************************
    /// Returns a reverse iterator to the end + 1 of the reference_flat_set.
    ///\return Reverse iterator to the end + 1 of the reference_flat_set.
    //*********************************************************************
    reverse_iterator rend()
    {
      return reverse_iterator(lookup.rend());
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the end + 1 of the reference_flat_set.
    ///\return Const reverse iterator to the end + 1 of the reference_flat_set.
    //*********************************************************************
    const_reverse_iterator rend() const
    {
      return const_reverse_iterator(lookup.rend());
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the reference_flat_set.
    ///\return Const reverse iterator to the reverse beginning of the reference_flat_set.
    //*********************************************************************
    const_reverse_iterator crbegin() const
    {
      return const_reverse_iterator(lookup.crbegin());
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the end + 1 of the reference_flat_set.
    ///\return Const reverse iterator to the end + 1 of the reference_flat_set.
    //*********************************************************************
    const_reverse_iterator crend() const
    {
      return const_reverse_iterator(lookup.crend());
    }

    //*********************************************************************
    /// Assigns values to the reference_flat_set.
    /// If asserts or exceptions are enabled, emits reference_flat_set_full if the reference_flat_set does not have enough free space.
    /// If asserts or exceptions are enabled, emits reference_flat_set_iterator if the iterators are reversed.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*********************************************************************
    template <typename TIterator>
    void assign(TIterator first, TIterator last)
    {
#if GDUT_IS_DEBUG_BUILD
      difference_type d = gdut::distance(first, last);
      GDUT_ASSERT(d <= difference_type(capacity()), GDUT_ERROR(flat_set_full));
#endif

      clear();

      while (first != last)
      {
        insert(*first);
        ++first;
      }
    }

    //*********************************************************************
    /// Inserts a value to the reference_flat_set.
    /// If asserts or exceptions are enabled, emits reference_flat_set_full if the reference_flat_set is already full.
    ///\param value    The value to insert.
    //*********************************************************************
    GDUT_OR_STD::pair<iterator, bool> insert(reference value)
    {
      iterator i_element = lower_bound(value);

      return insert_at(i_element, value);
    }

    //*********************************************************************
    /// Inserts a value to the reference_flat_set.
    /// If asserts or exceptions are enabled, emits reference_flat_set_full if the reference_flat_set is already full.
    ///\param position The position to insert at.
    ///\param value    The value to insert.
    //*********************************************************************
    iterator insert(const_iterator /*position*/, reference value)
    {
      return insert(value).first;
    }

    //*********************************************************************
    /// Inserts a range of values to the reference_flat_set.
    /// If asserts or exceptions are enabled, emits reference_flat_set_full if the reference_flat_set does not have enough free space.
    ///\param position The position to insert at.
    ///\param first    The first element to add.
    ///\param last     The last + 1 element to add.
    //*********************************************************************
    template <class TIterator>
    void insert(TIterator first, TIterator last)
    {
      while (first != last)
      {
        insert(*first);
        ++first;
      }
    }

    //*********************************************************************
    /// Erases an element.
    ///\param key The key to erase.
    ///\return The number of elements erased. 0 or 1.
    //*********************************************************************
    size_t erase(parameter_t key)
    {
      iterator i_element = find(key);

      if (i_element == end())
      {
        return 0;
      }
      else
      {
        lookup.erase(i_element.ilookup);
        return 1;
      }
    }

    //*********************************************************************
#if GDUT_USING_CPP11
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    size_t erase(K&& key)
    {
      iterator i_element = find(gdut::forward<K>(key));

      if (i_element == end())
      {
        return 0;
      }
      else
      {
        lookup.erase(i_element.ilookup);
        return 1;
      }
    }
#endif

    //*********************************************************************
    /// Erases an element.
    ///\param i_element Iterator to the element.
    //*********************************************************************
    iterator erase(iterator i_element)
    {
      return lookup.erase(i_element.ilookup);
    }

    //*********************************************************************
    /// Erases an element.
    ///\param i_element Iterator to the element.
    //*********************************************************************
    iterator erase(const_iterator i_element)
    {
      return lookup.erase(i_element.ilookup);
    }

    //*********************************************************************
    /// Erases a range of elements.
    /// The range includes all the elements between first and last, including the
    /// element pointed by first, but not the one pointed by last.
    ///\param first Iterator to the first element.
    ///\param last  Iterator to the last element.
    //*********************************************************************
    iterator erase(const_iterator first, const_iterator last)
    {
      return lookup.erase(first.ilookup, last.ilookup);
    }

    //*************************************************************************
    /// Clears the reference_flat_set.
    //*************************************************************************
    void clear()
    {
      lookup.clear();
    }

    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator pointing to the element or end() if not found.
    //*********************************************************************
    iterator find(parameter_t key)
    {
      iterator itr = gdut::lower_bound(begin(), end(), key, compare);

      if (itr != end())
      {
        if (!key_compare()(*itr, key) && !key_compare()(key, *itr))
        {
          return itr;
        }
        else
        {
          return end();
        }
      }

      return end();
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    iterator find(const K& key)
    {
      iterator itr = gdut::lower_bound(begin(), end(), key, compare);

      if (itr != end())
      {
        if (!key_compare()(*itr, key) && !key_compare()(key, *itr))
        {
          return itr;
        }
        else
        {
          return end();
        }
      }

      return end();
    }
#endif

    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator pointing to the element or end() if not found.
    //*********************************************************************
    const_iterator find(parameter_t key) const
    {
      const_iterator itr = gdut::lower_bound(begin(), end(), key, compare);

      if (itr != end())
      {
        if (!key_compare()(*itr, key) && !key_compare()(key, *itr))
        {
          return itr;
        }
        else
        {
          return end();
        }
      }

      return end();
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    const_iterator find(const K& key) const
    {
      const_iterator itr = gdut::lower_bound(begin(), end(), key, compare);

      if (itr != end())
      {
        if (!key_compare()(*itr, key) && !key_compare()(key, *itr))
        {
          return itr;
        }
        else
        {
          return end();
        }
      }

      return end();
    }
#endif

    //*********************************************************************
    /// Counts an element.
    ///\param key The key to search for.
    ///\return 1 if the key exists, otherwise 0.
    //*********************************************************************
    size_t count(parameter_t key) const
    {
      return (find(key) == end()) ? 0 : 1;
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    size_t count(const K& key) const
    {
      return (find(key) == end()) ? 0 : 1;
    }
#endif

    //*********************************************************************
    /// Finds the lower bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    iterator lower_bound(parameter_t key)
    {
      return gdut::lower_bound(begin(), end(), key, compare);
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    iterator lower_bound(const K& key)
    {
      return gdut::lower_bound(begin(), end(), key, compare);
    }
#endif

    //*********************************************************************
    /// Finds the lower bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    const_iterator lower_bound(parameter_t key) const
    {
      return gdut::lower_bound(cbegin(), cend(), key, compare);
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    const_iterator lower_bound(const K& key) const
    {
      return gdut::lower_bound(cbegin(), cend(), key, compare);
    }
#endif

    //*********************************************************************
    /// Finds the upper bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    iterator upper_bound(parameter_t key)
    {
      return gdut::upper_bound(begin(), end(), key, compare);
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    iterator upper_bound(const K& key)
    {
      return gdut::upper_bound(begin(), end(), key, compare);
    }
#endif

    //*********************************************************************
    /// Finds the upper bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    const_iterator upper_bound(parameter_t key) const
    {
      return gdut::upper_bound(cbegin(), cend(), key, compare);
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    const_iterator upper_bound(const K& key) const
    {
      return gdut::upper_bound(cbegin(), cend(), key, compare);
    }
#endif

    //*********************************************************************
    /// Finds the range of equal elements of a key
    ///\param key The key to search for.
    ///\return An iterator pair.
    //*********************************************************************
    GDUT_OR_STD::pair<iterator, iterator> equal_range(parameter_t key)
    {
      return gdut::equal_range(begin(), end(), key, compare);
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    GDUT_OR_STD::pair<iterator, iterator> equal_range(const K& key)
    {
      return gdut::equal_range(begin(), end(), key, compare);
    }
#endif

    //*********************************************************************
    /// Finds the range of equal elements of a key
    ///\param key The key to search for.
    ///\return An iterator pair.
    //*********************************************************************
    GDUT_OR_STD::pair<const_iterator, const_iterator> equal_range(parameter_t key) const
    {
      return gdut::upper_bound(cbegin(), cend(), key, compare);
    }

#if GDUT_USING_CPP11
    //*********************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    GDUT_OR_STD::pair<const_iterator, const_iterator> equal_range(const K& key) const
    {
      return gdut::upper_bound(cbegin(), cend(), key, compare);
    }
#endif

    //*************************************************************************
    /// Check if the set contains the key.
    //*************************************************************************
    bool contains(parameter_t key) const
    {
      return find(key) != end();
    }

#if GDUT_USING_CPP11
    //*************************************************************************
    template <typename K, typename KC = TKeyCompare, gdut::enable_if_t<comparator_is_transparent<KC>::value, int> = 0>
    bool contains(const K& k) const
    {
      return find(k) != end();
    }
#endif

    //*************************************************************************
    /// Gets the current size of the reference_flat_set.
    ///\return The current size of the reference_flat_set.
    //*************************************************************************
    size_type size() const
    {
      return lookup.size();
    }

    //*************************************************************************
    /// Checks the 'empty' state of the reference_flat_set.
    ///\return <b>true</b> if empty.
    //*************************************************************************
    bool empty() const
    {
      return lookup.empty();
    }

    //*************************************************************************
    /// Checks the 'full' state of the reference_flat_set.
    ///\return <b>true</b> if full.
    //*************************************************************************
    bool full() const
    {
      return lookup.full();
    }

    //*************************************************************************
    /// Returns the capacity of the reference_flat_set.
    ///\return The capacity of the reference_flat_set.
    //*************************************************************************
    size_type capacity() const
    {
      return lookup.capacity();
    }

    //*************************************************************************
    /// Returns the maximum possible size of the reference_flat_set.
    ///\return The maximum size of the reference_flat_set.
    //*************************************************************************
    size_type max_size() const
    {
      return lookup.max_size();
    }

    //*************************************************************************
    /// Returns the remaining capacity.
    ///\return The remaining capacity.
    //*************************************************************************
    size_t available() const
    {
      return lookup.available();
    }

  protected:

    //*********************************************************************
    /// Constructor.
    //*********************************************************************
    ireference_flat_set(lookup_t& lookup_)
      : lookup(lookup_)
    {
    }

    //*********************************************************************
    /// Inserts a value to the reference_flat_set.
    ///\param i_element The place to insert.
    ///\param value     The value to insert.
    //*********************************************************************
    GDUT_OR_STD::pair<iterator, bool> insert_at(iterator i_element, reference value)
    {
      GDUT_OR_STD::pair<iterator, bool> result(end(), false);

      if (i_element == end())
      {
        // At the end.
        GDUT_ASSERT(!lookup.full(), GDUT_ERROR(flat_set_full));

        lookup.push_back(&value);
        result.first = --end();
        result.second = true;
      }
      else
      {
        // Not at the end.
        result.first = i_element;

        // Existing element?
        if (compare(value, *i_element) || compare(*i_element, value))
        {
          // A new one.
          GDUT_ASSERT(!lookup.full(), GDUT_ERROR(flat_set_full));
          lookup.insert(i_element.ilookup, &value);
          result.second = true;
        }
      }

      return result;
    }

  private:

    // Disable copy construction.
    ireference_flat_set(const ireference_flat_set&);
    ireference_flat_set& operator =(const ireference_flat_set&);

    lookup_t& lookup;

    TKeyCompare compare;

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
#if defined(GDUT_POLYMORPHIC_REFERENCE_FLAT_SET) || defined(GDUT_POLYMORPHIC_CONTAINERS)
  public:
    virtual ~ireference_flat_set()
    {
    }
#else
  protected:
    ~ireference_flat_set()
    {
    }
#endif
  };

  //***************************************************************************
  /// An reference flat set
  ///\ingroup reference_flat_set
  //***************************************************************************
  template <typename TKey, const size_t MAX_SIZE_, typename TKeyCompare = gdut::less<TKey> >
  class reference_flat_set : public ireference_flat_set<TKey, TKeyCompare>
  {
  public:

    static GDUT_CONSTANT size_t MAX_SIZE = MAX_SIZE_;

    using typename ireference_flat_set<TKey, TKeyCompare>::value_type;

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    reference_flat_set()
      : ireference_flat_set<TKey, TKeyCompare>(lookup)
    {
    }

    //*************************************************************************
    /// Copy constructor.
    //*************************************************************************
    reference_flat_set(const reference_flat_set& other)
      : ireference_flat_set<TKey, TKeyCompare>(lookup)
    {
      ireference_flat_set<TKey, TKeyCompare>::assign(other.cbegin(), other.cend());
    }

    //*************************************************************************
    /// Constructor, from an iterator range.
    ///\tparam TIterator The iterator type.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*************************************************************************
    template <typename TIterator>
    reference_flat_set(TIterator first, TIterator last)
      : ireference_flat_set<TKey, TKeyCompare>(lookup)
    {
      ireference_flat_set<TKey, TKeyCompare>::assign(first, last);
    }

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~reference_flat_set()
    {
      ireference_flat_set<TKey, TKeyCompare>::clear();
    }

  private:

    // The vector that stores pointers to the nodes.
    gdut::vector<value_type*, MAX_SIZE> lookup;
  };

  template <typename TKey, const size_t MAX_SIZE_, typename TCompare>
  GDUT_CONSTANT size_t reference_flat_set<TKey, MAX_SIZE_, TCompare>::MAX_SIZE;

  //*************************************************************************
  /// Template deduction guides.
  //*************************************************************************
#if GDUT_USING_CPP17 && GDUT_HAS_INITIALIZER_LIST
  template <typename... T>
  reference_flat_set(T...) -> reference_flat_set<gdut::nth_type_t<0, T...>, sizeof...(T)>;
#endif

  //*************************************************************************
  /// Make
  //*************************************************************************
#if GDUT_USING_CPP11 && GDUT_HAS_INITIALIZER_LIST
  template <typename TKey, typename TKeyCompare = gdut::less<TKey>, typename... T>
  constexpr auto make_reference_flat_set(T&&... keys) -> gdut::reference_flat_set<TKey, sizeof...(T), TKeyCompare>
  {
    return { gdut::forward<T>(keys)... };
  }
#endif

  //***************************************************************************
  /// Equal operator.
  ///\param lhs Reference to the first reference_flat_set.
  ///\param rhs Reference to the second reference_flat_set.
  ///\return <b>true</b> if the arrays are equal, otherwise <b>false</b>
  ///\ingroup reference_flat_set
  //***************************************************************************
  template <typename T, typename TKeyCompare>
  bool operator ==(const gdut::ireference_flat_set<T, TKeyCompare>& lhs, const gdut::ireference_flat_set<T, TKeyCompare>& rhs)
  {
    return (lhs.size() == rhs.size()) && gdut::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  //***************************************************************************
  /// Not equal operator.
  ///\param lhs Reference to the first reference_flat_set.
  ///\param rhs Reference to the second reference_flat_set.
  ///\return <b>true</b> if the arrays are not equal, otherwise <b>false</b>
  ///\ingroup reference_flat_set
  //***************************************************************************
  template <typename T, typename TKeyCompare>
  bool operator !=(const gdut::ireference_flat_set<T, TKeyCompare>& lhs, const gdut::ireference_flat_set<T, TKeyCompare>& rhs)
  {
    return !(lhs == rhs);
  }
}

#endif
