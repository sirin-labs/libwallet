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

#include <string.h>
#ifdef WIN32
#include <WinSock2.h>
#define bzero(b, len) (memset((b), '\0', (len)), (void)0)
#define bcopy(b1, b2, len) (memmove((b2), (b1), (len)), (void)0)
#define close(s) closesocket(s)
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "common/utils.h"
#include "tcpchannel.h"

tcpChannel::tcpChannel(const std::string &host, unsigned short port) {
    m_host = host;
    m_port = port;
    m_sockfd = (-1);

#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);
    WSAStartup(wVersionRequested, &wsaData);
#endif
}

tcpChannel::~tcpChannel() { closeChannel(); }

int tcpChannel::openChannel(void) {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (isChannelOpen()) {
        WALLETERR("cannot reopen TCP channel\n");
        return -1;
    }

    WALLETINFO("opening TCP channel, HOST %s, PORT %d\n", m_host.c_str(), m_port);

    if ((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        WALLETERR("failed to creat TCP socket\n");
        m_sockfd = (-1);
        return (-1);
    }

    if ((server = gethostbyname(m_host.c_str())) == NULL) {
        WALLETERR("failed to connect TCP socket, no such host\n");
        close(m_sockfd);
        m_sockfd = (-1);
        return (-1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(m_port);

    if (connect(m_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        WALLETERR("failed to connect to TCP server\n");
        close(m_sockfd);
        m_sockfd = (-1);
        return (-1);
    }

    return 0;
}

void tcpChannel::closeChannel(void) {
    if (isChannelOpen()) {
        WALLETINFO("closing TCP channel\n");
        close(m_sockfd);
        m_sockfd = (-1);
    }
}

bool tcpChannel::isChannelOpen(void) const { return (m_sockfd != (-1)); }

int tcpChannel::writeChannel(const std::vector<unsigned char> &buf) {
    if (!isChannelOpen()) return -1;

    if (buf.size() == 0) return 0;

    int n = send(m_sockfd, (const char *)buf.data(), (int)buf.size(), 0);

    if (n != (int)buf.size()) {
        WALLETERR("TCP write error: %d\n", errno);
        return (-1);
    }

    return n;
}

int tcpChannel::readChannel(std::vector<unsigned char> &buf, size_t length) {
    if (!isChannelOpen()) return -1;

    buf.clear();

    if (length == 0) return 0;

    buf.resize(length);

    bzero(buf.data(), length);

    int n = recv(m_sockfd, (char *)buf.data(), (int)length, 0);

    if (n != (int)length) {
        WALLETERR("TCP read error: %d\n", errno);
        return (-1);
    }

    buf.resize(n);

    return n;
}
