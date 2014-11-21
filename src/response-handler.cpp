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

#include "response-handler.hpp"
#include "query-builder.hpp"

static int integer_length_as_string(int n)
{
    int len = 0;
    
    if (n < 0) {
        len += 1;
        n = std::abs(n);
    }
    
    while (n) {
        len += 1;
        n /= 10;
    }
    
    return len;
}

response_handler::response_handler(bool v, bool u)
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
      _v(v),
      _u(u)
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
    auto list = root["top"];
    
    
    if (list.empty())
        return;
    
    auto v = list[0]["viewers"].asInt();
    auto len = integer_length_as_string(v);
    
    for (auto &x : list) {
        auto game    = x["game"];
        auto viewers = x["viewers"].asInt();
        
        auto name    = game["name"].asString();
        
        std::cout << "  " << std::setw(len) << std::right << viewers 
                  << "  " << name 
                  << std::endl;
    }
}

void response_handler::handle_channels(const Json::Value &root)
{
    auto url    = root["url"].asString();
    
    if (_u) {
        std::cout << "  " << url << std::endl;
        return;
    }
    
    auto name   = root["display_name"].asString();
    auto id     = root["_id"].asInt();
    auto game   = root["game"].asString();
    auto delay  = root["delay"].asInt();
    auto mature = (root["mature"].asBool()) ? "yes" : "no";
    
    std::cout << "  Name   : " << name << "\n"
              << "  Id     : " << id << "\n"
              << "  Game   : " << game << "\n"
              << "  Delay  : " << delay << " s\n"
              << "  Mature : " << mature << "\n"
              << "  URL    : " << url 
              << std::endl;
}

void response_handler::handle_featured(const Json::Value& root)
{
    auto list = root["featured"];
    
    auto compare = [](const Json::Value &a, const Json::Value &b) {
        return a["stream"]["viewers"].asInt() < b["stream"]["viewers"].asInt();
    };
    
    /* This list is not sorted ... */
    auto max = std::max_element(list.begin(), list.end(), compare);

    auto v   = (*max)["stream"]["viewers"].asInt();
    auto len = integer_length_as_string(v);
    
    for (auto &x : list) {
        auto stream = x["stream"];

        auto viewers = stream["viewers"].asInt();
        auto name    = stream["channel"]["name"].asString();
        auto url     = stream["channel"]["url"].asString();
        auto game    = stream["game"].asString();
        
        if (name.length() > _max_name_str_len)
            name.resize(_max_name_str_len);
        
        if (game.length() > _max_game_str_len)
            game.resize(_max_game_str_len);
        
        std::cout << "  " << std::setw(len) << std::right << viewers
                  << "  " << std::setw(_max_name_str_len) << std::left << name 
                  << "  " << std::setw(_max_game_str_len) << std::left << game 
                  << "  " << url 
                  << std::endl;
    }
}

void response_handler::handle_search_channels(const Json::Value& root)
{
    auto channels = root["channels"];
    
    for (auto &x : channels) {
        auto url  = x["url"].asString();
        auto game = x["game"].asString();
        auto name = x["name"].asString();
        
        if (name.length() > _max_name_str_len)
            name.resize(_max_name_str_len);
        
        if (game.length() > _max_game_str_len)
            name.resize(_max_game_str_len);
        
        std::cout << "  " << std::setw(_max_name_str_len) << std::left << name
                  << "  " << std::setw(_max_game_str_len) << std::left << game
                  << "  " << url
                  << std::endl;
    }
}

void response_handler::handle_search_games(const Json::Value& root)
{
    auto games = root["games"];
    
    if (games.empty())
        return;
    
    auto v = games[0]["popularity"].asInt();
    auto len = integer_length_as_string(v);
    
    for (auto &x : games) {
        auto name = x["name"].asString();
        auto pop  = x["popularity"].asInt();
        
        std::cout << "  " << std::setw(len) << std::right << pop
                  << "  " << name 
                  << std::endl;
    }
}

void response_handler::handle_search_streams(const Json::Value& root)
{
    auto streams = root["streams"];
    
    if (streams.empty())
        return;
    
    auto v   = streams[0]["viewers"].asInt();
    auto len = integer_length_as_string(v); 
    
    for (auto &stream : streams) {
        auto viewers = stream["viewers"].asInt();
        auto chan = stream["channel"];
        auto game = stream["game"].asString();
        
        auto name = chan["display_name"].asString();
        auto url  = chan["url"].asString();
        
        if (name.length() > _max_name_str_len)
            name.resize(_max_name_str_len);
        
        if (game.length() > _max_game_str_len)
            game.resize(_max_game_str_len);
        
        std::cout << "  " << std::setw(len) << std::right << viewers 
                  << "  " << std::setw(_max_name_str_len) << std::left << name  
                  << "  " << std::setw(_max_game_str_len) << std::left << game
                  << "  " << url 
                  << std::endl;
    }
}

void response_handler::handle_streams(const Json::Value& root)
{
    auto stream = root["stream"];
    
    if (stream.isNull()) {
        auto self = root["_links"]["self"].asCString();
        auto name = std::strrchr(self, '/') + 1;
        
        std::cout << "  Stream " << name  << " is offline." << std::endl;
        return;
    }
    
    auto channel = stream["channel"];
    auto viewers = stream["viewers"].asInt();
    
    auto name    = channel["name"].asString();
    auto id      = channel["_id"].asInt();
    auto game    = channel["game"].asString();
    auto delay   = channel["delay"].asInt();
    auto url     = channel["url"].asString();
    auto status  = channel["status"].asString();
    auto lang    = channel["broadcaster_language"].asString();
    
    std::cout << "  Name     : " << name << "\n"
              << "  Status   : " << status << "\n"
              << "  Id       : " << id << "\n"
              << "  Viewers  : " << viewers << "\n"
              << "  Game     : " << game << "\n"
              << "  Delay    : " << delay << "\n"
              << "  URL      : " << url << "\n"
              << "  Language : " << lang
              << std::endl;
}