/*
 * Copyright © 2018 SirinLabs Boris Feinstein <borisf@sirinlabs.com>
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

#ifndef __RLP_H_
#define __RLP_H_

#include <string>
#include <vector>

extern std::vector<uint8_t> rlp_encode(const std::vector<uint8_t> &toAddress, uint64_t value, uint64_t nonce,
                                       uint64_t gasPrice, uint64_t gasLimit, const std::vector<uint8_t> &data,
                                       uint8_t v, const std::vector<uint8_t> &r, const std::vector<uint8_t> &s);
#endif // __RLP_H_
