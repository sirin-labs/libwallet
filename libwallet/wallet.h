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

#ifndef __WALLET_H_
#define __WALLET_H_

#include <list>
#include <string>
#include <vector>

#ifdef WALLET_LIBRARY
#include "wallet_global.h"
#else
#define WALLETSHARED_EXPORT
#endif

namespace wallet {

typedef enum outputScriptType_t {
    OutputScriptType_None = (-1),
    OutputScriptType_PayToAddress = 0,
    OutputScriptType_PayToScriptHash = 1,
    OutputScriptType_PayToMultisig = 2,
    OutputScriptType_PayToOpreturn = 3,
    OutputScriptType_PayToWitness = 4,
    OutputScriptType_PayToP2Shwitness = 5,
} outputScriptType;

typedef enum inputScriptType_t {
    InputScriptType_None = (-1),
    InputScriptType_SpendAddress = 0,
    InputScriptType_SpendMultisig = 1,
    InputScriptType_External = 2,
    InputScriptType_SpendWitness = 3,
    InputScriptType_SpendP2SHWitness = 4,
} inputScriptType;

typedef struct txOutput_t {
    std::string address;
    std::string derivationPath;
    uint64_t amount;
    outputScriptType scriptType;
} txOutput;

typedef struct txOutputBin_t {
    std::string scriptPubkey;
    uint64_t amount;
} txOutputBin;

typedef struct txInput_t {
    std::string prevHash;
    std::string address;
    std::string derivationPath;
    std::string scriptSignature;
    uint64_t amount;
    uint32_t prevIndex;
    inputScriptType scriptType;
} txInput;

typedef struct txMetadata_t {
    uint32_t version;
    uint32_t lockTime;
    uint32_t inputsCount;
    uint32_t outputsCount;
    uint32_t extraDataLen;
} txMetadata;

typedef enum userActionReqType_t {
    UserActionReqType_Button = 1,
    UserActionReqType_Word = 2,
    UserActionReqType_CurrentPin = 3,
    UserActionReqType_NewFirstPin = 4,
    UserActionReqType_NewSecondPin = 5,
    UserActionReqType_Passphrase = 6,
    UserActionReqType_Entropy = 7
} userActionReqType;

typedef enum txDetailsRequestType_t {
    TxDetailsReqType_Input = 1,
    TxDetailsReqType_BinOutput = 2,
    TxDetailsReqType_Metadata = 3,
} txDetailsRequestType;

typedef struct coin_t {
    std::string name;
    std::string shortcut;
} coin;

typedef struct walletInfo_t {
    bool initialized;
    std::string vendorName;
    std::string deviceId;
    std::string firmwareRev;
    std::string xpub;
    std::list<coin> supportedCoins;
} walletInfo;

typedef enum ethChainId_t {
    ethChainId_Unknown = 0,
    ethChainId_Mainnet = 1,
    ethChainId_Classic_Mainnet = 61,
    ethChainId_Classic_Testnet = 62,
    ethChainId_Rinkeby_Testnet = 4,
    ethChainId_Kovan_Testnet = 42,
} ethChainId;

class WALLETSHARED_EXPORT channel {
public:
    virtual ~channel() {}

    virtual int openChannel(void) = 0;
    virtual void closeChannel(void) = 0;

    virtual bool isChannelOpen(void) const = 0;

    virtual int writeChannel(const std::vector<unsigned char> &buf) = 0;
    virtual int readChannel(std::vector<unsigned char> &buf, size_t length) = 0;

    static class channel *getTCPChannel(const std::string &host, unsigned short port);
    static class channel *getHIDChannel(unsigned short vendorId, unsigned short productId);
    static class channel *getChannel(const std::string &channelDesc);
};

class WALLETSHARED_EXPORT walletDev {

public:
    walletDev() {
        m_userActionRequestHandler = NULL;
        m_txDetailsRequestHandler = NULL;
        m_debugLevel = 0;
    }
    virtual ~walletDev() {}

    virtual int openDevice(wallet::channel *chan) = 0;
    virtual void closeDevice(void) = 0;

    virtual bool isDeviceConnected(void) = 0;

    virtual int signTx(std::string &retSignedTx, const std::string &coinName, const std::vector<txInput> &inputs,
                       const std::vector<txOutput> &outputs, uint32_t version = 1, uint32_t lockTime = 0,
                       bool usePassphrase = false) = 0;
    virtual int signEthTx(std::string &retSignedTx, const std::string &toAddress, uint64_t value, uint64_t nonce,
                          uint64_t gasPrice, uint64_t gasLimit, const std::string &derivationPath = "",
                          bool usePassphrase = false, const std::string &data = "",
                          ethChainId chainId = ethChainId_Unknown) = 0;
    virtual int getAddress(std::string &retAddress, const std::string &coinName = "Bitcoin",
                           const std::string &derivationPath = "", inputScriptType scriptType = InputScriptType_None,
                           bool display = false, bool usePassphrase = false) = 0;
    virtual int getAddresses(std::list<std::string> &retAddresses, unsigned int addressesNum = 1,
                             const std::string &coinName = "Bitcoin", const std::string &derivationPath = "",
                             inputScriptType scriptType = InputScriptType_None, bool display = false,
                             bool usePassphrase = false) = 0;
    virtual int wipe(void) = 0;
    virtual int changePin(void) = 0;
    virtual int recovery(const std::string &, bool pinProtection, bool passphraseProtection, int wordCount) = 0;
    virtual int setup(const std::string &label, bool pinProtection, bool passphraseProtection, int wordCount) = 0;
    virtual int getXPub(std::string &xpub) = 0;
    virtual int ping(void) = 0;

    virtual int cancel(void) = 0;

    virtual int getWalletInfo(walletInfo &winfo) = 0;

    void setDebugLevel(unsigned short debugLevel);
    std::string getResultString(void) const;

    int registerUserActionRequestHandler(int (*userActionRequestHandler)(userActionReqType, std::string &));
    int registerTxDetailsRequestHandler(int (*txDetailsRequestHandler)(txDetailsRequestType, const std::string &,
                                                                       uint32_t, void *));

    static class walletDev *getDevice(const std::string &devType);

private:
    int (*m_userActionRequestHandler)(userActionReqType, std::string &);
    int (*m_txDetailsRequestHandler)(txDetailsRequestType, const std::string &, uint32_t reqIndex, void *);

protected:
    int userActionRequest(userActionReqType userActReqType, std::string &retInput);
    int txDetailsRequest(txDetailsRequestType txDetailsRequestType, const std::string &hash, uint32_t reqIndex,
                         void *data);

    channel *m_channel;
    std::string m_ResultString;

    int m_debugLevel;
};
}

#define WALLETVERBOSE(...)                                                                                             \
    if (this->m_debugLevel >= 2) {                                                                                     \
        fprintf(stdout, __VA_ARGS__);                                                                                  \
        fflush(stdout);                                                                                                \
    }                                                                                                                  \
    while (0)

#define WALLETDEBUG(...)                                                                                               \
    if (this->m_debugLevel >= 1) {                                                                                     \
        fprintf(stdout, __VA_ARGS__);                                                                                  \
        fflush(stdout);                                                                                                \
    }                                                                                                                  \
    while (0)

#define WALLETINFO(...)                                                                                                \
    do {                                                                                                               \
        fprintf(stdout, __VA_ARGS__);                                                                                  \
        fflush(stdout);                                                                                                \
    } while (0)

#define WALLETERR(...)                                                                                                 \
    do {                                                                                                               \
        fprintf(stderr, "ERROR: " __VA_ARGS__);                                                                        \
        fflush(stderr);                                                                                                \
    } while (0)

#endif // __WALLET_H_
