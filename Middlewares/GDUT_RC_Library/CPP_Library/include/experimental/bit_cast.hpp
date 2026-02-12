#pragma once

#include "platform.hpp"

template <typename TDestination, typename TSource>
typename gdut::enable_if<(sizeof(TDestination) == sizeof(TSource))   && 
                         gdut::is_trivially_copyable<TSource>::value && 
                         gdut::is_trivially_copyable<TDestination>::value, TDestination>::type
  bit_cast(const TSource& source) GDUT_NOEXCEPT
{
  TDestination destination;

  memcpy(&destination, &source, sizeof(TDestination));

  return destination;
}

template <typename TDestination, typename TSource>
GDUT_CONSTEXPR
typename gdut::enable_if<(sizeof(TDestination) == sizeof(TSource))   &&
                         gdut::is_trivially_copyable<TSource>::value &&
                         gdut::is_trivially_copyable<TDestination>::value, TDestination>::type
  bit_cast(const TSource& source) GDUT_NOEXCEPT
{
  TDestination destination;

  __builtin_memcpy(&destination, &source, sizeof(TDestination));

  return destination;
}



