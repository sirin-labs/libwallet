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

#ifndef __TCPCHANNEL_H_
#define __TCPCHANNEL_H_

#include "../wallet.h"
#include <string>

class tcpChannel : public wallet::channel {
public:
    explicit tcpChannel(const std::string &host, unsigned short port);
    ~tcpChannel();

    int openChannel(void);
    void closeChannel(void);

    bool isChannelOpen(void) const;

    int writeChannel(const std::vector<unsigned char> &buf);
    int readChannel(std::vector<unsigned char> &buf, size_t length);

private:
    unsigned short m_port;
    std::string m_host;
    int m_sockfd;
};

#endif // __TCPCHANNEL_H_
