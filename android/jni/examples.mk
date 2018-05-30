# Android build config for libwallet examples
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
LIBHWWALLET_ROOT_REL:= ../..
LIBHWWALLET_ROOT_ABS:= $(LOCAL_PATH)/../..

# setupdev

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  $(LIBHWWALLET_ROOT_REL)/examples/setupdev.cpp \
  $(LIBHWWALLET_ROOT_REL)/examples/common.cpp

LOCAL_C_INCLUDES += \
  $(LIBHWWALLET_ROOT_ABS)

LOCAL_SHARED_LIBRARIES += libwallet

LOCAL_MODULE:= setupdev

include $(BUILD_EXECUTABLE)

# wipedev

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  $(LIBHWWALLET_ROOT_REL)/examples/wipedev.cpp \
  $(LIBHWWALLET_ROOT_REL)/examples/common.cpp

LOCAL_C_INCLUDES += \
  $(LIBHWWALLET_ROOT_ABS)

LOCAL_SHARED_LIBRARIES += libwallet

LOCAL_MODULE:= wipedev

include $(BUILD_EXECUTABLE)

# getaddress

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  $(LIBHWWALLET_ROOT_REL)/examples/getaddress.cpp \
  $(LIBHWWALLET_ROOT_REL)/examples/common.cpp

LOCAL_C_INCLUDES += \
  $(LIBHWWALLET_ROOT_ABS)

LOCAL_SHARED_LIBRARIES += libwallet

LOCAL_MODULE:= getaddress

include $(BUILD_EXECUTABLE)

# signethtx

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  $(LIBHWWALLET_ROOT_REL)/examples/signethtx.cpp \
  $(LIBHWWALLET_ROOT_REL)/examples/common.cpp

LOCAL_C_INCLUDES += \
  $(LIBHWWALLET_ROOT_ABS)

LOCAL_SHARED_LIBRARIES += libwallet

LOCAL_MODULE:= signethtx

include $(BUILD_EXECUTABLE)

# getinfo

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  $(LIBHWWALLET_ROOT_REL)/examples/getinfo.cpp \
  $(LIBHWWALLET_ROOT_REL)/examples/common.cpp

LOCAL_C_INCLUDES += \
  $(LIBHWWALLET_ROOT_ABS)

LOCAL_SHARED_LIBRARIES += libwallet

LOCAL_MODULE:= getinfo

include $(BUILD_EXECUTABLE)
