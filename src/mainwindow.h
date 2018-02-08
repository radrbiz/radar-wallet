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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void UpdateUI();

private slots:
    void on_toolButtonAddressCopy_clicked();

    void on_toolButtonRefreshBalance_clicked();

    void on_toolButtonAddressBook_clicked();

    void on_pushButtonSend_clicked();

    void on_commandLinkButtonSwitchAccount_clicked();

    void on_actionAdd_Key_triggered();

    void on_actionSave_triggered();

    void on_commandLinkButtonNewAddress_clicked();

    void on_actionRadar_Lab_triggered();

    void on_actionAbout_triggered();

    void on_actionSave_As_triggered();

    void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;
    bool QueryPassword(bool new_pass = false);
};

#endif // MAINWINDOW_H
