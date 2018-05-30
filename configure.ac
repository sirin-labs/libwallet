AC_PREREQ([2.69])
AC_INIT([libwallet], [0.1.0], [uriy@sirinlabs.com])

AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE([subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S

LT_INIT

# Checks for libraries.

PKG_CHECK_MODULES(LIBUSB, libusb-1.0 >= 1.0.9)
AC_SUBST(LIBUSB_CFLAGS)
AC_SUBST(LIBUSB_LIBS)

PKG_CHECK_MODULES(LIBHID, hidapi-libusb >= 0.8.0)
AC_SUBST(LIBHID_CFLAGS)
AC_SUBST(LIBHID_LIBS)

# check for protobuf
PKG_CHECK_MODULES(PROTOBUF, protobuf >= 2.4.0)
AC_SUBST(PROTOBUF_LIBS)
AC_SUBST(PROTOBUF_CFLAGS)
AC_SUBST(PROTOBUF_VERSION)

AC_CHECK_PROG([PROTOC], [protoc], [protoc])
AS_IF([test "x${PROTOC}" == "x"],
       [AC_MSG_ERROR([ProtoBuf compiler "protoc" not found.])])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.

AC_CONFIG_FILES([libwallet.pc])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([libwallet/Makefile])
AC_OUTPUT
