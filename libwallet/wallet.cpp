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

#include "wallet.h"
#include "common/utils.h"

class wallet::walletDev *wallet::walletDev::getDevice(const std::string &devType) {
    return NULL;
}

std::string wallet::walletDev::getResultString(void) const { return m_ResultString; }

void wallet::walletDev::setDebugLevel(unsigned short debugLevel) { m_debugLevel = debugLevel; }

int wallet::walletDev::registerUserActionRequestHandler(
    int (*userActionRequestHandler)(userActionReqType userActReqType, std::string &)) {
    m_userActionRequestHandler = userActionRequestHandler;

    return 0;
}

int wallet::walletDev::registerTxDetailsRequestHandler(int (*txDetaiesRequestHandler)(txDetaiesRequestType,
                                                                                      const std::string &, uint32_t,
                                                                                      void *)) {
    m_txDetaiesRequestHandler = txDetaiesRequestHandler;

    return 0;
}

int wallet::walletDev::userActionRequest(userActionReqType userActReqType, std::string &retInput) {
    if (!m_userActionRequestHandler) return (-1);

    return m_userActionRequestHandler(userActReqType, retInput);
}

int wallet::walletDev::txDetaiesRequest(txDetaiesRequestType txDetaiesReqType, const std::string &hash,
                                        uint32_t reqIndex, void *data) {
    if (!m_txDetaiesRequestHandler) return (-1);

    return m_txDetaiesRequestHandler(txDetaiesReqType, hash, reqIndex, data);
}
