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

#ifndef __TREZORDECIVE_H_
#define __TREZORDECIVE_H_

#include "wallet.h"
#include <google/protobuf/message.h>

class trezorDecive : public wallet::walletDev {

public:
    explicit trezorDecive();
    ~trezorDecive();

    int openDevice(wallet::channel *chan);
    void closeDevice(void);

    bool isDeviceConnected(void);

    int signTx(std::string &retSignedTx, const std::string &coinName, const std::vector<wallet::txInput> &inputs,
               const std::vector<wallet::txOutput> &outputs, uint32_t version, uint32_t lockTime, bool usePassphrase);
    int signEthTx(std::string &retSignedTx, const std::string &toAddress, uint64_t value, uint64_t nonce,
                  uint64_t gasPrice, uint64_t gasLimit, const std::string &derivationPath, bool usePassphrase,
                  const std::string &data, wallet::ethChainId chainId);
    int getAddress(std::string &retAddress, const std::string &coinName, const std::string &derivationPath,
                   wallet::inputScriptType scriptType, bool display, bool usePassphrase);
    int getAddresses(std::list<std::string> &retAddresses, unsigned int addressesNum, const std::string &coinName,
                     const std::string &derivationPath, wallet::inputScriptType scriptType, bool display,
                     bool usePassphrase);
    int getEthAddress(std::string &retAddress, const std::string &derivationPath, bool display, bool usePassphrase);
    int getEthAddresses(std::list<std::string> &retAddresses, unsigned int addressesNum,
                        const std::string &derivationPath, bool display, bool usePassphrase);

    int wipe(void);
    int changePin(void);
    int recovery(const std::string &label, bool pinProtection, bool passphraseProtection, int wordCount);
    int setup(const std::string &label, bool pinProtection, bool passphraseProtection, int wordCount);
    int getXPub(std::string &xpub);
    int ping(void);

    int cancel(void);

    int getWalletInfo(wallet::walletInfo &winfo);

private:
    std::vector<wallet::txInput> m_inputs;
    std::vector<wallet::txOutput> m_outputs;
    std::vector<uint8_t> m_ethData;
    uint32_t m_ethDataPos;

    int sendMsg(uint16_t msgType, const ::google::protobuf::Message &pbMsg);
    int sendMsg(uint16_t msgType);

    int userActionAck(int userAckType, const std::string &data = "");

    int processCommand(void *retValue);
    int processMsg(uint16_t msgType, std::vector<unsigned char> data, void *retValue);
    int processTxRequestMsg(std::vector<unsigned char> data, void *retValue);
};

#endif // __TREZORDECIVE_H_
