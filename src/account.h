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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <json.hpp>
#include <vector>
#include "balance.h"
#include <map>

class Account
{
public:
    Account();
    Account(nlohmann::json &);
    Account(const std::string &password, const std::string &pub_key, const std::string &pri_key, const std::string &tag="");

    nlohmann::json ToJson();

    std::string GetPrivateKey(const std::string &password);

    std::string BalancesString() const;

    bool IsNullAccount();
    static Account& NullAccount();

    static std::string Encrypt(const std::string &key, const std::string &content);
    static std::string Decrypt(const std::string &key, const std::string &content);


    std::string BalancesString(const std::string &asset);

    int version;
    std::string pub_key;
    std::string pri_key_en;

    time_t ctime;
    time_t mtime;
    std::string tag;

    std::vector<Balance> balances;
};

#endif // ACCOUNT_H
