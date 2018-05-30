AM_CXXFLAGS = -g -std=c++11 -Wall -fPIC
AM_CPPFLAGS = -I$(top_srcdir)/libwallet
LDADD = ../libwallet/libwallet.la

bin_PROGRAMS = setupdev wipedev getaddress signethtx getinfo

setupdev_SOURCES = common.cpp setupdev.cpp
setupdev_CFLAGS = $(AM_CFLAGS)

wipedev_SOURCES = common.cpp wipedev.cpp
wipedev_CFLAGS = $(AM_CFLAGS)

getaddress_SOURCES = common.cpp getaddress.cpp
getaddress_CFLAGS = $(AM_CFLAGS)

signethtx_SOURCES = common.cpp signethtx.cpp
signethtx_CFLAGS = $(AM_CFLAGS)

getinfo_SOURCES = common.cpp getinfo.cpp
getinfo_CFLAGS = $(AM_CFLAGS)
