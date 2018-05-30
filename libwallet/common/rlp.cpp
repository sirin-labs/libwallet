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

#include "rlp.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void writeData(std::vector<uint8_t> &result, const uint8_t *buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        result.push_back(buf[i]);
    }
}

static void writeRlpLength(std::vector<uint8_t> &result, uint32_t length, uint8_t firstbyte) {
    uint8_t buf[4];
    if (length == 1 && firstbyte <= 0x7f) {
        /* empty length header */
    } else if (length <= 55) {
        buf[0] = 0x80 + length;
        writeData(result, buf, 1);
    } else if (length <= 0xff) {
        buf[0] = 0xb7 + 1;
        buf[1] = length;
        writeData(result, buf, 2);
    } else if (length <= 0xffff) {
        buf[0] = 0xb7 + 2;
        buf[1] = length >> 8;
        buf[2] = length & 0xff;
        writeData(result, buf, 3);
    } else {
        buf[0] = 0xb7 + 3;
        buf[1] = length >> 16;
        buf[2] = length >> 8;
        buf[3] = length & 0xff;
        writeData(result, buf, 4);
    }
}

static void writeRlpField(std::vector<uint8_t> &result, const uint8_t *buf, size_t size) {
    writeRlpLength(result, (uint32_t)size, buf[0]);
    writeData(result, buf, size);
}

static void writeRlpNumber(std::vector<uint8_t> &result, uint64_t number) {
    if (!number) {
        return writeRlpLength(result, 0, 0);
    }
    uint8_t data[8];
    data[0] = (number >> 56) & 0xff;
    data[1] = (number >> 48) & 0xff;
    data[2] = (number >> 40) & 0xff;
    data[3] = (number >> 32) & 0xff;
    data[4] = (number >> 24) & 0xff;
    data[5] = (number >> 16) & 0xff;
    data[6] = (number >> 8) & 0xff;
    data[7] = (number)&0xff;
    int offset = 0;
    while (!data[offset]) {
        offset++;
    }
    writeRlpField(result, data + offset, 8 - offset);
}

static size_t rlpCalculateLength(size_t length, uint8_t firstbyte) {
    if (length == 1 && firstbyte <= 0x7f) {
        return 1;
    } else if (length <= 55) {
        return 1 + length;
    } else if (length <= 0xff) {
        return 2 + length;
    } else if (length <= 0xffff) {
        return 3 + length;
    } else {
        return 4 + length;
    }
}

static size_t rlpNumberLength(uint64_t number) {
    if (!number) {
        return rlpCalculateLength(0, 0);
    }
    uint8_t data[8];
    data[0] = (number >> 56) & 0xff;
    data[1] = (number >> 48) & 0xff;
    data[2] = (number >> 40) & 0xff;
    data[3] = (number >> 32) & 0xff;
    data[4] = (number >> 24) & 0xff;
    data[5] = (number >> 16) & 0xff;
    data[6] = (number >> 8) & 0xff;
    data[7] = (number)&0xff;
    int offset = 0;
    while (!data[offset]) {
        offset++;
    }
    return rlpCalculateLength(8 - offset, data[offset]);
}

static size_t getTxDataRLPLength(const std::vector<uint8_t> &toAddress, uint64_t value, uint64_t nonce,
                                 uint64_t gasPrice, uint64_t gasLimit, const std::vector<uint8_t> &data, uint8_t v,
                                 const std::vector<uint8_t> &r, const std::vector<uint8_t> &s) {
    size_t rlp_length = 0;

    rlp_length += rlpNumberLength(nonce);
    rlp_length += rlpNumberLength(gasPrice);
    rlp_length += rlpNumberLength(gasLimit);
    rlp_length += rlpCalculateLength(toAddress.size(), toAddress[0]);
    rlp_length += rlpNumberLength(value);
    if (data.size() == 0)
        rlp_length += rlpCalculateLength(0, 0);
    else
        rlp_length += rlpCalculateLength(data.size(), data[0]);

    rlp_length += rlpNumberLength(v);
    rlp_length += rlpCalculateLength(r.size(), r[0]);
    rlp_length += rlpCalculateLength(s.size(), s[0]);

    return rlp_length;
}

static void writeRlpListLength(std::vector<uint8_t> &result, size_t length) {
    uint8_t buf[4];
    if (length <= 55) {
        buf[0] = 0xc0 + (uint8_t)length;
        writeData(result, buf, 1);
    } else if (length <= 0xff) {
        buf[0] = 0xf7 + 1;
        buf[1] = (uint8_t)length;
        writeData(result, buf, 2);
    } else if (length <= 0xffff) {
        buf[0] = 0xf7 + 2;
        buf[1] = (uint8_t)(length >> 8);
        buf[2] = (uint8_t)(length & 0xff);
        writeData(result, buf, 3);
    } else {
        buf[0] = 0xf7 + 3;
        buf[1] = (uint8_t)(length >> 16);
        buf[2] = (uint8_t)(length >> 8);
        buf[3] = (uint8_t)(length & 0xff);
        writeData(result, buf, 4);
    }
}

std::vector<uint8_t> rlp_encode(const std::vector<uint8_t> &toAddress, uint64_t value, uint64_t nonce,
                                uint64_t gasPrice, uint64_t gasLimit, const std::vector<uint8_t> &data, uint8_t v,
                                const std::vector<uint8_t> &r, const std::vector<uint8_t> &s) {
    std::vector<uint8_t> result;

    result.clear();

    writeRlpListLength(result, getTxDataRLPLength(toAddress, value, nonce, gasPrice, gasLimit, data, v, r, s));

    writeRlpNumber(result, nonce);
    writeRlpNumber(result, gasPrice);
    writeRlpNumber(result, gasLimit);
    writeRlpField(result, (uint8_t *)toAddress.data(), toAddress.size());
    writeRlpNumber(result, value);
    if (data.size() == 0)
        writeRlpNumber(result, 0);
    else
        writeRlpField(result, (uint8_t *)data.data(), data.size());
    writeRlpNumber(result, v);
    writeRlpField(result, (uint8_t *)r.data(), r.size());
    writeRlpField(result, (uint8_t *)s.data(), s.size());

    return result;
}
