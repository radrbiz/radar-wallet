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

#ifndef RADARDELEGATE_H
#define RADARDELEGATE_H

#include <QObject>
#include <string>
#include <utility>
#include "balance.h"
#include <vector>

Q_DECLARE_METATYPE(std::string)

class RadarDelegate : public QObject
{
    Q_OBJECT
public:
    typedef enum {Transfer_Status_Fail, Transfer_Status_Success, Transfer_Status_Running} TransferStatus;
    explicit RadarDelegate(QObject *parent = nullptr);

    static RadarDelegate& Instance();

    std::pair<std::string, std::string> KeyGenerate(const std::string &seed="");
    std::string KeyConvert(const std::string &pri_key);
    bool KeyCheck(const std::string &key);
    std::pair<std::string, std::string> TxSign(const std::string &pri_key, const std::string &tx);

    void FetchAccountBalances(const std::string &account);
    void Transfer(const std::string &sign_key, const std::string &from, const std::string &to, uint64_t amount, const std::string &asset);

signals:
    void DataUpdated();
    void TransferStatusUpdated(const std::string &info, TransferStatus status);

public slots:
};

#endif // RADARDELEGATE_H
