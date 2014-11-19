/*
 * Copyright (C) 2014  Steffen Nüssle
 * tq - Twitch Query
 *
 * This file is part of tq.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <iomanip>
#include <iostream>

#include "response-handler.hpp"
#include "query-builder.hpp"

response_handler::response_handler(bool v, bool u)
    : _reader(),
      _v(v),
      _u(u)
{
}

response_handler::~response_handler()
{

}


void response_handler::handle_response(const std::string& str)
{
    Json::Value root;
    
    auto ok = _reader.parse(str, root, false);
    
    if (!ok) {
        std::cerr << "Unable to parse response." << std::endl;
        return;
    }

    if (root.isMember("error")) {
        auto msg  = root["error"].asString();
        auto code = root["status"].asInt();
        
        std::cerr << "Error " << code << " - " << msg << std::endl;
        return;
    }
    
    auto self = root["_links"]["self"].asString();

    if (self.find("channels/") != std::string::npos)
        handle_channels(root);
    else if (self.find("streams/featured") != std::string::npos)
        handle_featured(root);
    else if (self.find("search/channels") != std::string::npos)
        handle_search_channels(root);
    else if (self.find("search/games") != std::string::npos)
        handle_search_games(root);
    else if (self.find("search/streams") != std::string::npos)
        handle_search_streams(root);
    else if (self.find("streams/") != std::string::npos)
        handle_streams(root);
    else if (self.find("games/top") != std::string::npos)
        handle_top(root);
    else
        std::cerr << "Unable to retrieve information from response:\n" 
                  << root.toStyledString() << std::endl;
}

void response_handler::handle_top(const Json::Value &root)
{
    auto list = root["top"];
    
    for (const auto &ele : list) {
        auto game    = ele["game"];
        auto viewers = ele["viewers"].asInt();
        
        auto name    = game["name"].asString();
        
        std::cout << std::setw(10) << std::right << viewers << "  " << name 
                  << std::endl;
    }
}

void response_handler::handle_channels(const Json::Value &root)
{
    
}

void response_handler::handle_featured(const Json::Value& root)
{
    auto list = root["featured"];
    
    for (const auto &ele : list) {
        auto stream = ele["stream"];
        
        auto link = stream["_links"]["self"].asString();
        auto name = stream["channel"]["display_name"].asString();
        auto game = stream["game"].asString();
        
        if (name.length() > 20)
            name.resize(20);
        
        if (game.length() > 30)
            game.resize(30);
        
        std::cout << "  " 
                  << std::setw(20) << std::left << name << " "
                  << std::setw(30) << std::left << game << " "
                  << link << std::endl;
    }
}

void response_handler::handle_search_channels(const Json::Value& root)
{

}

void response_handler::handle_search_games(const Json::Value& root)
{

}

void response_handler::handle_search_streams(const Json::Value& root)
{
    auto streams = root["streams"];
    
    for (const auto &stream : streams) {
        auto viewers = stream["viewers"].asInt();
        
        if (viewers >= 20) {
            auto chan = stream["channel"];
            auto name = chan["display_name"].asString();
            auto url  = chan["url"].asString();
            
            std::cout << "  " 
                      << std::setw(20) << std::left << name  
                      << std::setw(10) << std::left << viewers 
                      << url << std::endl;
        }
    }
}

void response_handler::handle_streams(const Json::Value& root)
{

}

    

/*
void tq::response_handler::game_response(const std::string& response)
{
    Json::Value root;
    
    auto ok = _reader.parse(response, root, false);
    
    if (!ok) {
        std::cerr << "Unable to parse game response." << std::endl;
        return;
    }
    

    

}

void tq::response_handler::streams_response(const std::string& response)
{

}

void tq::response_handler::search_streams_response(const std::string& response)
{
    Json::Value root;
    
    auto ok = _reader.parse(response, root, false);
    
    if (!ok)
        throw std::runtime_error("Json::Reader::parse() failed.");
    

}
*/

