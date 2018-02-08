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

#include "addressbookdialog.h"
#include "ui_addressbookdialog.h"
#include "addressbook.h"
#include "radardelegate.h"
#include <QMessageBox>

AddressBookDialog::AddressBookDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressBookDialog),
    chosen_address("")
{
    ui->setupUi(this);
    UpdateList();
}

AddressBookDialog::~AddressBookDialog()
{
    delete ui;
}

void AddressBookDialog::UpdateList()
{
    ui->listWidgetAddressBook->clear();
    auto &ab = AddressBook::Instance();
    for (auto it : ab.AddressList()) {
        ui->listWidgetAddressBook->addItem(it.c_str());
    }
}

void AddressBookDialog::on_pushButtonAdd_clicked()
{
    std::string key = ui->lineEditAddress->text().toStdString();
    if (key.empty()) {
        QMessageBox::warning(this, "Alert", "Empty address to add.", QMessageBox::Close);
        return;
    }
    if (RadarDelegate::Instance().KeyCheck(key)) {
        if (AddressBook::Instance().Add(key)) {
            UpdateList();
        } else {
            QMessageBox::warning(this, "Alert", "Address already exist.", QMessageBox::Close);
        }
    } else {
        QMessageBox::warning(this, "Alert", "Invalid address to add.", QMessageBox::Close);
    }
    ui->lineEditAddress->setText("");
}

void AddressBookDialog::on_pushButtonCancel_clicked()
{
    close();
}

void AddressBookDialog::on_pushButtonChoose_clicked()
{
    if (ui->listWidgetAddressBook->selectedItems().size() > 0) {
        chosen_address = ui->listWidgetAddressBook->selectedItems()[0]->text().toStdString();
        close();
    } else {
        QMessageBox::warning(this, "Alert", "Choose an address in list first.", QMessageBox::Close);
    }
}

void AddressBookDialog::on_pushButtonRemove_clicked()
{
    if (ui->listWidgetAddressBook->selectedItems().size() > 0) {
        std::string address = ui->listWidgetAddressBook->selectedItems()[0]->text().toStdString();
        AddressBook::Instance().Remove(address);
        UpdateList();
    } else {
        QMessageBox::warning(this, "Alert", "Choose an address in list first.", QMessageBox::Close);
    }
}

void AddressBookDialog::on_pushButtonRemoveAll_clicked()
{
    AddressBook::Instance().RemoveAll();
    UpdateList();
}

std::string AddressBookDialog::ChosenAddress()
{
    return chosen_address;
}

void AddressBookDialog::on_listWidgetAddressBook_itemDoubleClicked(QListWidgetItem *item)
{
    chosen_address = item->text().toStdString();
    close();
}
