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

#include "passworddialog.h"
#include "ui_passworddialog.h"
#include <QMessageBox>
#include "config.h"
#include "wallet.h"

PasswordDialog::PasswordDialog(bool np, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordDialog),
    new_pass(np)
{
    ui->setupUi(this);
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

void PasswordDialog::on_pushButtonOK_clicked()
{
    password = ui->lineEditPassword->text().toStdString();
    if (!new_pass) {
        if (Wallet::Instance().CheckPassword(password)) {
            Wallet::Instance().SetPassword(password);
            this->close();
        } else {
            QMessageBox::warning(this, "Alert", "Wrong password!", QMessageBox::Close);
        }
    } else {
        this->close();
    }
}

std::string PasswordDialog::GetPassword()
{
    return password;
}
