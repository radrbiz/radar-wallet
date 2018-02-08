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

#include "radardelegate.h"
#include <QDebug>
#include <QThread>
#include "radartool.h"
#include "radarhttpclient.h"
#include "logger.h"
#include "wallet.h"
#include <json.hpp>
#include <QTimer>
#include <QEventLoop>

using json = nlohmann::json;

RadarDelegate::RadarDelegate(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<std::string>();
}

RadarDelegate& RadarDelegate::Instance()
{
    static RadarDelegate d;
    return d;
}

std::pair<std::string, std::string> RadarDelegate::KeyGenerate(const std::string &seed)
{
    return RadarTool::Instance().KeyGenerate(seed);
}

std::string RadarDelegate::KeyConvert(const std::string &pri_key)
{
    return RadarTool::Instance().KeyConvert(pri_key);
}

bool RadarDelegate::KeyCheck(const std::string &key)
{
    return RadarTool::Instance().KeyCheck(key);
}

std::pair<std::string, std::string> RadarDelegate::TxSign(const std::string &pri_key, const std::string &tx)
{
    return RadarTool::Instance().TxSign(pri_key, tx);
}

void RadarDelegate::FetchAccountBalances(const std::string &account)
{
    auto &client = RadarHttpClient::Instance();
    connect(&client, &RadarHttpClient::FetchAccountBalancesDone, [&](const std::string &acc, const std::vector<Balance> &balances){
        Wallet::Instance().SetAccountBalances(acc, balances);
        emit DataUpdated();
        return;
    });
    client.FetchAccountBalances(account);
}

void RadarDelegate::Transfer(const std::string &sign_key, const std::string &from, const std::string &to, uint64_t amount, const std::string &asset)
{
    auto &client = RadarHttpClient::Instance();
    // fetch seq
    emit TransferStatusUpdated("fetch tx seq...", Transfer_Status_Running);
    uint64_t seq = RadarHttpClient::Instance().FetchAccountTxSeqSync(from);
    qDebug() << seq;
    if (seq == 0) {
        Logger::Warning() << "fetch seq for - " << from << " fail" << std::endl;
        emit TransferStatusUpdated("fetch seq fail.", Transfer_Status_Fail);
        return;
    }
    // check des account
    uint64_t fee = 1000;
    emit TransferStatusUpdated("checking des account...", Transfer_Status_Running);
    bool des_exist = RadarHttpClient::Instance().CheckAccountExistSync(to);
    if (!des_exist) {
        emit TransferStatusUpdated("des account not actived...", Transfer_Status_Running);
        fee = 11000;
    } else {
        emit TransferStatusUpdated("des account actived...", Transfer_Status_Running);
    }
    // fetch current ledger seq
    emit TransferStatusUpdated("fetching last validated ledger seq...", Transfer_Status_Running);
    uint64_t ledger = RadarHttpClient::Instance().FetchLastValidateLedgerSync();
    qDebug() << ledger;
    if (ledger == 0) {
        Logger::Warning() << "fetch ledger for tx sign fail" << std::endl;
        emit TransferStatusUpdated("fetch for tx sign fail.", Transfer_Status_Fail);
        return;
    }
    uint64_t last_ledger = ledger + 5;
    emit TransferStatusUpdated("generating & signing tx...", Transfer_Status_Running);
    // construct tx
    json tx = json::object();
    tx["TransactionType"] = "Payment";
    tx["Account"] = from;
    if (asset == "VRP") {
        tx["Amount"] = amount;
    } else {
        tx["Amount"] = json::object();
        tx["Amount"]["currency"] = "VBC";
        tx["Amount"]["value"] = std::to_string(amount);
        tx["Amount"]["issuer"] = "rrrrrrrrrrrrrrrrrrrrVFngv46";
    }
    tx["Destination"] = to;
    tx["Sequence"] = seq;
    tx["LastLedgerSequence"] = last_ledger;
    tx["Flags"] = 2147483648;
    tx["Fee"] = std::to_string(fee);
    qDebug() << tx.dump(2).c_str();
    // sign tx
    auto txs = RadarTool::Instance().TxSign(sign_key, tx.dump());
    std::string tx_singed = txs.first;
    std::string tx_hash = txs.second;
    qDebug() << tx_singed.c_str();
    qDebug() << tx_hash.c_str();
    if (tx_singed == "" || tx_hash == "") {
        Logger::Warning() << "tx sign fail" << std::endl;
        emit TransferStatusUpdated("tx sign fail.", Transfer_Status_Fail);
        return;
    }
    // send tx
    emit TransferStatusUpdated("sending to radar net...", Transfer_Status_Running);
    if (!RadarHttpClient::Instance().SubmitTxSync(tx_singed)) {
        Logger::Warning() << "tx submit fail" << std::endl;
        emit TransferStatusUpdated("tx submit fail.", Transfer_Status_Fail);
        return;
    }
    // verify tx result
    emit TransferStatusUpdated("checking tx...", Transfer_Status_Running);
    uint64_t validated_ledger = 0;
    while (validated_ledger <= last_ledger) {
        validated_ledger = RadarHttpClient::Instance().FetchLastValidateLedgerSync();
        qDebug() << validated_ledger;
        if (RadarHttpClient::Instance().CheckTxSync(tx_hash)) {
            emit TransferStatusUpdated("tx applied...", Transfer_Status_Success);
            return;
        }
        QEventLoop eventloop;
        QTimer::singleShot(1000, &eventloop, SLOT(quit()));
        eventloop.exec();
    }
    Logger::Warning() << "tx apply fail" << std::endl;
    emit TransferStatusUpdated("tx apply fail.", Transfer_Status_Fail);
}
