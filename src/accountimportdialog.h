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

#ifndef ACCOUNTIMPORTDIALOG_H
#define ACCOUNTIMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class AccountImportDialog;
}

class AccountImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountImportDialog(QWidget *parent = 0);
    ~AccountImportDialog();
    std::pair<std::string, std::string> GetKeypair();

private slots:
    void on_pushButton_clicked();
    void on_lineEditPrivateKey_textChanged(const QString &arg1);

private:
    Ui::AccountImportDialog *ui;
    std::string pub_key;
    std::string pri_key;
};

#endif // ACCOUNTIMPORTDIALOG_H
