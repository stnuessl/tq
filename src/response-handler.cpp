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

tq::response_handler::response_handler()
    : _reader()
{
    
}

tq::response_handler::~response_handler()
{

}


void tq::response_handler::channel_response(const std::string& response)
{

}

void tq::response_handler::game_response(const std::string& response)
{
    Json::Value root;
    
    auto ok = _reader.parse(response, root, false);
    
    if (!ok) {
        std::cerr << "Unable to parse game response." << std::endl;
        return;
    }
    
    auto list = root["top"];
    
    for (const auto &ele : list) {
        auto game    = ele["game"];
        auto viewers = ele["viewers"].asInt();
        
        auto name    = game["name"].asString();
        
        std::cout << std::setw(10) << std::right << viewers 
                  << "  " << name << std::endl;
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
    
    auto streams = root["streams"];
    
    for (const auto &stream : streams) {
        auto viewers = stream["viewers"].asInt();
        
        if (viewers >= 20) {
            auto chan = stream["channel"];
            auto name = chan["display_name"].asString();
            auto url  = chan["url"].asString();
            
            std::cout << std::setw(20) << std::left << name 
                      << std::setw(10) << std::left << viewers 
                      << url << std::endl;
        }
    }
}

