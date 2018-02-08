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

#include "addressbook.h"
#include <json.hpp>
#include <fstream>
#include "logger.h"
#include "config.h"

using json = nlohmann::json;

AddressBook::AddressBook()
{

}

AddressBook& AddressBook::Instance()
{
    static AddressBook ab;
    return ab;
}

const std::vector<std::string>& AddressBook::AddressList() const
{
    return address_list;
}

bool AddressBook::Add(const std::string &address)
{
    for (auto it : address_list) {
        if (it == address) {
            return false;
        }
    }
    address_list.push_back(address);
    Save(save_path);
    return true;
}

bool AddressBook::Remove(const std::string &address)
{
    for (auto it = address_list.begin(); it != address_list.end(); ++it) {
        if (*it == address) {
            address_list.erase(it);
            Save(save_path);
            return true;
        }
    }
    return false;
}

bool AddressBook::RemoveAll()
{
    address_list.clear();
    Save(save_path);
    return true;
}

bool AddressBook::Load(const std::string &path)
{
    json j;
    // try read and parse from json list file
    try {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            Logger::Error() << "Fail to find address book file - \"" << path << "\"" << std::endl;
            return false;
        }
        ifs >> j;
        ifs.close();
    } catch (...) {
        Logger::Error() << "Fail to parse address book file - \"" << path << "\"" << std::endl;
        return false;
    }
    Logger::Debug() << j.dump(2);
    try {
        if (j.find("data") == j.end() || !j["data"].is_array()) {
            Logger::Error() << "Fail to get data field - \"" << path << "\"" << std::endl;
            return false;
        }
        address_list.clear();
        for (auto it = j["data"].begin(); it != j["data"].end(); ++it) {
            if (it->find("address") == it->end() || !(*it)["address"].is_string()) {
                continue;
            }
            address_list.push_back((*it)["address"].get<std::string>());
        }
    } catch (...) {
        Logger::Error() << "Fail to extract address book fields" << std::endl;
        return false;
    }
    save_path = path;
    return true;
}

bool AddressBook::Save(const std::string &path)
{
    json j;
    j["version"] = Config::GlobalConfig().APP_VERSION;
    j["data"] = json::array();
    for (auto it : address_list) {
        json r = json::object();
        r["address"] = it;
        j["data"].push_back(r);
    }
    // save json to address list file
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        Logger::Info() << "Fail open address list file - \"" << path << "\"" << std::endl;
        return false;
    }
    ofs << j.dump(2) << std::endl;
    ofs.close();
    return true;
}
