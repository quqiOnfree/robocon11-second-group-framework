///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2019 John Wellbelove

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

#ifndef GDUT_DETERMINE_DEVELOPMENT_OS_INCLUDED
#define GDUT_DETERMINE_DEVELOPMENT_OS_INCLUDED

#if !defined(GDUT_DEVELOPMENT_OS_WINDOWS) && \
    !defined(GDUT_DEVELOPMENT_OS_LINUX) && \
    !defined(GDUT_DEVELOPMENT_OS_UNIX) && \
    !defined(GDUT_DEVELOPMENT_OS_APPLE) && \
    !defined(GDUT_DEVELOPMENT_OS_BSD) && \
    !defined(GDUT_DEVELOPMENT_OS_GENERIC)

  #if !defined(GDUT_DEVELOPMENT_OS_DETECTED) && !defined(GDUT_DEVELOPMENT_OS_WINDOWS)
    #if defined(WIN32) || defined(WIN64)
      #define GDUT_DEVELOPMENT_OS_WINDOWS
      #define GDUT_DEVELOPMENT_OS_DETECTED
    #endif
  #endif

  #if !defined(GDUT_DEVELOPMENT_OS_DETECTED) && !defined(GDUT_DEVELOPMENT_OS_LINUX)
    #if defined(__linux__)
      #define GDUT_DEVELOPMENT_OS_LINUX
      #define GDUT_DEVELOPMENT_OS_DETECTED
    #endif
  #endif

  #if !defined(GDUT_DEVELOPMENT_OS_DETECTED) && !defined(GDUT_DEVELOPMENT_OS_UNIX)
    #if defined(__unix__)
      #define GDUT_DEVELOPMENT_OS_UNIX
      #define GDUT_DEVELOPMENT_OS_DETECTED
    #endif
  #endif

  #if !defined(GDUT_DEVELOPMENT_OS_DETECTED) && !defined(GDUT_DEVELOPMENT_OS_APPLE)
    #if defined(__APPLE__)
      #define GDUT_DEVELOPMENT_OS_APPLE
      #define GDUT_DEVELOPMENT_OS_DETECTED
    #endif
  #endif

#if !defined(GDUT_DEVELOPMENT_OS_DETECTED) && !defined(GDUT_DEVELOPMENT_OS_BSD)
  #if defined(BSD)
    #define GDUT_DEVELOPMENT_OS_BSD
    #define GDUT_DEVELOPMENT_OS_DETECTED
  #endif
#endif

  #if !defined(GDUT_DEVELOPMENT_OS_DETECTED)
    #define GDUT_DEVELOPMENT_OS_GENERIC
    #define GDUT_DEVELOPMENT_OS_DETECTED

  #endif
#endif

#endif
