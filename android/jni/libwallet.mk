# Android build config for libwallet
# Copyright © 2018 SirinLabs Ltd. <uriy@sirinlabs.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#

LOCAL_PATH:= $(call my-dir)

# libwallet

include $(CLEAR_VARS)

TARGET_PIE := true
NDK_APP_PIE := true

LIBHWWALLET_ROOT_REL:= ../..
LIBHWWALLET_ROOT_ABS:= $(LOCAL_PATH)/../..

LOCAL_SHARED_LIBRARIES := libusb1.0
LOCAL_SHARED_LIBRARIES := libhid
LOCAL_STATIC_LIBRARIES := protobuf

LOCAL_SRC_FILES := \
	$(LIBHWWALLET_ROOT_REL)/libwallet/trezor/protob/config.pb.cc \
	$(LIBHWWALLET_ROOT_REL)/libwallet/trezor/protob/messages.pb.cc \
	$(LIBHWWALLET_ROOT_REL)/libwallet/trezor/protob/storage.pb.cc \
	$(LIBHWWALLET_ROOT_REL)/libwallet/trezor/protob/types.pb.cc \
	$(LIBHWWALLET_ROOT_REL)/libwallet/wallet.cpp \
	$(LIBHWWALLET_ROOT_REL)/libwallet/common/utils.cpp \
	$(LIBHWWALLET_ROOT_REL)/libwallet/common/rlp.cpp \
	$(LIBHWWALLET_ROOT_REL)/libwallet/channels/channel.cpp \
	$(LIBHWWALLET_ROOT_REL)/libwallet/channels/hidchannel.cpp \
	$(LIBHWWALLET_ROOT_REL)/libwallet/channels/tcpchannel.cpp \
	$(LIBHWWALLET_ROOT_REL)/libwallet/trezor/trezordecive.cpp

LOCAL_C_INCLUDES += \
	$(LIBHWWALLET_ROOT_ABS)/libwallet

LOCAL_EXPORT_C_INCLUDES := \
	$(LIBHWWALLET_ROOT_ABS)/libwallet

LOCAL_LDLIBS := -lz
LOCAL_CFLAGS := -DGOOGLE_PROTOBUF_NO_RTTI
LOCAL_CPPFLAGS := -std=c++11 -fexceptions -DANDROID

LOCAL_MODULE := libwallet

LOCAL_DISABLE_FATAL_LINKER_WARNINGS=true

include $(BUILD_SHARED_LIBRARY)
