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

#include "trezordecive.h"
#include "common/rlp.h"
#include "common/utils.h"
#include "protob/messages.pb.h"
#include "protob/types.pb.h"

#define TREZOR_HEADER_SIZE 8

struct signEthRetValues {
    uint32_t lastV;
    std::string lastR;
    std::string lastS;
};

static int buildHeader(std::vector<unsigned char> &header, uint16_t msgType, uint32_t length);
static int parseHeader(const std::vector<unsigned char> &header, uint16_t *type, uint32_t *length);

trezorDecive::trezorDecive() : walletDev() { m_channel = NULL; }

trezorDecive::~trezorDecive() { closeDevice(); }

int trezorDecive::openDevice(wallet::channel *chan) {
    if (m_channel != NULL || chan == NULL) return -1;

    m_channel = chan;

    WALLETINFO("connecting to TREZOR device\n");

    if (m_channel->openChannel() != 0) {
        WALLETERR("failed to connect to TREZOR device\n");
        return -1;
    }

    WALLETINFO("TREZOR device connected\n");

    return 0;
}

void trezorDecive::closeDevice(void) {
    if (isDeviceConnected()) {
        WALLETINFO("disconnecting from TREZOR device\n");

        m_channel->closeChannel();
    }

    m_channel = NULL;
}

bool trezorDecive::isDeviceConnected(void) { return (m_channel != NULL && m_channel->isChannelOpen()); }

int trezorDecive::signTx(std::string &retSignedTx, const std::string &coinName,
                         const std::vector<wallet::txInput> &inputs, const std::vector<wallet::txOutput> &outputs,
                         uint32_t version, uint32_t lockTime, bool usePassphrase) {
    (void)usePassphrase;

    SignTx stx;

    stx.set_outputs_count((uint32_t)outputs.size());
    stx.set_inputs_count((uint32_t)inputs.size());
    stx.set_coin_name(coinName.c_str(), coinName.length());
    stx.set_version(version);
    stx.set_lock_time(lockTime);

    m_inputs = inputs;
    m_outputs = outputs;

    if (sendMsg(MessageType_SignTx, stx) != 0) return (-1);

    return processCommand(&retSignedTx);
}

int trezorDecive::signEthTx(std::string &retSignedTx, const std::string &toAddress, uint64_t value, uint64_t nonce,
                            uint64_t gasPrice, uint64_t gasLimit, const std::string &derivationPath, bool usePassphrase,
                            const std::string &data, wallet::ethChainId chainId) {
    EthereumSignTx setx;

    (void)usePassphrase;

    setx.set_to(hex2bin(toAddress).data(), toAddress.length() / 2);
    setx.set_value(int2be(value));
    setx.set_gas_price(int2be(gasPrice));
    setx.set_gas_limit(int2be(gasLimit));
    setx.set_nonce(int2be(nonce));

    if (chainId != wallet::ethChainId_Unknown) {
        setx.set_chain_id(chainId);
    }

    m_ethData.clear();
    m_ethDataPos = 0;

    if (data.length() > 0) {
        m_ethData = hex2bin(data);

        size_t count = m_ethData.size() <= 1024 ? m_ethData.size() : 1024;

        setx.set_data_length((uint32_t)m_ethData.size());
        setx.set_data_initial_chunk(m_ethData.data() + m_ethDataPos, count);

        m_ethDataPos += (uint32_t)count;
    }

    if (!derivationPath.empty()) {
        std::vector<uint32_t> n = bip32path2arr(derivationPath);
        for (size_t i = 0; i < n.size() && i < 6; i++)
            setx.add_address_n(n[i]);
    }

    if (sendMsg(MessageType_EthereumSignTx, setx) != 0) return (-1);

    signEthRetValues retValues;

    if (processCommand(&retValues) != 0) return (-1);

    const std::vector<uint8_t> r(retValues.lastR.begin(), retValues.lastR.end());
    const std::vector<uint8_t> s(retValues.lastS.begin(), retValues.lastS.end());

    retSignedTx =
        bin2hex(rlp_encode(hex2bin(toAddress), value, nonce, gasPrice, gasLimit, m_ethData, retValues.lastV, r, s));

    return 0;
}

int trezorDecive::getAddress(std::string &retAddress, const std::string &coinName, const std::string &derivationPath,
                             wallet::inputScriptType scriptType, bool display, bool usePassphrase) {
    std::list<std::string> retAddresses;

    (void)usePassphrase;

    if (getAddresses(retAddresses, 1, coinName, derivationPath, scriptType, display, usePassphrase) != 0) return (-1);

    retAddress = retAddresses.front();

    return 0;
}

int trezorDecive::getAddresses(std::list<std::string> &retAddresses, unsigned int addressesNum,
                               const std::string &coinName, const std::string &derivationPath,
                               wallet::inputScriptType scriptType, bool display, bool usePassphrase)

{
    std::vector<uint32_t> n;
    std::string address;
    unsigned int addrIdx = 0;

    (void)usePassphrase;

    while (addrIdx < addressesNum) {
        if (!derivationPath.empty()) {
            n = bip32path2arr(derivationPath);
            if (n.size()) n[n.size() - 1] += addrIdx;
        }

        if (coinName == "Ethereum") {
            EthereumGetAddress ega;

            for (size_t i = 0; i < n.size() && i < 10; i++)
                ega.add_address_n(n[i]);

            ega.set_show_display(display);

            if (sendMsg(MessageType_EthereumGetAddress, ega) != 0) return -1;

            if (processCommand(&address) != 0) return 0;
        } else {
            GetAddress ga;

            for (size_t i = 0; i < n.size() && i < 10; i++)
                ga.add_address_n(n[i]);

            ga.set_show_display(display);
            ga.set_coin_name(coinName.c_str(), coinName.length());

            if (scriptType != wallet::InputScriptType_None) ga.set_script_type((InputScriptType)scriptType);

            if (sendMsg(MessageType_GetAddress, ga) != 0) return -1;

            if (processCommand(&address) != 0) return 0;
        }

        retAddresses.push_back(address);
        addrIdx++;
    }

    return 0;
}

int trezorDecive::wipe(void) {
    if (sendMsg(MessageType_WipeDevice) != 0) return (-1);

    return processCommand(NULL);
}

int trezorDecive::changePin(void) {
    if (sendMsg(MessageType_ChangePin) != 0) return (-1);

    return processCommand(NULL);
}

int trezorDecive::recovery(const std::string &label, bool pinProtection, bool passphraseProtection, int wordCount) {
    RecoveryDevice rd;

    rd.set_passphrase_protection(passphraseProtection);
    rd.set_word_count(wordCount);
    rd.set_label(label);
    rd.set_enforce_wordlist(true);
    rd.set_pin_protection(pinProtection);

    if (sendMsg(MessageType_RecoveryDevice, rd) != 0) return (-1);

    return processCommand(NULL);
}

int trezorDecive::setup(const std::string &label, bool pinProtection, bool passphraseProtection, int wordCount) {
    ResetDevice rd;
    int strength;

    if (wordCount == 12)
        strength = 128;
    else if (wordCount == 18)
        strength = 192;
    else
        strength = 256;

    rd.set_passphrase_protection(passphraseProtection);
    rd.set_strength(strength);
    rd.set_label(label);
    rd.set_pin_protection(pinProtection);

    if (sendMsg(MessageType_ResetDevice, rd) != 0) return (-1);

    return processCommand(NULL);
}

int trezorDecive::ping(void) {
    std::string message = "pong";
    Ping ping;

    ping.set_message(message.c_str(), message.length());

    if (sendMsg(MessageType_Ping, ping) != 0) return (-1);

    return processCommand(NULL);
}

int trezorDecive::getXPub(std::string &xpub) {
    if (sendMsg(MessageType_GetPublicKey) != 0) return (-1);

    return processCommand(&xpub);
}

int trezorDecive::userActionAck(int userAckType, const std::string &data) {
    if (userAckType == MessageType_ButtonAck) {
        if (sendMsg(MessageType_ButtonAck) != 0) return (-1);
    } else if (userAckType == MessageType_WordAck) {
        WordAck wa;

        wa.set_word(data.c_str(), data.length());
        if (sendMsg(userAckType, wa) != 0) return (-1);
    } else if (userAckType == MessageType_PinMatrixAck) {
        PinMatrixAck pma;

        pma.set_pin(data.c_str(), data.length());
        if (sendMsg(userAckType, pma) != 0) return (-1);
    } else if (userAckType == MessageType_PassphraseAck) {
        PassphraseAck pa;

        pa.set_passphrase(data.c_str(), data.length());
        if (sendMsg(userAckType, pa) != 0) return (-1);
    } else if (userAckType == MessageType_EntropyAck) {
        if (sendMsg(userAckType) != 0) return (-1);
    }

    return 0;
}

int trezorDecive::cancel(void) {
    if (sendMsg(MessageType_Cancel) != 0) return (-1);

    return 0;
}

int trezorDecive::getWalletInfo(wallet::walletInfo &winfo) {
    winfo.initialized = false;
    winfo.xpub.clear();

    if (!isDeviceConnected()) return (-1);

    if (sendMsg(MessageType_Initialize) != 0) return (-1);

    if (processCommand(&winfo) != 0) return (-1);

    if (getXPub(winfo.xpub) == 0) {
        winfo.initialized = true;
    }

    return 0;
}

int trezorDecive::sendMsg(uint16_t msgType) {
    if (!isDeviceConnected()) {
        WALLETERR("failed to send command, device is not connected\n");
        return -1;
    }

    std::vector<unsigned char> msg;

    buildHeader(msg, msgType, 0);

    WALLETDEBUG("SEND message: %s\n", MessageType_Name((MessageType)msgType).c_str());

    if (m_channel->writeChannel(msg) < 0) {
        WALLETERR("failed to send command, write error\n");
        return (-1);
    }

    return 0;
}

int trezorDecive::sendMsg(uint16_t msgType, const ::google::protobuf::Message &pbMsg) {
    if (!isDeviceConnected()) {
        WALLETERR("failed to send command, device is not connected\n");
        return -1;
    }

    std::string str = pbMsg.SerializeAsString();
    std::vector<unsigned char> data;

    buildHeader(data, msgType, (uint32_t)str.length());

    std::copy(str.c_str(), str.c_str() + str.length(), back_inserter(data));

    WALLETDEBUG("SEND message: %s\n", MessageType_Name((MessageType)msgType).c_str());

    WALLETVERBOSE("   # %s\n", pbMsg.DebugString().c_str());

    if (m_channel->writeChannel(data) < 0) {
        WALLETERR("failed to send command, write error\n");
        return (-1);
    }

    return 0;
}

int trezorDecive::processCommand(void *retValue) {
    int rc = (-1);
    std::vector<unsigned char> header;
    std::vector<unsigned char> currentMsg;
    uint16_t currentMsgType = 0;
    uint32_t currentMsgLength = 0;

    while (1) {
        std::vector<unsigned char> inData;

        if (header.size() < TREZOR_HEADER_SIZE) {
            if (m_channel->readChannel(inData, TREZOR_HEADER_SIZE - header.size()) <= 0) break;

            header.insert(header.end(), inData.begin(), inData.end());

            if ((header.size() >= 1 && header[0] != '#') || (header.size() >= 2 && header[1] != '#')) {
                header.erase(header.begin(), header.begin() + 1);
                continue;
            }

            if (header.size() == TREZOR_HEADER_SIZE) {
                parseHeader(header, &currentMsgType, &currentMsgLength);
            } else {
                continue;
            }
        }

        if (m_channel->readChannel(inData, currentMsgLength - currentMsg.size()) < 0) break;

        currentMsg.insert(currentMsg.end(), inData.begin(), inData.end());

        if (currentMsgLength == (uint32_t)currentMsg.size()) {

            WALLETDEBUG("RECV message: %s\n", MessageType_Name((MessageType)currentMsgType).c_str());

            if ((rc = processMsg(currentMsgType, currentMsg, retValue)) != 1) break;

            header.clear();
            currentMsg.clear();
        }
    }
    return rc;
}

int trezorDecive::processMsg(uint16_t msgType, std::vector<unsigned char> data, void *retValue) {
    int rc = 0;

    if (msgType == MessageType_TxRequest) {
        rc = processTxRequestMsg(data, retValue);
    } else if (msgType == MessageType_EthereumTxRequest) {
        EthereumTxRequest etxr;

        etxr.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s", etxr.DebugString().c_str());

        if (etxr.has_data_length()) {
            EthereumTxAck etxa;

            int count = (std::min)((int)(m_ethData.size() - m_ethDataPos), (int)etxr.data_length());

            etxa.set_data_chunk(m_ethData.data() + m_ethDataPos, count);

            m_ethDataPos += count;

            if (sendMsg(MessageType_EthereumTxAck, etxa) != 0) return (-1);

            rc = 1;
        } else {
            ((signEthRetValues *)retValue)->lastR = etxr.signature_r();
            ((signEthRetValues *)retValue)->lastS = etxr.signature_s();
            ((signEthRetValues *)retValue)->lastV = etxr.signature_v();
        }
    } else if (msgType == MessageType_Address) {
        Address a;

        a.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s\n", a.DebugString().c_str());

        *(std::string *)retValue = a.address();
    } else if (msgType == MessageType_MessageSignature) {
        MessageSignature ms;

        ms.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s\n", ms.DebugString().c_str());

        *(std::string *)retValue = ms.signature();
    } else if (msgType == MessageType_EthereumAddress) {
        EthereumAddress aa;

        aa.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   #%s\n", aa.DebugString().c_str());

        *(std::string *)retValue = bin2hex(aa.address());
    } else if (msgType == MessageType_PassphraseRequest) {
        std::string passphrase;
        PassphraseRequest psr;

        psr.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   #%s\n", psr.DebugString().c_str());

        userActionRequest(wallet::UserActionReqType_Passphrase, passphrase);

        if (userActionAck(MessageType_PassphraseAck, passphrase) != 0) return (-1);

        rc = 1;
    } else if (msgType == MessageType_ButtonRequest) {
        std::string dummy;
        ButtonRequest br;

        br.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s\n", br.DebugString().c_str());

        if (userActionRequest(wallet::UserActionReqType_Button, dummy) != 0) return (-1);

        if (userActionAck(MessageType_ButtonAck) != 0) return (-1);

        rc = 1;
    } else if (msgType == MessageType_PinMatrixRequest) {
        std::string pinCode;
        wallet::userActionReqType userReqType;
        PinMatrixRequest pmr;

        pmr.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s\n", pmr.DebugString().c_str());

        if (pmr.type() == PinMatrixRequestType_Current)
            userReqType = wallet::UserActionReqType_CurrentPin;
        else if (pmr.type() == PinMatrixRequestType_NewFirst)
            userReqType = wallet::UserActionReqType_NewFirstPin;
        else if (pmr.type() == PinMatrixRequestType_NewSecond)
            userReqType = wallet::UserActionReqType_NewSecondPin;
        else
            userReqType = wallet::UserActionReqType_CurrentPin;

        if (userActionRequest(userReqType, pinCode) != 0) return (-1);

        if (userActionAck(MessageType_PinMatrixAck, pinCode) != 0) return (-1);

        rc = 1;
    } else if (msgType == MessageType_WordRequest) {
        std::string word;

        if (userActionRequest(wallet::UserActionReqType_Word, word) != 0) return (-1);

        if (userActionAck(MessageType_WordAck, word) != 0) return (-1);

        rc = 1;
    } else if (msgType == MessageType_Features) {
        Features f;

        f.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s\n", f.DebugString().c_str());

        ((wallet::walletInfo *)retValue)->vendorName = f.vendor().c_str();

        char firmwareRev[50];
        sprintf(firmwareRev, "%d.%d.%d", f.major_version(), f.minor_version(), f.patch_version());
        ((wallet::walletInfo *)retValue)->firmwareRev = firmwareRev;

        ((wallet::walletInfo *)retValue)->deviceId = f.device_id().c_str();

        for (int i = 0; i < f.coins_size(); i++) {
            wallet::coin c;

            c.name = f.coins(i).coin_name().c_str();
            c.shortcut = f.coins(i).coin_shortcut().c_str();

            ((wallet::walletInfo *)retValue)->supportedCoins.push_back(c);
        }
    } else if (msgType == MessageType_PublicKey) {
        PublicKey pk;

        pk.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s\n", pk.DebugString().c_str());

        *(std::string *)retValue = pk.xpub();

        rc = 0;
    } else if (msgType == MessageType_EntropyRequest) {
        std::string dummy;

        if (userActionRequest(wallet::UserActionReqType_Entropy, dummy) != 0) return (-1);

        if (userActionAck(MessageType_EntropyAck, dummy) != 0) return (-1);

        rc = 1;
    } else if (msgType == MessageType_Success) {
        Success sc;

        sc.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s\n", sc.DebugString().c_str());

        m_ResultString = sc.message().c_str();
        rc = 0;
    } else if (msgType == MessageType_Failure) {
        Failure f;

        f.ParseFromArray(data.data(), (int)data.size());
        WALLETVERBOSE("   # %s\n", f.DebugString().c_str());

        m_ResultString = f.message().c_str();
        rc = (-1);
    }

    return rc;
}

int trezorDecive::processTxRequestMsg(std::vector<unsigned char> data, void *retValue) {
    TxRequest txr;
    TxAck txa;
    std::string currTxHash;
    uint32_t currRequestIndex = 0;
    TransactionType *transactionType = new TransactionType();

    txr.ParseFromArray(data.data(), (int)data.size());

    WALLETVERBOSE("   # %s\n", txr.DebugString().c_str());

    if (txr.details().has_tx_hash()) currTxHash = txr.details().tx_hash();

    if (txr.details().has_request_index()) currRequestIndex = txr.details().request_index();

    if (txr.request_type() == TXINPUT) {
        TxInputType *txInputType;

        txInputType = transactionType->add_inputs();

        if (currTxHash.empty()) {

            if (!m_inputs[currRequestIndex].derivationPath.empty()) {
                std::vector<uint32_t> n = bip32path2arr(m_inputs[currRequestIndex].derivationPath);
                for (size_t i = 0; i < n.size() && i < 6; i++)
                    txInputType->add_address_n(n[i]);
            }
            txInputType->set_prev_index(m_inputs[currRequestIndex].prevIndex);
            txInputType->set_prev_hash(hex2bin(m_inputs[currRequestIndex].prevHash).data(),
                                       m_inputs[currRequestIndex].prevHash.length() / 2);
        } else {
            wallet::txInput txIn;

            if (txDetaiesRequest(wallet::TxDetaiesReqType_Input, bin2hex(currTxHash), currRequestIndex, &txIn) != 0)
                return (-1);

            txInputType->set_script_sig(hex2bin(txIn.scriptSignature).data(), txIn.scriptSignature.length() / 2);
            txInputType->set_prev_hash(hex2bin(txIn.prevHash).data(), txIn.prevHash.length() / 2);
            txInputType->set_prev_index(txIn.prevIndex);
            if (txIn.scriptType > wallet::InputScriptType_None)
                txInputType->set_script_type(::InputScriptType(txIn.scriptType));
        }

        txa.set_allocated_tx(transactionType);

        if (sendMsg(MessageType_TxAck, txa) != 0) return (-1);

        return 1;
    } else if (txr.request_type() == TXOUTPUT) {
        if (currTxHash.empty()) {
            TxOutputType *txOutputType;

            txOutputType = transactionType->add_outputs();

            if (!m_outputs[currRequestIndex].derivationPath.empty()) {
                std::vector<uint32_t> n = bip32path2arr(m_outputs[currRequestIndex].derivationPath);
                for (size_t i = 0; i < n.size() && i < 6; i++)
                    txOutputType->add_address_n(n[i]);
            }
            txOutputType->set_amount(m_outputs[currRequestIndex].amount);
            txOutputType->set_address(m_outputs[currRequestIndex].address);
            if (m_outputs[currRequestIndex].scriptType > wallet::OutputScriptType_None)
                txOutputType->set_script_type(::OutputScriptType(m_outputs[currRequestIndex].scriptType));
        } else {
            TxOutputBinType *txOutputBinType;

            txOutputBinType = transactionType->add_bin_outputs();

            wallet::txOutputBin txOutBin;
            if (txDetaiesRequest(wallet::TxDetaiesReqType_BinOutput, bin2hex(currTxHash), currRequestIndex,
                                 &txOutBin) != 0)
                return (-1);

            txOutputBinType->set_amount(txOutBin.amount);
            txOutputBinType->set_script_pubkey(hex2bin(txOutBin.scriptPubkey).data(),
                                               txOutBin.scriptPubkey.length() / 2);
        }

        txa.set_allocated_tx(transactionType);

        if (sendMsg(MessageType_TxAck, txa) != 0) return (-1);

        return 1;
    } else if (txr.request_type() == TXMETA) {
        wallet::txMetadata metadata;

        if (txDetaiesRequest(wallet::TxDetaiesReqType_Metadata, bin2hex(currTxHash), currRequestIndex, &metadata) != 0)
            return (-1);

        transactionType->set_inputs_cnt(metadata.inputsCount);
        transactionType->set_outputs_cnt(metadata.outputsCount);
        transactionType->set_extra_data_len(metadata.extraDataLen);
        transactionType->set_version(metadata.version);
        transactionType->set_lock_time(metadata.lockTime);
        txa.set_allocated_tx(transactionType);

        if (sendMsg(MessageType_TxAck, txa) != 0) return (-1);

        return 1;
    } else if (txr.request_type() == TXFINISHED) {
        *(std::string *)retValue = bin2hex(txr.serialized().serialized_tx());
    } else if (txr.request_type() == TXEXTRADATA) {
        return (-1);
    }

    return 0;
}

static int buildHeader(std::vector<unsigned char> &header, uint16_t msgType, uint32_t length) {
    header.resize(TREZOR_HEADER_SIZE);

    header[0] = '#';
    header[1] = '#';

    msgType = ntohs(msgType);
    header[3] = msgType >> 8;
    header[2] = msgType & 0xFF;

    length = (uint32_t)ntohl(length);
    header[7] = length >> 24;
    header[6] = (length >> 16) & 0xFF;
    header[5] = (length >> 8) & 0xFF;
    header[4] = length & 0xFF;

    return 0;
}

static int parseHeader(const std::vector<unsigned char> &header, uint16_t *type, uint32_t *length) {
    if (header[0] != '#' || header[1] != '#') return (-1);

    *type = htons(uint16_t((header[2] << 0) | (header[3] << 8)));
    *length = htonl(uint32_t((header[4] << 0) | (header[5] << 8) | (header[6] << 16) | (header[7] << 24)));

    return 0;
}
