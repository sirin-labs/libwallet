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

#include "hidchannel.h"
#include "common/utils.h"
#include <string.h>

hidChannel::hidChannel(unsigned short vendorId, unsigned short productId) {
    m_vendorId = vendorId;
    m_productId = productId;
    m_hidDevice = NULL;
}

hidChannel::~hidChannel() { closeChannel(); }

int hidChannel::openChannel(void) {
    if (isChannelOpen()) {
        WALLETERR("cannot reopen HID channel\n");
        return (-1);
    }

    WALLETINFO("opening HID channel, VID 0x%x, PID 0x%x\n", m_vendorId, m_productId);

    if ((m_hidDevice = hid_open(m_vendorId, m_productId, NULL)) == NULL) {
        WALLETERR("failed to open HID device\n");
        return (-1);
    }

    const unsigned char uart[] = {0x41, 0x01};
    const unsigned char txrx[] = {0x43, 0x03};

    hid_send_feature_report(m_hidDevice, uart, 2); // enable UART
    hid_send_feature_report(m_hidDevice, txrx, 2); // purge TX/RX FIFOs

    return 0;
}

void hidChannel::closeChannel(void) {
    if (isChannelOpen()) {
        WALLETINFO("closing HID channel\n");
        hid_close(m_hidDevice);
        m_hidDevice = NULL;
    }
}

bool hidChannel::isChannelOpen(void) const { return (m_hidDevice != NULL); }

int hidChannel::writeChannel(const std::vector<unsigned char> &buf) {
    if (!isChannelOpen()) return (-1);

    if (buf.size() == 0) return 0;

    for (size_t i = 0; i < buf.size(); i += 63) {
        uint8_t chunk[1 + 63] = {0};

        size_t ncopy = (std::min)(sizeof(chunk) - 1, buf.size() - i);

        chunk[0] = sizeof(chunk) - 1;
        memcpy(&chunk[1], buf.data() + i, ncopy);

        int res = hid_write(m_hidDevice, chunk, sizeof(chunk));
        if (res < (int)sizeof(chunk)) {
            const wchar_t *err = hid_error(m_hidDevice);
            WALLETERR("HID write error: %S\n", err);
            return (-1);
        }
    }

    return (int)buf.size();
}

int hidChannel::readChannel(std::vector<unsigned char> &buf, size_t length) {
    if (!isChannelOpen()) return -1;

    buf.clear();

    if (length == 0) return 0;

    while (m_readBuf.size() < length) {
        uint8_t chunk[1 + 63] = {0};

        const int res = hid_read(m_hidDevice, chunk, sizeof(chunk));
        if (res > 0) {
            int chlen = (std::min)(res - 1, int(chunk[0]));
            m_readBuf.insert(m_readBuf.end(), (const char *)(chunk + 1), (const char *)(chunk + 1 + chlen));
        } else if (res == 0) { // check timeout
            break;
        } else if (res < 0) { // read returns -1 on error
            const wchar_t *err = hid_error(m_hidDevice);
            WALLETERR("HID read error: %S\n", err);
            return -1;
        }
    }
    length = (std::min)(length, m_readBuf.size());

    std::copy(m_readBuf.begin(), m_readBuf.begin() + length, std::back_inserter(buf));
    m_readBuf.erase(m_readBuf.begin(), m_readBuf.begin() + length);

    return (int)buf.size();
}
