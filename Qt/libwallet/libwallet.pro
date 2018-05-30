#-------------------------------------------------
#
# Project created by QtCreator 2018-03-07T12:41:01
#
#-------------------------------------------------

QT       -= gui
QT       += core

TARGET = wallet
TEMPLATE = lib

DEFINES += WALLET_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32: PROTOC =  C:/libwallet-extras/windows-64bit/protobuf/bin/protoc.exe
else:android: PROTOC =  C:/libwallet-extras/android/protobuf/bin/protoc.exe
else:unix: PROTOC = protoc
PROTOS = $$files(../../libwallet/trezor/protob/*.proto)
include(protobuf.pri)
win32:DEFINES += PROTOBUF_USE_DLLS

CONFIG   += c++11

INCLUDEPATH += ../../libwallet/

SOURCES += \
    ../../libwallet/common/utils.cpp \
    ../../libwallet/common/rlp.cpp \
    ../../libwallet/channels/hidchannel.cpp \
    ../../libwallet/channels/tcpchannel.cpp \
    ../../libwallet/channels/channel.cpp \
    ../../libwallet/wallet.cpp \
    ../../libwallet/trezor/trezordecive.cpp \

HEADERS += \
    ../../libwallet/common/utils.h \
    ../../libwallet/common/rlp.h \
    ../../libwallet/channels/hidchannel.h \
    ../../libwallet/channels/tcpchannel.h \
    ../../libwallet/wallet.h \
    ../../libwallet/wallet_global.h \
    ../../libwallet/trezor/trezordecive.h \

FORMS += \
        ../../libwallet/mainwidget.ui

win32 {
    LIBS += -lsetupapi -lWs2_32
    LIBS += -LC:/libwallet-extras/windows-64bit/protobuf/lib -llibprotobufd
    LIBS += -LC:/libwallet-extras/windows-64bit/libhid/lib -lhidapi
    INCLUDEPATH += C:/libwallet-extras/windows-64bit/protobuf/include
    DEPENDPATH += C:/libwallet-extras/windows-64bit/protobuf/include
    INCLUDEPATH += C:/libwallet-extras/windows-64bit/libhid/include
    DEPENDPATH += C:/libwallet-extras/windows-64bit/libhid/include
}
else:android {
    equals(ANDROID_TARGET_ARCH, armeabi-v7a){
        LIBS += -lc++_shared
        LIBS += -LC:/libwallet-extras/android/protobuf/libs/$$ANDROID_TARGET_ARCH -lprotobuf
        LIBS += -LC:/libwallet-extras/android/libusb1.0/libs/$$ANDROID_TARGET_ARCH  -lusb1.0 -pthread
        INCLUDEPATH += C:/libwallet-extras/android/protobuf/include
        DEPENDPATH += C:/libwallet-extras/android/protobuf/include
        INCLUDEPATH += C:/libwallet-extras/android/libusb1.0/include
        DEPENDPATH += C:/libwallet-extras/android/libusb1.0/include
    }
}
else:unix {
    LIBS += -lusb-1.0 -lhidapi-libusb
    LIBS += -L/usr/local/lib -lprotobuf -pthread -lpthread
}
