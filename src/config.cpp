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

#include "config.h"
#include <json.hpp>
#include <fstream>
#include "logger.h"
#include <algorithm>

using json = nlohmann::json;

Config::Config()
{
    APP_VERSION = 1000;
    WALLET_VERSION = 1000;
    ACCOUNT_VERSION = 1000;

    CONFIG_FILE_NAME = "config.json";
    DATA_FILE_NAME = "wallet.json";
    ADDRESS_BOOK_FILE_NAME = "address_book.json";

    SERVER_SCHEME = "http";
    SERVER_HOST = "s1.radarlab.org";
    SERVER_PORT = 5005;

    PASSWORD_LIFE = 300;
    PASSWORD_LEN_MIN = 6;
    PASSWORD_LEN_MAX = 32;

    assets_decimal = {
        {"VBC", 6},
        {"VRP", 6}
    };
}

Config& Config::GlobalConfig()
{
    // singleton
    static Config c;
    return c;
}

std::string Config::VersionString(int version)
{
    int big = version % 10;
    version /= 10;
    int mid = version % 10;
    int small = version / 10;
    return std::to_string(big) + "." + std::to_string(mid) + "." + std::to_string(small);
}

int Config::GetDecimal(const std::string &token)
{
    auto it = assets_decimal.find(token);
    if (it == assets_decimal.end()) {
        return -1;
    }
    return it->second;
}

bool Config::Load(const std::string &path)
{
    // TODO
    return true;

    json j;
    // try read and parse from json config file
    try {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            Logger::Error() << "Fail to find config file - \"" << path << "\"" << std::endl;
            return false;
        }
        ifs >> j;
        ifs.close();
    } catch (...) {
        Logger::Error() << "Fail to parse config file - \"" << path << "\"" << std::endl;
        return false;
    }
    try {
        // read server settings
        SERVER_SCHEME = j["server"]["scheme"].get<std::string>();
        SERVER_HOST = j["server"]["host"].get<std::string>();
        SERVER_PORT = j["server"]["port"].get<int>();
        // read password settings
        PASSWORD_LIFE = j["password"]["life"].get<int>();
        PASSWORD_LEN_MAX = j["password"]["len_max"].get<int>();
        PASSWORD_LEN_MIN = j["password"]["len_min"].get<int>();
        // read asset settings
        assets_decimal.clear();
        for (auto it = j["asset"].begin(); it != j["asset"].end(); ++it) {
            assets_decimal.insert(std::make_pair<std::string, int>((*it)["asset"], (*it)["decimal"]));
        }
    } catch (...) {
        Logger::Error() << "Fail to extract config fields" << std::endl;
        return false;
    }
    return true;
}

bool Config::Save(const std::string &path)
{
    json j;
    // version info, just save, won't read
    j["version"] = APP_VERSION;
    j["wallet_version"] = WALLET_VERSION;
    j["account_version"] = ACCOUNT_VERSION;
    // server settings
    j["server"] = json::object();
    j["server"]["scheme"] = SERVER_SCHEME;
    j["server"]["host"] = SERVER_HOST;
    j["server"]["port"] = SERVER_PORT;
    // password settings
    j["password"]["life"] = PASSWORD_LIFE;
    j["password"]["len_min"] = PASSWORD_LEN_MIN;
    j["password"]["len_max"] = PASSWORD_LEN_MAX;
    // asset settings
    j["asset"] = json::array();
    for (auto &it : assets_decimal) {
        json r;
        r["asset"] = it.first;
        r["decimal"] = it.second;
        j["asset"].push_back(r);
    }

    // save json to config file
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        Logger::Info() << "Fail open config file - \"" << path << "\"" << std::endl;
        return false;
    }
    ofs << j.dump(2) << std::endl;
    ofs.close();

    return true;
}
