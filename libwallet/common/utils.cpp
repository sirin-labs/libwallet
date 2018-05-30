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

#include "utils.h"
#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>
#include <string>
#include <vector>

std::string bin2hex(std::string const &s) {
    std::ostringstream oss;

    for (unsigned char ch : s) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)ch;
    }

    return oss.str();
}

std::string bin2hex(std::vector<uint8_t> const &s) {
    std::ostringstream oss;

    for (unsigned char ch : s) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)ch;
    }

    return oss.str();
}

std::vector<uint8_t> hex2bin(std::string const &s) {
    assert(s.length() % 2 == 0);

    std::vector<uint8_t> sOut;
    sOut.reserve(s.length() / 2);

    std::string extract;
    for (std::string::const_iterator pos = s.begin(); pos < s.end(); pos += 2) {
        extract.assign(pos, pos + 2);
        sOut.push_back(std::stoi(extract, nullptr, 16));
    }
    return sOut;
}

std::string int2be(uint64_t value) {
    std::string sOut;

    while (value > 0) {
        sOut.insert(sOut.begin(), char(value % 256));
        value = value / 256;
    }

    return sOut;
}

#define PRIME_DERIVATION_FLAG 0x80000000

/*  Convert string of bip32 path to list of
    uint32 integers with prime flags
*/
std::vector<uint32_t> bip32path2arr(std::string const &n) {
    std::vector<uint32_t> arr;
    // Convert string of bip32 path to list of uint32 integers with prime flags
    // # 0/-1/1' -> [0, 0x80000001, 0x80000001]

    if (n.empty()) {
        return arr;
    }

    std::istringstream f(n);
    std::string s;
    while (getline(f, s, '/')) {
        if (s == std::string("m")) continue;

        bool prime = false;

        if (s[s.length() - 1] == '\'') {
            s.erase(s.end() - 1);
            prime = true;
        }
        if (s[0] == '-') prime = true;

        uint32_t x = std::abs(std::stoi(s));

        if (prime) x |= PRIME_DERIVATION_FLAG;

        arr.push_back(x);
    }
    return arr;
}
