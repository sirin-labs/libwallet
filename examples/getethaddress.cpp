/*
 * getaddress: Get Ethereum address from wallet test program
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

static int getEthAddresses(wallet::walletDev *wallet, const std::string &derivationPath, int count, bool display,
                           bool usePassphrase) {
    std::list<std::string> addresses;

    printf("Get address ...\n");

    if (wallet->getEthAddresses(addresses, count, derivationPath, display, usePassphrase) != 0) {
        printf("Get address failed: %s\n", wallet->getResultString().c_str());
        return -1;
    }

    std::list<std::string>::iterator it = addresses.begin();

    while (it != addresses.end()) {
        printf("ADDRESS: %s\n", it->c_str());
        it++;
    }

    return 0;
}

int main(int argc, char **argv) {
    wallet::walletDev *wallet = NULL;
    wallet::channel *chan = NULL;
    const char *dev_type = NULL;
    const char *derivation_path_str = "";
    unsigned short debug_level = 0;
    int count = 1;
    bool show_help = false;
    bool display = false;
    bool use_passphrase = false;

    int j;
    size_t arglen;
    if (argc >= 2) {
        for (j = 1; j < argc; j++) {
            arglen = strlen(argv[j]);
            if (((argv[j][0] == '-') || (argv[j][0] == '/')) && (arglen >= 2)) {
                switch (argv[j][1]) {
                case 'i':
                    display = true;
                    break;
                case 'a':
                    use_passphrase = true;
                    break;
                case 'd':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -d requires a debug level\n");
                        return 1;
                    }
                    debug_level = atoi(argv[++j]);
                    break;
                case 'o':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -o requires a number\n");
                        return 1;
                    }
                    count = atoi(argv[++j]);
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
                case 'p':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -p requires a derivation path\n");
                        return 1;
                    }
                    derivation_path_str = argv[++j];
                    break;
                }
            }
        }
    }

    if (show_help || (dev_type == NULL)) {
        printf("usage: %s -t dev_type [-n channel_type] [-d debug_level] [-p "
               "derivation_path] [-o count] [-i] "
               "[-s] [-h]\n",
               argv[0]);
        printf("   -t dev_type        : device type "
               "(SIRIN|DUMMY|KEEPKEY|TREZOR)\n");
        printf("   -n channel_type    : channel type "
               "(TCP:<host>:<port>|HID:<vid>:<pid>)\n");
        printf("   -p derivation path : derivation path\n");
        printf("   -o count           : count, default=1\n");
        printf("   -d debug_level     : debug level, default=0\n");
        printf("   -a                 : use passphrase, default=false\n");
        printf("   -i                 : display address on screen\n");
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

    int rc = getEthAddresses(wallet, std::string(derivation_path_str), count, display, use_passphrase);

    wallet->closeDevice();

    delete wallet;
    delete chan;

    return rc;
}
