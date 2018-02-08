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

#ifndef WALLET_H
#define WALLET_H

#include "account.h"
#include <vector>
#include <json.hpp>

class Wallet
{
public:
    static Wallet& Instance();
    static Account NewAccount(const std::string &seed = "");
    Account& AddAccount(Account account);
    bool RemoveAccount(const std::string &pub_key);

    Account& GetAccount(const std::string &pub_key);
    Account& CurrentAccount();
    Account& SwitchAccount(const std::string &pub_key);
    const std::vector<Account>& GetAccounts();

    bool SetAccountBalances(const std::string &account, const std::vector<Balance> &balances);

    bool Load(std::string path, bool is_import = false);
    bool Save(std::string path = "");

    bool SetPassword(const std::string &new_pwd, const std::string &old_pwd="");
    bool CheckPassword(const std::string &password);
    bool HasPassword();
    std::string GetPassword();

    bool IsEmpty();

    std::vector<Account> accounts;
    std::string current_account;
    int version;
    time_t ctime;
    time_t mtime;

    std::string magic_word; //magic_word is used for password checking
    std::string password;   //password won't save to wallet file
    uint64_t password_birth;

    std::string wallet_path;

private:
    Wallet();
    bool CheckWallet(nlohmann::json &j);
};

#endif // WALLET_H
