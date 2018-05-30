TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../../../examples/getinfo.cpp \
    ../../../examples/common.cpp

HEADERS += \
    ../../../examples/common.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libwallet/release/ -lwallet
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libwallet/debug/ -lwallet
else:unix: LIBS += -L$$OUT_PWD/../../libwallet/ -lwallet

INCLUDEPATH += $$PWD/../../../libwallet
DEPENDPATH += $$PWD/../../../libwallet
