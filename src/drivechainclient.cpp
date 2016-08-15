// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "drivechainclient.h"

#include "utilstrencodings.h" // For EncodeBase64

#include <iostream>
#include <sstream>
#include <string>

#include <boost/asio.hpp>
#include <boost/foreach.hpp>

using boost::asio::ip::tcp;

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

std::vector<drivechainIncoming> DrivechainClient::getDeposits(uint32_t height)
{
    std::vector<drivechainIncoming> incoming;

    // JSON for requesting drivechain deposits via mainchain HTTP-RPC
    std::string json;
    json.append("{\"jsonrpc\": \"1.0\", \"id\":\"drivechainclient\", ");
    json.append("\"method\": \"requestdrivechaindeposits\", \"params\": ");
    json.append("[\"");
    json.append(itostr(height));
    json.append("\"] }");

    // Try to request deposits from mainchain
    boost::property_tree::ptree ptree;
    if (!sendRequestToMainchain(json, ptree)) {
        return incoming; // TODO display error
    }

    // Process deposits
    BOOST_FOREACH(boost::property_tree::ptree::value_type &value, ptree.get_child("")) {
        std::string a = value.second.data();
        std::cout << "[***]deposit a: " << a << std::endl;
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
        os << "Content-Length: " << json.size() << "\n\n";
        os << json;

        // Send the request
        boost::asio::write(socket, output);
        boost::asio::streambuf res;
        boost::asio::read_until(socket, res, "\n");

        // Read the request header info
        std::string version;
        unsigned int code;
        std::string message;
        std::istream is(&res);
        is >> version;
        is >> code;
        std::getline(is, message);

        if (version.empty() || code != 200)
            return false;

        boost::asio::read_until(socket, res, "\n");

        // Skip the rest of the header
        std::string header;
        while (std::getline(is, header) && header != "\r") { }

        // Read json response
        std::stringstream ss;
        if (res.size())
            ss << &res;

        // Parse json response;
        boost::property_tree::json_parser::read_json(ss, ptree);
    } catch (std::exception &exception) {
        // TODO display error
        return false;
    }
    return true;
}
