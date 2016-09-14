// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DRIVECHAINCLIENT_H
#define DRIVECHAINCLIENT_H

#include "primitives/drivechain.h"
#include "uint256.h"

#include <boost/property_tree/json_parser.hpp>

#include <vector>

class DrivechainClient
{
public:
    DrivechainClient();

    bool sendDrivechainWT(uint256 wtjid, string hex);

    std::vector<drivechainDeposit> getDeposits(uint256 sidechainid, uint32_t height);

private:
    bool sendRequestToMainchain(const std::string json, boost::property_tree::ptree &ptree);
};

#endif // DRIVECHAINCLIENT_H
