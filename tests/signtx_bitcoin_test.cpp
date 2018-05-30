/*
 * signtx_bitcoin_test: Test Bitcoid sign transaction
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

static int txDetailsRequestHandler(wallet::txDetaiesRequestType txDetReqType, const std::string &hash,
                                   uint32_t reqIndex, void *retData) {
    if (retData == NULL) return (-1);

    if (hash == "d5f65ee80147b4bcc70b75e4bbf2d7382021b871bd8867ef8fa525ef50864882") {
        if (txDetReqType == wallet::TxDetaiesReqType_Input) {
            if (reqIndex == 0) {
                ((wallet::txInput *)retData)->scriptSignature =
                    "483045022072ba61305fe7cb542d142b8f3299a7b10f9ea61f6ffaab5d"
                    "ca8142601869d53c0221009a8027ed79eb3b9bc13577ac285326932343"
                    "4558528c6b6a7e542be46e7e9a820141047a2d177c0f3626fc68c53610"
                    "b0270fa6156181f46586c679ba6a88b34c6f4874686390b4d92e5769fb"
                    "b89c8050b984f4ec0b257a0e5c4ff8bd3b035a51709503";
                ((wallet::txInput *)retData)->prevHash = "c16a03f1cf8f99f6b5297ab614586cacec784c2d259af245909dedb0e3"
                                                         "9eddcf";
                ((wallet::txInput *)retData)->prevIndex = 1;
                ((wallet::txInput *)retData)->scriptType = wallet::InputScriptType_None;
            } else if (reqIndex == 1) {
                ((wallet::txInput *)retData)->scriptSignature =
                    "48304502200fd63adc8f6cb34359dc6cca9e5458d7ea50376cbd0a7451"
                    "4880735e6d1b8a4c0221008b6ead7fe5fbdab7319d6dfede3a0bc8e2a7"
                    "c5b5a9301636d1de4aa31a3ee9b101410486ad608470d796236b003635"
                    "718dfc07c0cac0cfc3bfc3079e4f491b0426f0676e6643a39198e8e7bd"
                    "affb94f4b49ea21baa107ec2e237368872836073668214";
                ((wallet::txInput *)retData)->prevHash = "1ae39a2f8d59670c8fc61179148a8e61e039d0d9e8ab08610cb69b4a19"
                                                         "453eaf";
                ((wallet::txInput *)retData)->prevIndex = 1;
                ((wallet::txInput *)retData)->scriptType = wallet::InputScriptType_None;
            } else {
                return (-1);
            }
        } else if (txDetReqType == wallet::TxDetaiesReqType_BinOutput) {
            if (reqIndex == 0) {
                ((wallet::txOutputBin *)retData)->scriptPubkey = "76a91424a56db43cf6f2b02e838ea493f95d8d6047423188ac";
                ((wallet::txOutputBin *)retData)->amount = 390000;
            } else {
                return (-1);
            }
        } else if (txDetReqType == wallet::TxDetaiesReqType_Metadata) {
            ((wallet::txMetadata *)retData)->inputsCount = 2;
            ((wallet::txMetadata *)retData)->outputsCount = 1;
            ((wallet::txMetadata *)retData)->version = 1;
            ((wallet::txMetadata *)retData)->lockTime = 0;
            ((wallet::txMetadata *)retData)->extraDataLen = 0;
        } else {
            return (-1);
        }
    } else {
        return (-1);
    }

    return 0;
}

static int SignTx(wallet::walletDev *wallet) {
    std::string signedTx;
    wallet::txOutput txOut;
    wallet::txInput txIn;
    std::vector<wallet::txOutput> outputs;
    std::vector<wallet::txInput> inputs;

    txOut.address = "1MJ2tj2ThBE62zXbBYA5ZaN3fdve5CPAz1";
    txOut.amount = 390000 - 10000;
    txOut.scriptType = wallet::OutputScriptType_PayToAddress;

    txIn.derivationPath = "0";
    txIn.prevHash = "d5f65ee80147b4bcc70b75e4bbf2d7382021b871bd8867ef8fa525ef50864882";
    txIn.prevIndex = 0;

    outputs.insert(outputs.begin(), txOut);
    inputs.insert(inputs.begin(), txIn);

    printf("Sign transaction ...\n");

    if (wallet->signTx(signedTx, "Bitcoin", inputs, outputs, 1, 0, false) != 0) {
        printf("Sign transaction failed: %s\n", wallet->getResultString().c_str());
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
    bool show_help = false;

    // seed('alcohol woman abuse must during monitor noble actual mixed trade
    // anger aisle')

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
    wallet->registerTxDetailsRequestHandler(txDetailsRequestHandler);

    if (wallet->openDevice(chan) != 0) {
        fprintf(stderr, "device not found\n");
        return 1;
    }

    int rc = SignTx(wallet);

    wallet->closeDevice();

    delete wallet;
    delete chan;

    return rc;
}
