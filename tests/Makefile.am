AM_CXXFLAGS = -g -std=c++11 -Wall -fPIC
AM_CPPFLAGS = -I$(top_srcdir)/libwallet
LDADD = ../libwallet/libwallet.la

bin_PROGRAMS = signtx_bitcoin_test

signtx_bitcoin_test_SOURCES = common.cpp signtx_bitcoin_test.cpp
signtx_bitcoin_test_CFLAGS = $(AM_CFLAGS)
