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

#include "radarhttpclient.h"
#include <QNetworkReply>
#include "config.h"
#include <QByteArray>
#include <json.hpp>
#include "logger.h"
#include <string>
#include <QEventLoop>

using json = nlohmann::json;

RadarHttpClient::RadarHttpClient(QObject *parent) : QObject(parent), net_mgr(this), net_mgr_sync(this)
{
}

RadarHttpClient& RadarHttpClient::Instance()
{
    static RadarHttpClient c;
    return c;
}

uint64_t RadarHttpClient::FetchAccountTxSeqSync(const std::string &account)
{
    json req = R"({
        "method" : "account_info",
        "params" : [{
                "ledger_index":"validated"
            }
        ]
    })"_json;
    req["params"][0]["account"] = account;

    Logger::Debug() << req.dump(2) << std::endl;

    QNetworkReply *r = PostToRadardSync(req.dump());
    if (r->error() != QNetworkReply::NoError) {
        Logger::Error() << "FetchAccountTxSeqSync - " << account << " fail." << std::endl;
        return 0;
    }
    std::string content = r->readAll().toStdString();
    Logger::Debug() << content << r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << std::endl;
    try {
        auto j = json::parse(content.c_str());
        if (!CheckReturn(j)) {
            Logger::Error() << "FetchAccountTxSeqSync - " << account << " return error." << std::endl;
            return 0;
        }
        if (j["result"].find("account_data") == j["result"].end()
                || j["result"]["account_data"].find("Account") == j["result"]["account_data"].end()
                || j["result"]["account_data"].find("Sequence") == j["result"]["account_data"].end()
                || !j["result"]["account_data"]["Sequence"].is_number()) {
            Logger::Error() << "FetchAccountTxSeqSync - " << account << " field not found." << std::endl;
            return 0;
        }
        std::string acc = j["result"]["account_data"]["Account"].get<std::string>();
        uint64_t seq = j["result"]["account_data"]["Sequence"].get<uint64_t>();
        return seq;
    } catch (...) {
        Logger::Error() << "FetchAccountTxSeqSync - " << account << " parse return json error." << std::endl;
        return 0;
    }
}

uint64_t RadarHttpClient::FetchLastValidateLedgerSync()
{
    json req = R"({
        "method" : "server_state"
    })"_json;
    QNetworkReply *r = PostToRadardSync(req.dump());
    if (r->error() != QNetworkReply::NoError) {
        Logger::Error() << "FetchLastValidateLedgerSync fail." << std::endl;
        return 0;
    }
    std::string content = r->readAll().toStdString();
    Logger::Debug() << content << std::endl;
    try {
        auto j = json::parse(content.c_str());
        if (!CheckReturn(j)) {
            Logger::Error() << "FetchLastValidateLedgerSync return error." << std::endl;
            return 0;
        }
        if (j["result"].find("state") == j["result"].end()
                || j["result"]["state"].find("validated_ledger") == j["result"]["state"].end()
                || j["result"]["state"]["validated_ledger"].find("seq") == j["result"]["state"]["validated_ledger"].end()
                || !j["result"]["state"]["validated_ledger"]["seq"].is_number()) {
            Logger::Error() << "FetchAccountTxSeqSync field not found." << std::endl;
            return 0;
        }
        uint64_t seq = j["result"]["state"]["validated_ledger"]["seq"].get<uint64_t>();
        return seq;
    } catch (...) {
        Logger::Error() << "FetchAccountTxSeqSync parse return json error." << std::endl;
        return 0;
    }
}

bool RadarHttpClient::SubmitTxSync(const std::string &tx)
{
    json req = R"({
        "method" : "submit",
        "params" : [
        ]
    })"_json;
    req["params"][0] = json::object();
    req["params"][0]["tx_blob"] = tx;
    qDebug() << req.dump(2).c_str();
    QNetworkReply *r = PostToRadardSync(req.dump());
    if (r->error() != QNetworkReply::NoError) {
        Logger::Error() << "SubmitTxSync fail." << std::endl;
        return false;
    }
    std::string content = r->readAll().toStdString();
    Logger::Debug() << content << std::endl;
    try {
        auto j = json::parse(content.c_str());
        if (!CheckReturn(j)) {
            Logger::Error() << "SubmitTxSync return error." << std::endl;
            return false;
        }
        qDebug() << j.dump(2).c_str();
        return true;
    } catch (...) {
        Logger::Error() << "SubmitTxSync parse return json error." << std::endl;
        return false;
    }
}

bool RadarHttpClient::CheckTxSync(const std::string &tx_hash)
{
    json req = R"({
        "method" : "tx",
        "params" : [
        ]
    })"_json;
    req["params"][0] = json::object();
    req["params"][0]["transaction"] = tx_hash;
    QNetworkReply *r = PostToRadardSync(req.dump());
    if (r->error() != QNetworkReply::NoError) {
        Logger::Error() << "CheckTxSync fail." << std::endl;
        return false;
    }
    std::string content = r->readAll().toStdString();
    Logger::Debug() << content << std::endl;
    try {
        auto j = json::parse(content.c_str());
        if (!CheckReturn(j)) {
            Logger::Error() << "CheckTxSync return error." << std::endl;
            return false;
        }
        if (j["result"].find("validated") == j["result"].end()
                || !j["result"]["validated"].is_boolean()) {
            Logger::Error() << "CheckTxSync field not found." << std::endl;
            return false;
        }
        return j["result"]["validated"].get<bool>();
    } catch (...) {
        Logger::Error() << "CheckTxSync parse return json error." << std::endl;
        return false;
    }
}

bool RadarHttpClient::CheckAccountExistSync(const std::string &account)
{
    json req = R"({
        "method" : "account_info",
        "params" : [{
                "ledger_index":"validated"
            }
        ]
    })"_json;
    req["params"][0]["account"] = account;
    QNetworkReply *r = PostToRadardSync(req.dump());
    if (r->error() != QNetworkReply::NoError) {
        Logger::Error() << "CheckAccountExistAsync - " << account << " fail." << std::endl;
        return false;
    }
    std::string content = r->readAll().toStdString();
    Logger::Debug() << content << r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << std::endl;
    try {
        auto j = json::parse(content.c_str());
        if (!CheckReturn(j)) {
            Logger::Error() << "CheckAccountExistAsync - " << account << " return error." << std::endl;
            return false;
        }
        return true;
    } catch (...) {
        Logger::Error() << "CheckAccountExistAsync - " << account << " parse return json error." << std::endl;
        return false;
    }
}

bool RadarHttpClient::FetchAccountBalances(const std::string &account)
{
    json req = R"({
        "method" : "account_info",
        "params" : [{
                "ledger_index":"validated"
            }
        ]
    })"_json;
    req["params"][0]["account"] = account;
    return PostToRadard(req.dump(), [&](QNetworkReply *r) {
        if (r->error() != QNetworkReply::NoError) {
            Logger::Error() << "FetchAccountBalances - " << account << " fail." << std::endl;
            return;
        }
        std::string content = r->readAll().toStdString();
        try {
            auto j = json::parse(content.c_str());
            if (!CheckReturn(j)) {
                Logger::Error() << "FetchAccountBalances - " << account << " return error." << std::endl;
                return;
            }
            if (j["result"].find("account_data") == j["result"].end()
                    || j["result"]["account_data"].find("Account") == j["result"]["account_data"].end()
                    || j["result"]["account_data"].find("Balance") == j["result"]["account_data"].end()
                    || j["result"]["account_data"].find("BalanceVBC") == j["result"]["account_data"].end() ) {
                Logger::Error() << "FetchAccountBalances - " << account << " field not found." << std::endl;
                return;
            }
            std::string acc = j["result"]["account_data"]["Account"].get<std::string>();
            uint64_t vrp = std::stoll(j["result"]["account_data"]["Balance"].get<std::string>());
            uint64_t vbc = std::stoll(j["result"]["account_data"]["BalanceVBC"].get<std::string>());
            std::vector<Balance> balance;
            balance.push_back(Balance("VRP", vrp));
            balance.push_back(Balance("VBC", vbc));
            emit FetchAccountBalancesDone(acc, balance);
        } catch (...) {
            Logger::Error() << "FetchAccountBalances - " << account << " parse return json error." << std::endl;
        }
    });
}

bool RadarHttpClient::CheckReturn(const nlohmann::json &j)
{
    if (j.find("result") == j.end()
            || j["result"].find("status") == j["result"].end()
            || !j["result"]["status"].is_string()
            || j["result"]["status"].get<std::string>() != "success"
        ) {
        return false;
    }
    return true;
}

bool RadarHttpClient::PostToRadard(const std::string &content,
                                   std::function<void(QNetworkReply *)> callback)
{
    auto &config = Config::GlobalConfig();
    std::string url = config.SERVER_SCHEME + "://" + config.SERVER_HOST + ":" + std::to_string(config.SERVER_PORT);
    QUrl base_url(QString::fromStdString(url));
    QNetworkRequest request;
    request.setUrl(base_url);
    QByteArray post_data = QByteArray::fromStdString(content);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QByteArray::number(post_data.size()));
    net_mgr.post(request, post_data);
    QObject::connect(&net_mgr, &QNetworkAccessManager::finished, callback);
    return true;
}

QNetworkReply* RadarHttpClient::PostToRadardSync(const std::string &content)
{
    auto &config = Config::GlobalConfig();
    std::string url = config.SERVER_SCHEME + "://" + config.SERVER_HOST + ":" + std::to_string(config.SERVER_PORT);
    QUrl base_url(QString::fromStdString(url));
    QNetworkRequest request;
    request.setUrl(base_url);
    QByteArray post_data = QByteArray::fromStdString(content);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QByteArray::number(post_data.size()));
    QNetworkReply *r = net_mgr_sync.post(request, post_data);

    QEventLoop eventLoop;
    connect(&net_mgr_sync, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    return r;
}
