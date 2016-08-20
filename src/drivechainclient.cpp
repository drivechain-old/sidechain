// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "drivechainclient.h"

#include "utilstrencodings.h" // For EncodeBase64
#include "util.h" // For logPrintf

#include <iostream>
#include <sstream>
#include <string>

#include <boost/asio.hpp>
#include <boost/foreach.hpp>

using boost::asio::ip::tcp;

// Max buffer size for drivechain requests
const int MAX_DC_SIZE = 1024; // TODO update

DrivechainClient::DrivechainClient()
{

}

bool DrivechainClient::sendDrivechainWT(uint256 txid)
{
    // JSON for sending the WT^ to mainchain via HTTP-RPC
    std::string json;
    json.append("{\"jsonrpc\": \"1.0\", \"id\":\"drivechainclient\", ");
    json.append("\"method\": \"receivesidechainwt\", \"params\": ");
    json.append("[\"");
    json.append(txid.GetHex());
    json.append("\"] }");

    // TODO Read result, display to user
    boost::property_tree::ptree ptree;
    return sendRequestToMainchain(json, ptree);
}

std::vector<drivechainIncoming> DrivechainClient::getDeposits(uint256 sidechainid, uint32_t height)
{
    // List of deposits in drivechain format for DB
    std::vector<drivechainIncoming> incoming;

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
        drivechainIncoming deposit;
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, value.second.get_child("")) {
            // Looping through this deposit's members
            std::string data = v.second.data();
            if (!data.length())
                continue;

            std::cout << "getDeposits v: " << data << std::endl; // TODO

            // TODO this better, use json keypairs in server response

            // TODO Actually, just get the deposit hash, and add a
            // function to the drivechain client that can deserialize
            // the deposit, take those values and create a drivechain
            // incoming object.
            if (data.at(0) == '1' && data.length() == 40) {
                // KeyID
                deposit.keyID.SetHex(data);
            }
            else
            if (data.length() == 64) {
                // Get the sidechain id
                if (uint256S(data) == SIDECHAIN_ID)
                    deposit.sidechainid.SetHex(data);
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

        // TODO username:password
        // HTTP request (package the json for sending)
        boost::asio::streambuf output;
        std::ostream os(&output);
        os << "POST / HTTP/1.1\n";
        os << "Host: 127.0.0.1\n";
        os << "Content-Type: application/json\n";
        os << "Authorization: Basic " << EncodeBase64("patrick:drivechain") << "\n";
        os << "Connection: close\n";
        os << "Content-Length: " << json.size() << "\n\n";
        os << json;

        // Send the request
        boost::asio::write(socket, output);
        boost::asio::streambuf res;

        // TODO use boost's read function instead

        // Read the reponse
        boost::array<char, MAX_DC_SIZE> buf;
        for (;;)
        {
            // Read until end of file (socket closed)
            boost::system::error_code e;
            socket.read_some(boost::asio::buffer(buf), e);

            if (e == boost::asio::error::eof)
                break; // socket closed
            else if (e)
                throw boost::system::system_error(e);
        }

        std::stringstream ss;
        ss << buf.data();

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
        std::cout << "JSON: \n" << JSON << std::endl; // TODO

        std::stringstream jss;
        jss << JSON;

        // Parse json response;
        // TODO consider using univalue read_json instead of boost
        boost::property_tree::json_parser::read_json(jss, ptree);
    } catch (std::exception &exception) {
        std::cout << "DV: except: " << exception.what() <<  "\n"; // TODO
        LogPrintf("ERROR Drivechain client (sendRequestToMainchain): %s\n", exception.what());
        return false;
    }
    return true;
}
