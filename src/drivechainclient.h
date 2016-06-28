// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DRIVECHAINCLIENT_H
#define DRIVECHAINCLIENT_H

#include "uint256.h"

class DrivechainClient
{
public:
    DrivechainClient();

    bool sendDrivechainWT(uint256 txid);

private:
    bool sendRequestToMainchain(std::string json);
};

#endif // DRIVECHAINCLIENT_H
