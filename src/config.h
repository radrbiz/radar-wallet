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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

class Config
{
public:

    static Config& GlobalConfig();

    bool Load(const std::string &path);
    bool Save(const std::string &path);

    int GetDecimal(const std::string &token);

    int APP_VERSION;
    int WALLET_VERSION;
    int ACCOUNT_VERSION;

    std::string SERVER_SCHEME;
    std::string SERVER_HOST;
    uint16_t SERVER_PORT;


    int PASSWORD_LIFE;
    int PASSWORD_LEN_MIN;
    int PASSWORD_LEN_MAX;

    std::string CONFIG_FILE_NAME;
    std::string DATA_FILE_NAME;
    std::string ADDRESS_BOOK_FILE_NAME;

    static std::string VersionString(int version);

private:
    std::map<std::string, int> assets_decimal;

private:
    Config();
};

#endif // CONFIG_H
