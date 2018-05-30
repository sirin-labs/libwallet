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

#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wallet.h>

using namespace std;

int userActionRequestHandler(wallet::userActionReqType userActReqType, std::string &input) {
    if (userActReqType == wallet::UserActionReqType_CurrentPin) {
        printf("Enter current PIN CODE ...\n");
        cin >> input;
    } else if (userActReqType == wallet::UserActionReqType_NewFirstPin) {
        printf("Enter new PIN CODE ...\n");
        cin >> input;
    } else if (userActReqType == wallet::UserActionReqType_NewSecondPin) {
        printf("Enter new PIN CODE again ...\n");
        cin >> input;
    } else if (userActReqType == wallet::UserActionReqType_Button) {
        printf("Please press the approve button ...\n");
    } else if (userActReqType == wallet::UserActionReqType_Word) {
        printf("Enter word ...\n");
        cin >> input;
    } else if (userActReqType == wallet::UserActionReqType_Passphrase) {
        printf("Enter passphrase ...\n");
        cin >> input;
    } else if (userActReqType == wallet::UserActionReqType_Entropy) {
    }

    return 0;
}
