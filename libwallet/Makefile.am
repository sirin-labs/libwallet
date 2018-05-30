AM_CXXFLAGS = -g -std=c++11 -Wall -fPIC
AM_CPPFLAGS =

lib_LTLIBRARIES = libwallet.la
libwallet_la_SOURCES = wallet.cpp \
						 common/utils.cpp \
						 common/rlp.cpp \
						 channels/channel.cpp \
						 channels/hidchannel.cpp \
						 channels/tcpchannel.cpp \
						 trezor/trezordecive.cpp

libwallet_la_LIBADD = $(LIBUSB_LIBS) $(LIBHID_LIBS) $(PROTOBUF_LIBS)
libwallet_la_CFLAGS = $(LIBUSB_CFLAGS) $(LIBHID_CFLAGS) $(PROTOBUF_CFLAGS) $(AM_CFLAGS)

include_HEADERS = wallet.h

dist_noinst_DATA = trezor/protob/config.proto \
				   trezor/protob/messages.proto \
				   trezor/protob/storage.proto \
				   trezor/protob/types.proto

nodist_libwallet_la_SOURCES = @builddir@/trezor/protob/config.pb.cc @builddir@/trezor/protob/config.pb.h \
						@builddir@/trezor/protob/messages.pb.cc @builddir@/trezor/protob/messages.pb.h \
						@builddir@/trezor/protob/storage.pb.cc @builddir@/trezor/protob/storage.pb.h \
						@builddir@/trezor/protob/types.pb.cc @builddir@/trezor/protob/types.pb.h


MOSTLYCLEANFILES = trezor/protob/config.pb.cc trezor/protob/config.pb.h \
						trezor/protob/messages.pb.cc trezor/protob/messages.pb.h \
						trezor/protob/storage.pb.cc trezor/protob/storage.pb.h \
						trezor/protob/types.pb.cc trezor/protob/types.pb.h

BUILT_SOURCES = trezor/protob/config.pb.h \
				trezor/protob/messages.pb.h \
				trezor/protob/storage.pb.h \
				trezor/protob/types.pb.h

%.pb.cc %.pb.h: %.proto
	$(PROTOC) --proto_path=$(dir $^) --cpp_out=$(dir $^) $^
