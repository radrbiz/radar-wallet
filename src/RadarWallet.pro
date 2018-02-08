#--------------------------------------------------------------------------------#
#                                                                                #
#    This file is part of Radar Wallet: http://www.radarlab.org                  #
#    Copyright (c) 2015 - 2018 Radar Laboratory                                  #
#                                                                                #
#    Permission to use, copy, modify, and/or distribute this software for any    #
#    purpose  with  or without fee is hereby granted, provided that the above    #
#    copyright notice and this permission notice appear in all copies.           #
#                                                                                #
#    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES   #
#    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF   #
#    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   #
#    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   #
#    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN   #
#    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   #
#    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.              #
#                                                                                #
#--------------------------------------------------------------------------------#

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RadarWallet
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

ICON = ../res/logo.icns
RC_ICONS = ../res/logo.ico

INCLUDEPATH += "../vendor/include"

win32 {
    LIBS += -L"..\vendor\windows\lib" -llibeay32 -lssleay32 -luser32 -lws2_32 -lgdi32 -lshell32 -ladvapi32
    INCLUDEPATH += "..\vendor\windows\include"
}

unix {
    INCLUDEPATH += "/usr/local/include" "/usr/local/ssl/include"
    LIBS += -L"/usr/local/lib" -L"/usr/local/ssl/lib" -lssl -lcrypto
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    account.cpp \
    balance.cpp \
    config.cpp \
    crypto.cpp \
    logger.cpp \
    radardelegate.cpp \
    radarhttpclient.cpp \
    radartool.cpp \
    wallet.cpp \
    passworddialog.cpp \
    accountimportdialog.cpp \
    switchaccountdialog.cpp \
    addressbookdialog.cpp \
    aboutdialog.cpp \
    addressbook.cpp

HEADERS += \
    mainwindow.h \
    account.h \
    balance.h \
    config.h \
    crypto.h \
    logger.h \
    radardelegate.h \
    radarhttpclient.h \
    radartool.h \
    wallet.h \
    passworddialog.h \
    accountimportdialog.h \
    switchaccountdialog.h \
    addressbookdialog.h \
    aboutdialog.h \
    addressbook.h

FORMS += \
    mainwindow.ui \
    passworddialog.ui \
    accountimportdialog.ui \
    switchaccountdialog.ui \
    addressbookdialog.ui \
    aboutdialog.ui

RESOURCES += \
    ../res/base.qrc
