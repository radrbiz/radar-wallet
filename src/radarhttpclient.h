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

#ifndef RADARHTTPCLIENT_H
#define RADARHTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>
#include <balance.h>
#include <json.hpp>

class RadarHttpClient : public QObject
{
    Q_OBJECT
public:
    explicit RadarHttpClient(QObject *parent = nullptr);
    static RadarHttpClient& Instance();

    bool FetchAccountBalances(const std::string &account);
    uint64_t FetchAccountTxSeqSync(const std::string &account);
    uint64_t FetchLastValidateLedgerSync();
    bool SubmitTxSync(const std::string &tx);
    bool CheckTxSync(const std::string &tx_hash);
    bool CheckAccountExistSync(const std::string &address);

signals:

    void FetchAccountBalancesDone(const std::string &account, const std::vector<Balance> &balances);
//    void FetchAccountTxSeqDone(const std::string &account, uint64_t seq);

public slots:

private:
    QNetworkAccessManager net_mgr;
    QNetworkAccessManager net_mgr_sync;
    bool PostToRadard(const std::string &content, std::function<void(QNetworkReply *)> callback);
    QNetworkReply* PostToRadardSync(const std::string &content);
    bool CheckReturn(const nlohmann::json &);
};

#endif // RADARHTTPCLIENT_H
