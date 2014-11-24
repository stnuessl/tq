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
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

#include <json/json.h>

#include "response-handler.hpp"
#include "query-builder.hpp"

response_handler::response_handler(bool verbose)
    : _table ({
          { query_builder::TYPE_CHANNELS,
              &response_handler::handle_channels },
          { query_builder::TYPE_FEATURED,
              &response_handler::handle_featured },
          { query_builder::TYPE_SEARCH_CHANNELS,
              &response_handler::handle_search_channels },
          { query_builder::TYPE_SEARCH_GAMES,
              &response_handler::handle_search_games },
          { query_builder::TYPE_SEARCH_STREAMS,
              &response_handler::handle_search_streams },
          { query_builder::TYPE_STREAMS,
              &response_handler::handle_streams },
          { query_builder::TYPE_TOP,
              &response_handler::handle_top }
      }),
      _reader(),
      _max_name_str_len(20),
      _max_game_str_len(40),
      _verbose(verbose)
{
}

response_handler::~response_handler()
{

}


void response_handler::handle_response(const std::string& str, 
                                       query_builder::type type)
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
    
    try {
        auto f = _table[type];
        
        (this->*f)(root);
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void response_handler::handle_top(const Json::Value &root)
{
    for (auto &x : root["top"])
        print_top(x);
}

void response_handler::handle_channels(const Json::Value &root)
{
    print_channel_full(root);
}

void response_handler::handle_featured(const Json::Value& root)
{
    auto list = root["featured"];

    if (_verbose) {
        for (auto &x : list) {
            auto stream = x["stream"];
            
            print_channel_full(stream);
        }
    } else {
        for (auto &x : list) {
            auto stream = x["stream"];

            print_stream_short(stream);
        }
    }
}

void response_handler::handle_search_channels(const Json::Value& root)
{
    auto channels = root["channels"];
    
    if (_verbose) {
        for (auto &chan : channels)  
            print_channel_full(chan);        
    } else {
        for (auto &chan : channels)
            print_channel_short(chan);
    }   
}

void response_handler::handle_search_games(const Json::Value& root)
{
    auto games = root["games"];
    
    for (auto &x : games) {
        auto name = x["name"].asString();
        auto pop  = x["popularity"].asInt();
        
        std::cout << "  " << std::setw(11) << std::right << pop
                  << "  " << name 
                  << std::endl;
    }
}

void response_handler::handle_search_streams(const Json::Value& root)
{
    auto streams = root["streams"];
    
    if (_verbose) {
        for (auto &s : streams)
            print_stream_full(s);
    } else {
        for (auto &s : streams)
            print_stream_short(s);
    }
}

void response_handler::handle_streams(const Json::Value& root)
{
    auto stream = root["stream"];
    
    if (stream.isNull()) {
        auto self = root["_links"]["self"].asCString();
        auto name = std::strrchr(self, '/') + 1;
        
        std::cout << "  Stream [ " << name  << " ] is offline." << std::endl;
    } else {
        print_stream_full(stream);
    }
}

void response_handler::print_channel_full(const Json::Value &channel)
{
    const auto name     = channel["name"].asString();
    const auto status   = channel["status"].asString();
    const auto url      = channel["url"].asString();
    const auto id       = channel["_id"].asInt();
    const auto game     = channel["game"].asString();
    const auto delay    = channel["delay"].asInt();
    const auto mature   = (channel["mature"].asBool()) ? "yes" : "no";
    const auto language = channel["language"].asString();
    
    std::cout << "  Channel [ " << name << " ]:"  << "\n"
              << "      Status   : " << status      << "\n"
              << "      ID       : " << id          << "\n"
              << "      Url      : " << url         << "\n"
              << "      Game     : " << game        << "\n"
              << "      Delay    : " << delay       << " s\n"
              << "      Mature   : " << mature      << "\n"
              << "      Language : " << language    << std::endl;    
}

void response_handler::print_channel_short(const Json::Value &channel)
{
    const auto name = channel["name"].asString();
    const auto game = channel["game"].asString();
    const auto url  = channel["url"].asString();
    
    std::cout << "  " << std::setw(_max_name_str_len) << std::left << name
              << "  " << std::setw(_max_game_str_len) << std::left << game
              << "  " << url << std::endl;
}


void response_handler::print_stream_full(const Json::Value &stream)
{
    const auto chan = stream["channel"];
    
    const auto name    = chan["name"].asString();
    const auto url     = chan["url"].asString();
    const auto viewers = stream["viewers"].asInt();
    const auto id      = stream["_id"].asUInt64();
    const auto game    = stream["game"].asString();
    
    std::cout << "  Stream [ " << name << " ]:" << "\n"
              << "      Url     : " << url        << "\n"
              << "      Viewers : " << viewers    << "\n"
              << "      Id      : " << id         << "\n"
              << "      Game    : " << game       << std::endl;
}

void response_handler::print_stream_short(const Json::Value &stream)
{
    const auto viewers = stream["viewers"].asInt();
    const auto game    = stream["game"].asString();
    const auto url     = stream["channel"]["url"].asString();
    
    std::cout << "  " << std::setw(11)                << std::right << viewers
              << "  " << std::setw(_max_game_str_len) << std::left << game
              << "  " << url << std::endl;
}

void response_handler::print_top(const Json::Value &top)
{
    const auto viewers  = top["viewers"].asInt();
    const auto channels = top["channels"].asInt();
    const auto game     = top["game"]["name"].asString();
    
    
    std::cout << "  "  << std::setw(11) << std::right << viewers 
              << " / " << std::setw(11) << std::right << channels 
              << "  "  << game  << std::endl;
}
