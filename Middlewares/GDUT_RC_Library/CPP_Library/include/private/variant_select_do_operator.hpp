/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2025 John Wellbelove

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

//***************************************************************************
// This file is included in variant_variadic.h for C++11 and C++14, as they
// do not support template fold expressions.
//***************************************************************************

namespace private_variant
{
  //***************************************************************************
  // Selects a do_operator inplementation that is configured for the number of types.
  //***************************************************************************
  template <size_t NTypes>
  struct select_do_operator;

  //***************************************************************************
  template <>
  struct select_do_operator<1>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<2>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<3>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        case 2: { visitor(gdut::get<2>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<4>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        case 2: { visitor(gdut::get<2>(the_variant)); break; }
        case 3: { visitor(gdut::get<3>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<5>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        case 2: { visitor(gdut::get<2>(the_variant)); break; }
        case 3: { visitor(gdut::get<3>(the_variant)); break; }
        case 4: { visitor(gdut::get<4>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<6>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        case 2: { visitor(gdut::get<2>(the_variant)); break; }
        case 3: { visitor(gdut::get<3>(the_variant)); break; }
        case 4: { visitor(gdut::get<4>(the_variant)); break; }
        case 5: { visitor(gdut::get<5>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<7>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        case 2: { visitor(gdut::get<2>(the_variant)); break; }
        case 3: { visitor(gdut::get<3>(the_variant)); break; }
        case 4: { visitor(gdut::get<4>(the_variant)); break; }
        case 5: { visitor(gdut::get<5>(the_variant)); break; }
        case 6: { visitor(gdut::get<6>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<8>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        case 2: { visitor(gdut::get<2>(the_variant)); break; }
        case 3: { visitor(gdut::get<3>(the_variant)); break; }
        case 4: { visitor(gdut::get<4>(the_variant)); break; }
        case 5: { visitor(gdut::get<5>(the_variant)); break; }
        case 6: { visitor(gdut::get<6>(the_variant)); break; }
        case 7: { visitor(gdut::get<7>(the_variant)); break; }
        default: break;
      }
    }
  };
#if !defined(GDUT_VARIANT_CPP11_MAX_8_TYPES)
  //***************************************************************************
  template <>
  struct select_do_operator<9>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        case 2: { visitor(gdut::get<2>(the_variant)); break; }
        case 3: { visitor(gdut::get<3>(the_variant)); break; }
        case 4: { visitor(gdut::get<4>(the_variant)); break; }
        case 5: { visitor(gdut::get<5>(the_variant)); break; }
        case 6: { visitor(gdut::get<6>(the_variant)); break; }
        case 7: { visitor(gdut::get<7>(the_variant)); break; }
        case 8: { visitor(gdut::get<8>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<10>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0: { visitor(gdut::get<0>(the_variant)); break; }
        case 1: { visitor(gdut::get<1>(the_variant)); break; }
        case 2: { visitor(gdut::get<2>(the_variant)); break; }
        case 3: { visitor(gdut::get<3>(the_variant)); break; }
        case 4: { visitor(gdut::get<4>(the_variant)); break; }
        case 5: { visitor(gdut::get<5>(the_variant)); break; }
        case 6: { visitor(gdut::get<6>(the_variant)); break; }
        case 7: { visitor(gdut::get<7>(the_variant)); break; }
        case 8: { visitor(gdut::get<8>(the_variant)); break; }
        case 9: { visitor(gdut::get<9>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<11>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<12>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<13>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<14>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<15>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<16>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        default: break;
      }
    }
  };
#if !defined(GDUT_VARIANT_CPP11_MAX_16_TYPES)
  //***************************************************************************
  template <>
  struct select_do_operator<17>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<18>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<19>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<20>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<21>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<22>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<23>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<24>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        default: break;
      }
    }
  };
#if !defined(GDUT_VARIANT_CPP11_MAX_24_TYPES)
  //***************************************************************************
  template <>
  struct select_do_operator<25>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        case 24: { visitor(gdut::get<24>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<26>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        case 24: { visitor(gdut::get<24>(the_variant)); break; }
        case 25: { visitor(gdut::get<25>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<27>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        case 24: { visitor(gdut::get<24>(the_variant)); break; }
        case 25: { visitor(gdut::get<25>(the_variant)); break; }
        case 26: { visitor(gdut::get<26>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<28>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        case 24: { visitor(gdut::get<24>(the_variant)); break; }
        case 25: { visitor(gdut::get<25>(the_variant)); break; }
        case 26: { visitor(gdut::get<26>(the_variant)); break; }
        case 27: { visitor(gdut::get<27>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<29>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        case 24: { visitor(gdut::get<24>(the_variant)); break; }
        case 25: { visitor(gdut::get<25>(the_variant)); break; }
        case 26: { visitor(gdut::get<26>(the_variant)); break; }
        case 27: { visitor(gdut::get<27>(the_variant)); break; }
        case 28: { visitor(gdut::get<28>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<30>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        case 24: { visitor(gdut::get<24>(the_variant)); break; }
        case 25: { visitor(gdut::get<25>(the_variant)); break; }
        case 26: { visitor(gdut::get<26>(the_variant)); break; }
        case 27: { visitor(gdut::get<27>(the_variant)); break; }
        case 28: { visitor(gdut::get<28>(the_variant)); break; }
        case 29: { visitor(gdut::get<29>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<31>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        case 24: { visitor(gdut::get<24>(the_variant)); break; }
        case 25: { visitor(gdut::get<25>(the_variant)); break; }
        case 26: { visitor(gdut::get<26>(the_variant)); break; }
        case 27: { visitor(gdut::get<27>(the_variant)); break; }
        case 28: { visitor(gdut::get<28>(the_variant)); break; }
        case 29: { visitor(gdut::get<29>(the_variant)); break; }
        case 30: { visitor(gdut::get<30>(the_variant)); break; }
        default: break;
      }
    }
  };

  //***************************************************************************
  template <>
  struct select_do_operator<32>
  {
    template <typename TVariant, typename TVisitor>
    static void do_operator(TVariant& the_variant, TVisitor& visitor)
    {
      switch (the_variant.index())
      {
        case 0:  { visitor(gdut::get<0>(the_variant)); break; }
        case 1:  { visitor(gdut::get<1>(the_variant)); break; }
        case 2:  { visitor(gdut::get<2>(the_variant)); break; }
        case 3:  { visitor(gdut::get<3>(the_variant)); break; }
        case 4:  { visitor(gdut::get<4>(the_variant)); break; }
        case 5:  { visitor(gdut::get<5>(the_variant)); break; }
        case 6:  { visitor(gdut::get<6>(the_variant)); break; }
        case 7:  { visitor(gdut::get<7>(the_variant)); break; }
        case 8:  { visitor(gdut::get<8>(the_variant)); break; }
        case 9:  { visitor(gdut::get<9>(the_variant)); break; }
        case 10: { visitor(gdut::get<10>(the_variant)); break; }
        case 11: { visitor(gdut::get<11>(the_variant)); break; }
        case 12: { visitor(gdut::get<12>(the_variant)); break; }
        case 13: { visitor(gdut::get<13>(the_variant)); break; }
        case 14: { visitor(gdut::get<14>(the_variant)); break; }
        case 15: { visitor(gdut::get<15>(the_variant)); break; }
        case 16: { visitor(gdut::get<16>(the_variant)); break; }
        case 17: { visitor(gdut::get<17>(the_variant)); break; }
        case 18: { visitor(gdut::get<18>(the_variant)); break; }
        case 19: { visitor(gdut::get<19>(the_variant)); break; }
        case 20: { visitor(gdut::get<20>(the_variant)); break; }
        case 21: { visitor(gdut::get<21>(the_variant)); break; }
        case 22: { visitor(gdut::get<22>(the_variant)); break; }
        case 23: { visitor(gdut::get<23>(the_variant)); break; }
        case 24: { visitor(gdut::get<24>(the_variant)); break; }
        case 25: { visitor(gdut::get<25>(the_variant)); break; }
        case 26: { visitor(gdut::get<26>(the_variant)); break; }
        case 27: { visitor(gdut::get<27>(the_variant)); break; }
        case 28: { visitor(gdut::get<28>(the_variant)); break; }
        case 29: { visitor(gdut::get<29>(the_variant)); break; }
        case 30: { visitor(gdut::get<30>(the_variant)); break; }
        case 31: { visitor(gdut::get<31>(the_variant)); break; }
        default: break;
      }
    }
  };
#endif
#endif
#endif
}
