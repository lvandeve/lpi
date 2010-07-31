/*
Copyright (c) 2005-2010 Lode Vandevenne
All rights reserved.

This file is part of Lode's Programming Interface.

Lode's Programming Interface is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lode's Programming Interface is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lode's Programming Interface.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

/*
This file defines macros for identifying the target operating system. Currently
only Win32 and Linux are supported here.

Detect win32 with "#if defined(LPI_OS_WINDOWS)"
Detect Linux with "#if defined(LPI_LINUX)"

TODO: 64-bit variants, Mac, ...

based on: http://predef.sourceforge.net/preos.html
*/


#if defined(_WIN32) //despite its name, this is also true for 64-bit windows
#define LPI_OS_WINDOWS

#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#define LPI_OS_LINUX

//AmigaOS4 and AROS are defined separately because both have a few differences in libraries
#elif defined(__amigaos4__)
#define LPI_OS_AMIGA

//AmigaOS4 and AROS are defined separately because both have a few differences in libraries
#elif defined(__AROS__)
#define LPI_OS_AROS

#elif defined(__APPLE__) && defined(__MACH__)
#define LPI_OS_MACOSX


#endif
