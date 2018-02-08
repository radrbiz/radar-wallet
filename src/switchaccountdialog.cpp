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


#include "switchaccountdialog.h"
#include "ui_switchaccountdialog.h"
#include "wallet.h"

SwitchAccountDialog::SwitchAccountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SwitchAccountDialog)
{
    ui->setupUi(this);
    selected = Wallet::Instance().CurrentAccount().pub_key;
    auto accounts = Wallet::Instance().GetAccounts();
    for (auto it : accounts) {
        ui->listWidgetAccounts->addItem(it.pub_key.c_str());
    }

}

SwitchAccountDialog::~SwitchAccountDialog()
{
    delete ui;
}

std::string SwitchAccountDialog::GetSelected()
{
    return selected;
}

void SwitchAccountDialog::on_listWidgetAccounts_itemDoubleClicked(QListWidgetItem *item)
{
    selected = item->text().toStdString();
    close();
}

void SwitchAccountDialog::on_pushButtonSwitch_clicked()
{
    if (ui->listWidgetAccounts->selectedItems().size() > 0) {
        selected = ui->listWidgetAccounts->selectedItems()[0]->text().toStdString();
    }
    close();
}
