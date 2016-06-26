// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "drivechainclient.h"

#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <sstream>

#include "utilstrencodings.h" // for EncodeBase64

using boost::asio::ip::tcp;

DrivechainClient::DrivechainClient()
{
}

DrivechainClient::~DrivechainClient()
{
}

bool DrivechainClient::sendDrivechainWT(uint256 txid)
{
    // JSON for sending the WT^ to mainchain via HTTP-RPC
    std::string json;
    json.append("{\"jsonrpc\": \"1.0\", \"id\":\"1\", ");
    json.append("\"method\": \"getnewaddress\", \"params\": ");
    json.append("[\"\"] }");

    return sendRequestToMainchain(json);
}

bool DrivechainClient::sendRequestToMainchain(std::string json)
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

        if (error)
          throw boost::system::system_error(error);

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

        if (version.empty() || code != 200) {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!Error\n"; // TODO
            return false;
        }

        boost::asio::read_until(socket, res, "\n");

        // Skip the rest of the header
        std::string header;
        while (std::getline(is, header) && header != "\r") { }

        // Read the actual json response
        std::stringstream ss;
        if (res.size())
            ss << &res;

        std::cout << "ss: \n" << ss.str() << std::endl;
        std::cout << "----\n";

        // Parse json
        boost::property_tree::ptree ptree;
        boost::property_tree::json_parser::read_json(ss, ptree);

        std::cout << "Addr generated: " << ptree.get<std::string>("result") << std::endl;

    } catch (std::exception &exception) {
        std::cout << "DrivechainClient exception: " << exception.what() << std::endl;
        return false;
    }
    return true;
}
