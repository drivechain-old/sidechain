// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "drivechainclient.h"

#include "core_io.h"
#include "utilstrencodings.h"
#include "util.h"

#include <iostream>
#include <sstream>
#include <string>

#include <boost/asio.hpp>
#include <boost/foreach.hpp>

using boost::asio::ip::tcp;

DrivechainClient::DrivechainClient()
{

}

bool DrivechainClient::sendDrivechainWT(uint256 wtjid, std::string hex)
{
    // JSON for sending the WT^ to mainchain via HTTP-RPC
    std::string json;
    json.append("{\"jsonrpc\": \"1.0\", \"id\":\"drivechainclient\", ");
    json.append("\"method\": \"receivesidechainwt\", \"params\": ");
    json.append("[\"");
    json.append(wtjid.GetHex());
    json.append("\",\"");
    json.append(hex);
    json.append("\"] }");

    // TODO Read result, display to user
    boost::property_tree::ptree ptree;
    return sendRequestToMainchain(json, ptree);
}

std::vector<drivechainDeposit> DrivechainClient::getDeposits(uint256 sidechainid, uint32_t height)
{
    // List of deposits in drivechain format for DB
    std::vector<drivechainDeposit> incoming;

    // JSON for requesting drivechain deposits via mainchain HTTP-RPC
    std::string json;
    json.append("{\"jsonrpc\": \"1.0\", \"id\":\"drivechainclient\", ");
    json.append("\"method\": \"requestdrivechaindeposits\", \"params\": ");
    json.append("[\"");
    json.append(sidechainid.GetHex());
    json.append("\",");
    json.append(itostr(height));
    json.append("] }");

    // Try to request deposits from mainchain
    boost::property_tree::ptree ptree;
    if (!sendRequestToMainchain(json, ptree))
        return incoming; // TODO display error

    // Process deposits
    BOOST_FOREACH(boost::property_tree::ptree::value_type &value, ptree.get_child("result")) {
        // Looping through list of deposits
        drivechainDeposit deposit;
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, value.second.get_child("")) {
            // Looping through this deposit's members
            if (v.first == "sidechainid") {
                std::string data = v.second.data();
                if (!data.length())
                    continue;

                // Sidechain ID
                if (uint256S(data) == SIDECHAIN_ID)
                    deposit.sidechainid.SetHex(data);
            }
            else
            if (v.first == "dt") {
                std::string data = v.second.data();
                if (!data.length())
                    continue;

                // Deposit Transaction
                CTransaction dtx;
                if (DecodeHexTx(dtx, data))
                    deposit.deposit = dtx;
            }
            else
            if (v.first == "keyID") {
                std::string data = v.second.data();
                if (!data.length())
                    continue;

                // KeyID
                deposit.keyID.SetHex(data);
            }
        }
        // Add this deposit to the list
        incoming.push_back(deposit);
    }

    // return valid deposits in drivechain format
    return incoming;
}

bool DrivechainClient::sendRequestToMainchain(const string json, boost::property_tree::ptree &ptree)
{
    try {
        // Setup BOOST ASIO for a synchronus call to mainchain
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query("127.0.0.1", "18332");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        tcp::socket socket(io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;

        // Try to connect
        while (error && endpoint_iterator != end)
        {
          socket.close();
          socket.connect(*endpoint_iterator++, error);
        }

        if (error) throw boost::system::system_error(error);

        // HTTP request (package the json for sending)
        boost::asio::streambuf output;
        std::ostream os(&output);
        os << "POST / HTTP/1.1\n";
        os << "Host: 127.0.0.1\n";
        os << "Content-Type: application/json\n";
        std::string auth = mapArgs["-rpcuser"] + ":" + mapArgs["-rpcpassword"];
        os << "Authorization: Basic " << EncodeBase64(auth) << std::endl;
        os << "Connection: close\n";
        os << "Content-Length: " << json.size() << "\n\n";
        os << json;

        // Send the request
        boost::asio::write(socket, output);

        // Read the reponse
        std::string data;
        for (;;)
        {
            boost::array<char, 4096> buf;

            // Read until end of file (socket closed)
            boost::system::error_code e;
            size_t sz = socket.read_some(boost::asio::buffer(buf), e);

            data.insert(data.size(), buf.data(), sz);

            if (e == boost::asio::error::eof)
                break; // socket closed
            else if (e)
                throw boost::system::system_error(e);
        }

        std::stringstream ss;
        ss << data;

        // Get response code
        ss.ignore(numeric_limits<streamsize>::max(), ' ');
        int code;
        ss >> code;

        // Check response code
        if (code != 200) return false;

        // Skip the rest of the header
        for (size_t i = 0; i < 5; i++)
            ss.ignore(numeric_limits<streamsize>::max(), '\r');

        std::string JSON;
        ss >> JSON;

        std::stringstream jss;
        jss << JSON;

        // Parse json response;
        // TODO consider using univalue read_json instead of boost
        boost::property_tree::json_parser::read_json(jss, ptree);
    } catch (std::exception &exception) {
        LogPrintf("ERROR Drivechain client (sendRequestToMainchain): %s\n", exception.what());
        return false;
    }
    return true;
}
