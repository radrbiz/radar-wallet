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
#include "ui_mainwindow.h"
#include <QDebug>
#include <QClipboard>
#include "wallet.h"
#include "radardelegate.h"
#include "accountimportdialog.h"
#include "passworddialog.h"
#include "switchaccountdialog.h"
#include <QUrl>
#include <QDesktopServices>
#include "addressbookdialog.h"
#include "aboutdialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    UpdateUI();
    connect(&RadarDelegate::Instance(), &RadarDelegate::DataUpdated, this, &MainWindow::UpdateUI);
    RadarDelegate::Instance().FetchAccountBalances(Wallet::Instance().CurrentAccount().pub_key);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateUI()
{
    auto &acc = Wallet::Instance().CurrentAccount();
    ui->labelCurrentAddressValue->setText(acc.pub_key.c_str());
    ui->labelVBCValue->setText(acc.BalancesString("VBC").c_str());
    ui->labelVRPValue->setText(acc.BalancesString("VRP").c_str());
}

void MainWindow::on_toolButtonAddressCopy_clicked()
{
    QClipboard *board = QApplication::clipboard();
    QString address = ui->labelCurrentAddressValue->text();
    board->setText(address);
    ui->statusBar->showMessage("Address - " + address + " copied to clipboard.", 1000);
}

void MainWindow::on_toolButtonRefreshBalance_clicked()
{
    RadarDelegate::Instance().FetchAccountBalances(Wallet::Instance().CurrentAccount().pub_key);
    ui->statusBar->showMessage("Begin refeshing balance.", 1000);
}

void MainWindow::on_toolButtonAddressBook_clicked()
{
    AddressBookDialog dlg;
    dlg.exec();
    if (dlg.ChosenAddress() != "") {
        ui->lineEditSendToAddress->setText(dlg.ChosenAddress().c_str());
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    std::string address = ui->lineEditSendToAddress->text().toStdString();
    std::string asset = ui->comboBoxSendAsset->currentText().toStdString();
    double value = ui->doubleSpinBoxSendValue->value();
    uint64_t amount = uint64_t(value * 1000000);
    if (!RadarDelegate::Instance().KeyCheck(address)) {
        QMessageBox::warning(this, "Alert", QString("Invalid Address - ") + QString(address.c_str()), QMessageBox::Close);
        return;
    }

    if (amount <= 0) {
        QMessageBox::warning(this, "Alert", QString("Sending amount too small"), QMessageBox::Close);
        return;
    }

    // need password
    if (!QueryPassword()) {
        return;
    }

    auto &wallet = Wallet::Instance();
    auto &acc = wallet.CurrentAccount();
    // register status update callback
    connect(&RadarDelegate::Instance(), &RadarDelegate::TransferStatusUpdated, [&](const std::string &info, RadarDelegate::TransferStatus status){
        ui->statusBar->showMessage(info.c_str(), 1000);
        if (status == RadarDelegate::Transfer_Status_Fail) {
            QMessageBox::warning(this, "Alert", "Transfer failed!", QMessageBox::Close);
        } else if (status == RadarDelegate::Transfer_Status_Success) {
            QMessageBox::information(this, "Transfer Message", "Transfer send ok!", QMessageBox::Close);

            RadarDelegate::Instance().FetchAccountBalances(Wallet::Instance().CurrentAccount().pub_key);
        }
    });
    RadarDelegate::Instance().Transfer(acc.GetPrivateKey(wallet.GetPassword()), acc.pub_key, address, amount, asset);
    ui->lineEditSendToAddress->setText("");
    ui->doubleSpinBoxSendValue->setValue(0);
}

void MainWindow::on_commandLinkButtonSwitchAccount_clicked()
{
    SwitchAccountDialog dlg;
    dlg.exec();
    std::string selected = dlg.GetSelected();
    if (selected != Wallet::Instance().CurrentAccount().pub_key) {
        Wallet::Instance().SwitchAccount(selected);
        Wallet::Instance().Save();
        UpdateUI();
        RadarDelegate::Instance().FetchAccountBalances(Wallet::Instance().CurrentAccount().pub_key);
    }
}

void MainWindow::on_actionAdd_Key_triggered()
{
    if (!QueryPassword()) {
        return;
    }
    AccountImportDialog dlg;
    dlg.exec();
    auto key_pair = dlg.GetKeypair();
    if (key_pair.first != "" && key_pair.second != "") {
        auto acc = Account(Wallet::Instance().password, key_pair.first, key_pair.second);
        Wallet::Instance().AddAccount(acc);
        Wallet::Instance().SwitchAccount(key_pair.first);
    }
    UpdateUI();
    RadarDelegate::Instance().FetchAccountBalances(Wallet::Instance().CurrentAccount().pub_key);
}

bool MainWindow::QueryPassword(bool new_pass)
{
    if (Wallet::Instance().HasPassword()) {
        return true;
    }
    PasswordDialog dlg(new_pass);
    dlg.exec();
    return Wallet::Instance().HasPassword();
}


void MainWindow::on_actionSave_triggered()
{
    Wallet::Instance().Save();
    ui->statusBar->showMessage("Wallet file saved.", 1000);
}

void MainWindow::on_commandLinkButtonNewAddress_clicked()
{
    if (!QueryPassword()) {
        return;
    }
    Account acc = Wallet::Instance().NewAccount();
    Wallet::Instance().AddAccount(acc);
    Wallet::Instance().SwitchAccount(acc.pub_key);
    Wallet::Instance().Save();
    UpdateUI();
    ui->statusBar->showMessage(("New address created, now switch to - " + acc.pub_key).c_str(), 1000);
}

void MainWindow::on_actionRadar_Lab_triggered()
{
    QString URL = "http://www.radarlab.org";
    QDesktopServices::openUrl(QUrl(URL.toLatin1()));
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}

void MainWindow::on_actionSave_As_triggered()
{
    QString default_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString save_path = QFileDialog::getSaveFileName(this, "Export wallet to file.", default_path + "/wallet_bk.json", "Radar Wallet File (*.json)");
    if (!save_path.isNull()) {
        Wallet::Instance().Save(save_path.toStdString());
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if (QMessageBox::warning(this, "Warning", "This operation will overwrite you current wallet file.\n Make sure you have backuped you wallet.",
          QMessageBox::Ok | QMessageBox::Close) == QMessageBox::Close) {
        return;
    }
    QString default_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString load_path = QFileDialog::getOpenFileName(this, "Import waller from file", default_path, "Radar Wallet File (*.json)");
    if (!load_path.isNull()) {
        if (!Wallet::Instance().Load(load_path.toStdString())) {
            QMessageBox::warning(this, "Alert", "Wallet file import fail, check if your file is valid format.", QMessageBox::Close);
        } else {
            Wallet::Instance().Save();
            QMessageBox::information(this, "Notice", "File import ok.", QMessageBox::Close);
            UpdateUI();
        }
    }

}
