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
#include "tcpchannel.h"
#include "wallet.h"
#include <string.h>
#include <string>

class wallet::channel *wallet::channel::getHIDChannel(unsigned short vendorId, unsigned short productId) {
    return new hidChannel(vendorId, productId);
}

class wallet::channel *wallet::channel::getTCPChannel(const std::string &host, unsigned short port) {
    return new tcpChannel(host, port);
}

class wallet::channel *wallet::channel::getChannel(const std::string &channelDesc) {
    const char delimiter[] = ":";
    char *channel_type_str;
    char *arg1_str;
    char *arg2_str;
    wallet::channel *chan = NULL;
    char *str;

    if ((str = (char *)malloc(channelDesc.length() + 1)) == NULL) return NULL;

    memset(str, 0, channelDesc.length() + 1);
    strcpy(str, channelDesc.c_str());

    for (int i = 0; str[i]; i++)
        str[i] = toupper(str[i]);

    channel_type_str = strtok(str, delimiter);
    arg1_str = strtok(NULL, delimiter);
    arg2_str = strtok(NULL, delimiter);

    if (channel_type_str == NULL || arg1_str == NULL || arg2_str == NULL) return NULL;

    if (strcmp(channel_type_str, "HID") == 0)
        chan = new hidChannel(atoi(arg1_str), atoi(arg2_str));
    else if (strcmp(channel_type_str, "TCP") == 0)
        chan = new tcpChannel(arg1_str, atoi(arg2_str));

    free(str);

    return chan;
}
