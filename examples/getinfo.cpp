/*
 * getaddress: Get information from wallet test program
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

#include "common.h"
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wallet.h>

using namespace std;

static int getWalletInfo(wallet::walletDev *wallet) {
    wallet::walletInfo info;

    printf("Get wallet info ...\n");

    if (wallet->getWalletInfo(info) != 0) {
        printf("Get wallet info failed: %s\n", wallet->getResultString().c_str());
        return -1;
    }

    printf("INITIALIZED : %s\n", info.initialized ? "true" : "false");
    printf("DEV ID      : %s\n", info.deviceId.c_str());
    printf("FIRM REV    : %s\n", info.firmwareRev.c_str());
    printf("VENDOR      : %s\n", info.vendorName.c_str());
    printf("XPUB        : %s\n", info.xpub.c_str());

    std::list<wallet::coin>::iterator it = info.supportedCoins.begin();

    printf("SUPPORTED COINS:\n");
    while (it != info.supportedCoins.end()) {
        printf("%s(%s), ", it->name.c_str(), it->shortcut.c_str());
        it++;
    }
    printf("\n");

    return 0;
}

int main(int argc, char **argv) {
    wallet::walletDev *wallet = NULL;
    wallet::channel *chan = NULL;
    const char *dev_type = NULL;
    unsigned short debug_level = 0;

    bool show_help = false;

    int j;
    size_t arglen;
    if (argc >= 2) {
        for (j = 1; j < argc; j++) {
            arglen = strlen(argv[j]);
            if (((argv[j][0] == '-') || (argv[j][0] == '/')) && (arglen >= 2)) {
                switch (argv[j][1]) {
                case 'd':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -d requires a debug level\n");
                        return 1;
                    }
                    debug_level = atoi(argv[++j]);
                    break;
                case 't':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -t requires a device type "
                               "(SIRIN|DUMMY|KEEPKEY|TREZOR)\n");
                        return 1;
                    }
                    dev_type = argv[++j];
                    break;
                case 'n':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -n requires a channel type "
                               "(TCP:<host>:<port>|HID:<vid>:<pid>)\n");
                        return 1;
                    }
                    chan = wallet::channel::getChannel(std::string(argv[++j]));
                    if (chan == NULL) {
                        printf("   Option -n requires a valid channel type "
                               "(TCP:<host>:<port>|HID:<vid>:<pid>)\n");
                        return 1;
                    }
                    break;
                }
            }
        }
    }

    if (show_help || (dev_type == NULL)) {
        printf("usage: %s -t dev_type [-n channel_type] [-d debug_level] [-h]\n", argv[0]);
        printf("   -t dev_type        : device type "
               "(SIRIN|DUMMY|KEEPKEY|TREZOR)\n");
        printf("   -n channel_type    : channel type "
               "(TCP:<host>:<port>|HID:<vid>:<pid>)\n");
        printf("   -d debug_level     : debug level, default=0\n");
        printf("   -h                 : display usage\n");
        return 0;
    }

    if (chan == NULL) {
        if (strcmp(dev_type, "TREZOR") == 0) {
            chan = wallet::channel::getHIDChannel(0x534c, 0x0001);
        } else if (strcmp(dev_type, "KEEPKEY") == 0) {
            chan = wallet::channel::getHIDChannel(0x2b24, 0x0001);
        } else {
            printf("Channel type must be defined for this device "
                   "type(TCP:<host>:<port>|HID:<vid>:<pid>)\n");
            return 1;
        }
    }

    if ((wallet = wallet::walletDev::getDevice(std::string(dev_type))) == NULL) {
        fprintf(stderr, "failed to get device : %s\n", dev_type);
        return 1;
    }

    wallet->setDebugLevel(debug_level);
    wallet->registerUserActionRequestHandler(userActionRequestHandler);

    if (wallet->openDevice(chan) != 0) {
        fprintf(stderr, "device not found\n");
        return 1;
    }

    int rc = getWalletInfo(wallet);

    wallet->closeDevice();

    delete wallet;
    delete chan;

    return rc;
}
