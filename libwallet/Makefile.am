AM_CXXFLAGS = -g -std=c++11 -Wall -fPIC
AM_CPPFLAGS =

lib_LTLIBRARIES = libwallet.la
libwallet_la_SOURCES = wallet.cpp \
						 common/utils.cpp \
						 common/rlp.cpp \
						 channels/channel.cpp \
						 channels/hidchannel.cpp

libwallet_la_LIBADD = $(LIBUSB_LIBS) $(LIBHID_LIBS)
libwallet_la_CFLAGS = $(LIBUSB_CFLAGS) $(LIBHID_CFLAGS)

include_HEADERS = wallet.h
