/*
 * signethtx: Sign Ethereum transaction test program
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

static int SignEthTx(wallet::walletDev *wallet, const std::string &toAddress, uint64_t value, uint64_t nonce,
                     uint64_t gasPrice, uint64_t gasLimit, const std::string &derivationPath, bool usePassphrase,
                     std::string data, int chainId) {
    std::string signedTx;
    printf("Sign Ethereum transaction ...\n");

    if (wallet->signEthTx(signedTx, toAddress, value, nonce, gasPrice, gasLimit, derivationPath, usePassphrase, data,
                          (wallet::ethChainId)chainId) != 0) {
        printf("Sign Ethereum transaction failed: %s\n", wallet->getResultString().c_str());
        return 1;
    }

    printf("TX: %s\n", signedTx.c_str());

    return 0;
}

int main(int argc, char **argv) {
    wallet::walletDev *wallet = NULL;
    wallet::channel *chan = NULL;
    const char *dev_type = NULL;
    unsigned short debug_level = 0;
    const char *to_address = "";
    const char *derivation_path = "";
    const char *data = "";
    uint64_t value = 100000000000000000, nonce = 1, gas_price = 2000000000, gas_limit = 21000;
    bool show_help = false;
    bool use_passphrase = false;
    int chainId = 0;

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
                case 'a':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -a requires an address\n");
                        return 1;
                    }
                    to_address = argv[++j];
                    break;
                case 'o':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -o requires a nonce\n");
                        return 1;
                    }
                    nonce = atoll(argv[++j]);
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
                case 'v':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -v requires a value\n");
                        return 1;
                    }
                    value = atoll(argv[++j]);
                    break;
                case 'g':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -g requires a gas price\n");
                        return 1;
                    }
                    gas_price = atoll(argv[++j]);
                    break;
                case 'i':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -i requires a chain id\n");
                        return 1;
                    }
                    chainId = atoi(argv[++j]);
                    break;
                case 'l':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -l requires a gas limit\n");
                        return 1;
                    }
                    gas_limit = atoll(argv[++j]);
                    break;
                case 'p':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -p requires a derivation path\n");
                        return 1;
                    }
                    derivation_path = argv[++j];
                    break;
                case 'x':
                    if ((j + 1 >= argc) || (argv[j + 1][0] == '-') || (argv[j + 1][0] == '/')) {
                        printf("   Option -x requires data string\n");
                        return 1;
                    }
                    data = argv[++j];
                    break;
                case 's':
                    use_passphrase = true;
                    break;
                }
            }
        }
    }

    if (show_help || (dev_type == NULL)) {
        printf("usage: %s -t dev_type -a address [-n channel_type] [-v value] "
               "[-o nonce ] [-g gas_price] [-l gas_limit] [-x data] [-i "
               "chain_id] [-d debug_level] [-s] [-h]\n",
               argv[0]);
        printf("   -t dev_type        : device type "
               "(SIRIN|DUMMY|KEEPKEY|TREZOR)\n");
        printf("   -n channel_type    : channel type "
               "(TCP:<host>:<port>|HID:<vid>:<pid>)\n");
        printf("   -d debug_level     : debug level, default=0\n");
        printf("   -a address         : to ethereum address\n");
        printf("   -v value           : value, default=100000000000000000\n");
        printf("   -o nonce           : nonce, default=1\n");
        printf("   -g gas_price       : gas price, default=600000000\n");
        printf("   -l gas_limit       : gas limit, default=21000\n");
        printf("   -p derivation path : derivation path\n");
        printf("   -x data            : data\n");
        printf("   -i chain_id        : chain id\n");
        printf("   -s                 : use passphrase, default=false\n");
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

    int rc = SignEthTx(wallet, std::string(to_address), value, nonce, gas_price, gas_limit,
                       std::string(derivation_path), use_passphrase, std::string(data), chainId);

    wallet->closeDevice();

    delete wallet;
    delete chan;

    return rc;
}
