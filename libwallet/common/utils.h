/*
 * Copyright © 2018 SirinLabs Uri Yosef <uriy@sirinlabs.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdio.h>
#include <string>
#include <vector>

#ifdef WIN32
#define NOMINMAX
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

extern std::string int2be(uint64_t value);
extern std::string bin2hex(std::string const &s);
extern std::string bin2hex(std::vector<uint8_t> const &s);
extern std::vector<uint8_t> hex2bin(std::string const &s);
extern std::vector<uint32_t> bip32path2arr(std::string const &n);

#endif // __UTILS_H_
