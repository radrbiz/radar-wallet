//------------------------------------------------------------------------------
/*
    This file is part of Radar Wallet: http://www.radarlab.org
    Copyright (c) 2015 - 2018 Radar Laboratory

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include "config.h"
#include "logger.h"
#include "wallet.h"
#include "passworddialog.h"
#include "account.h"
#include "addressbook.h"

bool AppInit()
{
    auto& config = Config::GlobalConfig();
    // prepare app config
    QString conf_dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!QDir(conf_dir).exists()) {
        QDir().mkdir(conf_dir);
    }
    std::string conf_file_path = conf_dir.toStdString() + "/" + config.CONFIG_FILE_NAME;
    if (!config.Load(conf_file_path)) {
        Logger::Info() << "Config file load fail, create a new one." << std::endl;
        if (!config.Save(conf_file_path)) {
            Logger::Info() << "Config file create fail." << std::endl;
            return false;
        }
    }
    // prepare app data
    QString data_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir(data_dir).exists()) {
        QDir().mkdir(data_dir);
    }
    std::string data_file_path = data_dir.toStdString() + "/" + config.DATA_FILE_NAME;
    if (!Wallet::Instance().Load(data_file_path)) {
        Logger::Info() << "Wallet file - " << data_file_path << " not found, need create." << std::endl;
    }
    // prepare address book data
    QString address_book_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir(address_book_dir).exists()) {
        QDir().mkdir(address_book_dir);
    }
    std::string address_book_file_path = address_book_dir.toStdString() + "/" + config.ADDRESS_BOOK_FILE_NAME;
    if (!AddressBook::Instance().Load(address_book_file_path)) {
        Logger::Info() << "Address book file - " << address_book_file_path << " not found, need create." << std::endl;
        if (!AddressBook::Instance().Save(address_book_file_path)) {
            Logger::Info() << "Address book file create fail." << std::endl;
            return false;
        }
    }

    // initial wallet
    auto& wallet = Wallet::Instance();
    if (!wallet.Load(data_file_path) || wallet.IsEmpty()) {
        // no wallet file found. create a new one
        PasswordDialog dlg(true);
        dlg.exec();
        std::string password = dlg.GetPassword();
        wallet.SetPassword(password);
        Account acc = wallet.NewAccount();
        wallet.AddAccount(acc);
        wallet.SwitchAccount(acc.pub_key);
        wallet.Save(data_file_path);
    }

    return true;
}

bool AppQuit()
{
    Logger::Info() << "app now exit." << std::endl;
    return true;
}

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);

    if (!AppInit()) {
        Logger::Error() << "App init fail, now exit." << std::endl;
        return 1;
    }

    MainWindow w;
    w.show();

    int ret = a.exec();

    if (AppQuit()) {
        return ret;
    } else {
        return 2;
    }
}
