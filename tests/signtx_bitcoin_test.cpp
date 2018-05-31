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

static int txDetailsRequestHandler(wallet::txDetailsRequestType txDetReqType, const std::string &hash,
                                   uint32_t reqIndex, void *retData) {
    if (retData == NULL) return (-1);

    if (hash == "f7b8917c1f16b903c57c7e97169cd21cf4ec588e556f0e40491e9a1f7bbc6831") {
        if (txDetReqType == wallet::TxDetailsReqType_Input) {
            if (reqIndex == 0) {
                ((wallet::txInput *)retData)->scriptSignature = "1600145d45fe25b095cf22d9345ed4903049cd4c638df4";
                ((wallet::txInput *)retData)->prevHash =
                    "523e2014c436ba7b753db02f7e750fcc05ac3ef6ae5af9e4ff6fabcde1b9ebfd";
                ((wallet::txInput *)retData)->prevIndex = 1;
                ((wallet::txInput *)retData)->scriptType = wallet::InputScriptType_SpendMultisig;
            } else {
                return (-1);
            }
        } else if (txDetReqType == wallet::TxDetailsReqType_BinOutput) {
            if (reqIndex == 0) {
                ((wallet::txOutputBin *)retData)->scriptPubkey = "76a9140913215f8446682e773cf8319052ba0de7a92f7c88ac";
                ((wallet::txOutputBin *)retData)->amount = 55000000;
            } else if (reqIndex == 1) {
                ((wallet::txOutputBin *)retData)->scriptPubkey = "a914a3fde1a11dc7b562832bfaa044cfea41917fe3fd87";
                ((wallet::txOutputBin *)retData)->amount = 69621837041;
            } else {
                return (-1);
            }
        } else if (txDetReqType == wallet::TxDetailsReqType_Metadata) {

            ((wallet::txMetadata *)retData)->inputsCount = 1;
            ((wallet::txMetadata *)retData)->outputsCount = 2;
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
    wallet::txOutput txOut1;
    wallet::txOutput txOut2;
    wallet::txInput txIn;
    std::vector<wallet::txOutput> outputs;
    std::vector<wallet::txInput> inputs;

    txOut1.address = "mtJwQksCqsc1WpFoKvSFMRY27VDQEVLnym";
    txOut1.amount = 1000000;
    txOut1.scriptType = wallet::OutputScriptType_PayToAddress;

    txOut2.address = "mk8wwL3KWFvxiNZK8XD6781kNuJmF4HHtA";
    txOut2.amount = 53990000;
    txOut2.scriptType = wallet::OutputScriptType_PayToAddress;

    txIn.derivationPath = "m/44'/1'/0'/0/0";
    txIn.prevHash = "f7b8917c1f16b903c57c7e97169cd21cf4ec588e556f0e40491e9a1f7bbc6831";
    txIn.prevIndex = 0;

    outputs.insert(outputs.begin(), txOut1);
    outputs.insert(outputs.begin() + 1, txOut2);
    inputs.insert(inputs.begin(), txIn);

    printf("Sign transaction ...\n");

    if (wallet->signTx(signedTx, "Testnet", inputs, outputs, 1, 0, false) != 0) {
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

    // seed('skin clarify festival goddess picture tourist brain marble police wing derive tool churn bridge iron judge
    // session video jump rate neglect club glad blanket')

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
