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

#include "radartool.h"
#include <QProcess>
#include <json.hpp>
#include <QDebug>
#include <QCoreApplication>
#include "logger.h"

using json = nlohmann::json;

RadarTool::RadarTool(const QString &p) : tool_path(p)
{
}

RadarTool& RadarTool::Instance()
{
    static RadarTool t(QCoreApplication::applicationDirPath() + "/rda_tool");
    return t;
}

std::pair<std::string, std::string> RadarTool::KeyGenerate(const std::string &seed)
{
    QStringList arguments;
    arguments.append("key_gen");
    if (seed != "") {
        arguments.append(seed.c_str());
    }
    Logger::Info() << "tool_path:" << tool_path.toStdString() << std::endl;
    QProcess process;
    process.start(tool_path, arguments);
    process.waitForStarted();
    process.waitForFinished();
//    if (process.exitCode() != 0) {
//        Logger::Error() << "rda_tool return code error - " << process.exitCode() << std::endl;
//        return std::make_pair<std::string, std::string>("", "");
//    }
    std::string result = process.readAllStandardOutput().toStdString();

    std::string pub_key = "";
    std::string pri_key = "";

    try {
        json j = json::parse(result.c_str());
        if (j.find("status") == j.end() || !j["status"].is_string()
                || j.find("private") == j.end() || !j["private"].is_string()
                || j.find("public") == j.end() || !j["public"].is_string()) {
            Logger::Error() << "invalid rda_tool output" << std::endl;
        } else {
            if (!j["status"].is_string() || j["status"].get<std::string>() != "success") {
                Logger::Error() << "rda_tool return false" << std::endl;
            } else {
                pub_key = j["public"].get<std::string>();
                pri_key = j["private"].get<std::string>();
            }
        }
    } catch (...) {
        Logger::Error() << "parse rda_tool output fail." << std::endl;
    }

    return std::pair<std::string, std::string>(pub_key, pri_key);
}

std::string RadarTool::KeyConvert(const std::string &pri_key)
{
    if (pri_key == "") {
        Logger::Warning() << "null key to convert" << std::endl;
        return "";
    }
    QStringList arguments;
    arguments.append("key_conv");
    arguments.append(pri_key.c_str());
    QProcess process;
    process.start(tool_path, arguments);
    process.waitForStarted();
    process.waitForFinished();
//    if (process.exitCode() != 0) {
//        Logger::Error() << "rda_tool return code error - " << process.exitCode() << std::endl;
//        return std::make_pair<std::string, std::string>("", "");
//    }
    std::string result = process.readAllStandardOutput().toStdString();

//    Logger::Info() << result << std::endl;

    try {
        json j = json::parse(result.c_str());
        if (j.find("status") == j.end() || !j["status"].is_string()
                || j.find("public") == j.end() || !j["public"].is_string()) {
            Logger::Error() << "invalid rda_tool output" << std::endl;
        } else {
            if (!j["status"].is_string() || j["status"].get<std::string>() != "success") {
                Logger::Error() << "rda_tool return false" << std::endl;
            } else {
                return j["public"].get<std::string>();
            }
        }
    } catch (...) {
        Logger::Error() << "parse rda_tool output fail." << std::endl;
    }

    return "";
}

bool RadarTool::KeyCheck(const std::string &key)
{
    if (key == "") {
        Logger::Warning() << "null key to check" << std::endl;
        return false;
    }
    QStringList arguments;
    arguments.append("key_chk");
    arguments.append(key.c_str());
    QProcess process;
    process.start(tool_path, arguments);
    process.waitForStarted();
    process.waitForFinished();
//    if (process.exitCode() != 0) {
//        Logger::Error() << "rda_tool return code error - " << process.exitCode() << std::endl;
//        return std::make_pair<std::string, std::string>("", "");
//    }
    std::string result = process.readAllStandardOutput().toStdString();

//    Logger::Info() << result << std::endl;

    try {
        json j = json::parse(result.c_str());
        if (j.find("status") == j.end() || !j["status"].is_string()
                || j.find("result") == j.end() || !j["result"].is_string()) {
            Logger::Error() << "invalid rda_tool output" << std::endl;
        } else {
            if (!j["status"].is_string() || j["status"].get<std::string>() != "success") {
                Logger::Error() << "rda_tool return false" << std::endl;
            } else {
                if (j["result"].get<std::string>() == "true") {
                    return true;
                } else {
                    Logger::Info() << "key - " << key << " check fail." << std::endl;
                    return false;
                }
            }
        }
    } catch (...) {
        Logger::Error() << "parse rda_tool output fail." << std::endl;
    }

    return true;
}

std::pair<std::string, std::string> RadarTool::TxSign(const std::string &pri_key, const std::string &tx)
{
    // TODO check tx first
    if (tx == "") {
        Logger::Warning() << "null tx to sign" << std::endl;
        return std::pair<std::string, std::string>("", "");
    }
    QStringList arguments;
    arguments.append("tx_sign");
    arguments.append(pri_key.c_str());
    arguments.append(tx.c_str());
    QProcess process;
    process.start(tool_path, arguments);
    process.waitForStarted();
    process.waitForFinished();
//    if (process.exitCode() != 0) {
//        Logger::Error() << "rda_tool return code error - " << process.exitCode() << std::endl;
//        return std::make_pair<std::string, std::string>("", "");
//    }
    std::string result = process.readAllStandardOutput().toStdString();

    Logger::Error() << process.readAllStandardError().toStdString() << std::endl;
    Logger::Info() << result << std::endl;

    std::string tx_blob = "";
    std::string hash = "";

    try {
        json j = json::parse(result.c_str());
        if (j.find("status") == j.end() || !j["status"].is_string()
                || j.find("tx_blob") == j.end() || !j["tx_blob"].is_string()
                || j.find("hash") == j.end() || !j["hash"].is_string()) {
            Logger::Error() << "invalid rda_tool output" << std::endl;
        } else {
            if (!j["status"].is_string() || j["status"].get<std::string>() != "success") {
                Logger::Error() << "rda_tool return false" << std::endl;
            } else {
                tx_blob = j["tx_blob"].get<std::string>();
                hash = j["hash"].get<std::string>();
            }
        }
    } catch (...) {
        Logger::Error() << "parse rda_tool output fail." << std::endl;
    }

    return std::pair<std::string, std::string>(tx_blob, hash);
}
