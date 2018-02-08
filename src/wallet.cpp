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

#include "wallet.h"
#include <json.hpp>
#include <time.h>
#include "config.h"
#include "logger.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <radardelegate.h>

using json = nlohmann::json;

#define MAGIC_WORD_ORIGIN  ("RADAR_WALLET")

Wallet& Wallet::Instance()
{
    static Wallet w;
    return w;
}

Wallet::Wallet() : current_account(""),
    version(Config::GlobalConfig().WALLET_VERSION),
    ctime(time(NULL)),
    password(""),
    password_birth(0)
{
//    version = Config::GlobalConfig().WALLET_VERSION;
//    ctime = time(NULL);
//    current_index = 0;
//    password = "";
//    password_birth = 0;
}

bool Wallet::IsEmpty()
{
    return accounts.empty();
}

Account Wallet::NewAccount(const std::string &seed)
{
    auto key_pair = RadarDelegate::Instance().KeyGenerate(seed);
    return Account(Wallet::Instance().password, key_pair.first, key_pair.second);
}

Account& Wallet::AddAccount(Account acc)
{
    for (auto it : accounts) {
        if (it.pub_key == acc.pub_key) {
            return Account::NullAccount();
        }
    }
    accounts.push_back(acc);
    return accounts.back();
}

bool Wallet::RemoveAccount(const std::string &pub_key)
{
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if (it->pub_key == pub_key) {
            if (current_account == it->pub_key) {
                //TODO need switch current account
            }
            accounts.erase(it);
            return true;
        }
    }
    return false;
}

Account& Wallet::GetAccount(const std::string &pub_key)
{
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if (it->pub_key == pub_key) {
            return *it;
        }
    }
    return Account::NullAccount();
}

Account& Wallet::CurrentAccount()
{
    return GetAccount(current_account);
}

Account& Wallet::SwitchAccount(const std::string &pub_key)
{
    current_account = pub_key;
    return CurrentAccount();
}

const std::vector<Account>& Wallet::GetAccounts()
{
    return accounts;
}

bool Wallet::SetAccountBalances(const std::string &account, const std::vector<Balance> &balances)
{
    for (auto &acc : accounts) {
        if (acc.pub_key == account) {
            acc.balances = balances;
            return true;
        }
    }
    return false;
}

bool Wallet::Load(std::string path, bool is_import)
{
    if (path == "") {
        path = wallet_path;
    }
    json j;
    try {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            return false;
        }
        ifs >> j;
    } catch (...) {
        Logger::Error() << "Fail to load wallet - \"" << path << "\"" << std::endl;
        return false;
    }
    if (!CheckWallet(j)) {
        Logger::Error() << "Fail to check wallet record - \"" << path << "\"" << std::endl;
        return false;
    }

    version = j["version"].get<int>();
    ctime = j["ctime"].get<int>();
    mtime = j["mtime"].get<int>();
    magic_word = j["magic_word"].get<std::string>();

    accounts.clear();
    std::for_each(j["accounts"].begin(), j["accounts"].end(), [&](json &acc) {
        accounts.push_back(Account(acc));
    });

    if (j.find("current_account") != j.end() && j["current_account"].is_string()) {
        std::string pub_key = j["current_account"].get<std::string>();
        if (!GetAccount(pub_key).IsNullAccount()) {
            current_account = pub_key;
        }
    } else {
        current_account = accounts.size() > 0 ? accounts[0].pub_key : "";
    }
    if (!is_import) {
        wallet_path = path;
    }
    return true;
}

bool Wallet::Save(std::string path)
{
    if (path == "") {
        path = wallet_path;
    }
    json j;
    j["version"] = version;
    j["ctime"] = ctime;
    j["mtime"] = time(NULL);
    j["accounts"] = json::array();
    j["magic_word"] = magic_word;// Account::Encrypt(password, MAGIC_WORD_ORIGIN);

    std::for_each(accounts.begin(), accounts.end(), [&](Account &acc) {
        j["accounts"].push_back(acc.ToJson());
    });

    if (current_account != "") {
        j["current_account"] = current_account;
    }

    std::ofstream ofs(path);
    ofs << j.dump(2) << std::endl;
    ofs.close();
    return true;
}

bool Wallet::CheckWallet(json &j) {
    if (j.find("version") == j.end()
            || j.find("ctime") == j.end()
            || j.find("mtime") == j.end()
            || j.find("accounts") == j.end()) {

        return false;
    }
//    std::for_each(j["accounts"].begin(), j["accounts"].end(), [](json &acc){
//    });
    //TODO more check needed
    return true;
}

bool Wallet::CheckPassword(const std::string &pass){
    if (magic_word == Account::Encrypt(pass, MAGIC_WORD_ORIGIN)) {
        return true;
    }
    return false;
}

bool Wallet::SetPassword(const std::string &new_pwd, const std::string &old_pwd)
{
    if (old_pwd != "") {
        //TODO need change to support password modify
    }
    password = new_pwd;
    password_birth = time(NULL);
    magic_word = Account::Encrypt(new_pwd, MAGIC_WORD_ORIGIN);
    return true;
}

bool Wallet::HasPassword()
{
    return password != ""   // password not null
            && CheckPassword(password)  // password is right
            && time(NULL) - password_birth < Config::GlobalConfig().PASSWORD_LIFE; // password not expire
}

std::string Wallet::GetPassword()
{
    return password;
}
