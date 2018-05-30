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

#ifndef __HIDCHANNEL_H_
#define __HIDCHANNEL_H_

#include "../wallet.h"
#include "hidapi/hidapi.h"
#include <vector>

class hidChannel : public wallet::channel {
public:
    explicit hidChannel(unsigned short vendorId, unsigned short productId);
    ~hidChannel();

    int openChannel(void);
    void closeChannel(void);

    bool isChannelOpen(void) const;

    int writeChannel(const std::vector<unsigned char> &buf);
    int readChannel(std::vector<unsigned char> &buf, size_t length);

private:
    hid_device *m_hidDevice;
    std::vector<unsigned char> m_readBuf;

    unsigned short m_vendorId;
    unsigned short m_productId;
};

#endif // __HIDCHANNEL_H_
