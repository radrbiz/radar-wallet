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

#include "account.h"
#include <string>
#include "crypto.h"
#include <time.h>
#include "config.h"

using json = nlohmann::json;

Account::Account() : pub_key("")
{
}

Account::Account(json &j)
{
    version = j["version"].get<int>();
    pub_key = j["public"].get<std::string>();
    pri_key_en = j["private"].get<std::string>();
    tag = j["tag"].get<std::string>();
    ctime = j["ctime"].get<int>();
    mtime = j["mtime"].get<int>();
}

Account::Account(const std::string &password, const std::string &pub_key, const std::string &pri_key, const std::string &tag)
{
    time_t now = time(NULL);
    this->pub_key = pub_key;
    this->pri_key_en = Encrypt(password, pri_key);
    this->tag = tag;
    this->ctime = now;
    this->mtime = now;
    this->version = Config::GlobalConfig().ACCOUNT_VERSION;
}

Account& Account::NullAccount()
{
    static Account acc;
    return acc;
}

bool Account::IsNullAccount()
{
    return pub_key == "";
}

std::string Account::GetPrivateKey(const std::string &password)
{
    return Decrypt(password, pri_key_en);
}

std::string Account::BalancesString() const
{
    std::string result = "";
    for (auto &blc : balances) {
        if (result.length() > 0) {
            result += " / ";
        }
        result += blc.ToString();
    }
    return result;
}

std::string Account::Encrypt(const std::string &key, const std::string &content)
{
    uint8_t len = (uint8_t)content.size();
    std::string origin = std::string(1, len) + content;
    std::string des_en = Crypto::DES_Encrypt(key, origin);
    std::string base64_en = Crypto::Base64_Encrypt(des_en);
    return base64_en;
}

std::string Account::Decrypt(const std::string &key, const std::string &content)
{
    std::string base64_de = Crypto::Base64_Decrypt(content);
    std::string des_de = Crypto::DES_Decrypt(key, base64_de);
    if (des_de.size() <= 0) {
        return "";
    }
    uint8_t len = des_de[0];
    if (len <= 0 || len > des_de.size()) {
        return "";
    }
    std::string origin = des_de.substr(1, len);
    return origin;
}

std::string Account::BalancesString(const std::string &asset)
{
    for (auto blc : balances) {
        if (asset == blc.token) {
            return Balance::ToString(blc.amount, blc.token);
        }
    }
    return "0.000000";
}

json Account::ToJson()
{
    json acc;
    acc["version"] = version;
    acc["public"] = pub_key;
    acc["private"] = pri_key_en;
    acc["tag"] = tag;
    acc["ctime"] = ctime;
    acc["mtime"] = mtime;
    return acc;
}
