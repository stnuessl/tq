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
#include <json/writer.h>

#include <boost/program_options.hpp>

#include "response-printer.hpp"
#include "query.hpp"

namespace opt = boost::program_options;

response_printer::response_printer(const std::string &config, bool json,
                                   bool verbose)
    : file(config),
      _table ({
          { query::TYPE_CHANNELS, &response_printer::print_channels          },
          { query::TYPE_FEATURED, &response_printer::print_featured          },
          { query::TYPE_SEARCH_C, &response_printer::print_search_channels   },
          { query::TYPE_SEARCH_G, &response_printer::print_search_games      },
          { query::TYPE_SEARCH_S, &response_printer::print_search_streams    },
          { query::TYPE_STREAMS,  &response_printer::print_streams           },
          { query::TYPE_TOP,      &response_printer::print_top               }
      }),
      _reader(),
      _int_len(11),
      _name_len(20),
      _game_len(40),
      _json(json),
      _verbose(verbose)
{
    opt::option_description desc;
}

response_printer::~response_printer()
{

}


void response_printer::print_response(const query::response &response)
{
    Json::Value root;
    
    auto type = response.first;
    auto str  = response.second;
    
    auto ok = _reader.parse(str, root, false);
    if (!ok) {
        std::cerr << "Unable to parse response." << std::endl;
        return;
    }
    
    if (_json) {
        Json::StyledStreamWriter().write(std::cout, root);
        return;
    }

    if (root.isMember("error")) {
        auto msg   = root["message"].asString();
        auto code  = root["status"].asInt();
        auto error = root["error"].asString();
        
        std::cerr << "Error: " << error << " / " << code << " - " << msg 
                  << std::endl;
        return;
    }
    
    try {
        auto f = _table[type];
        
        (this->*f)(root);
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void response_printer::print_top(const Json::Value &root)
{
    for (auto &x : root["top"])
        print_top_game(x);
}

void response_printer::print_channels(const Json::Value &root)
{
    print_channel_full(root);
}

void response_printer::print_featured(const Json::Value& root)
{
    auto list = root["featured"];

    if (_verbose) {
        for (auto &x : list) {
            auto stream = x["stream"];
            
            print_stream_full(stream);
        }
    } else {
        for (auto &x : list) {
            auto stream = x["stream"];

            print_stream_short(stream);
        }
    }
}

void response_printer::print_search_channels(const Json::Value& root)
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

void response_printer::print_search_games(const Json::Value& root)
{
    auto games = root["games"];
    
    for (auto &x : games) {
        auto name = x["name"].asString();
        auto pop  = x["popularity"].asInt();
        
        std::cout << "  " << std::setw(_int_len) << std::right << pop
                  << "  " << name 
                  << std::endl;
    }
}

void response_printer::print_search_streams(const Json::Value& root)
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

void response_printer::print_streams(const Json::Value& root)
{
    auto stream = root["stream"];
    
    if (stream.isNull()) {
        auto self = root["_links"]["self"].asCString();
        auto name = std::strrchr(self, '/') + 1;
        
        std::cout << "  Stream [ " << name  << " ]: offline" << std::endl;
    } else {
        print_stream_full(stream);
    }
}

void response_printer::print_channel_full(const Json::Value &channel)
{
    const auto name     = channel["name"].asString();
    const auto status   = channel["status"].asString();
    const auto url      = channel["url"].asString();
    const auto id       = channel["_id"].asUInt64();
    const auto game     = channel["game"].asString();
    const auto delay    = channel["delay"].asInt();
    const auto mature   = (channel["mature"].asBool()) ? "yes" : "no";
    const auto language = channel["language"].asString();
    
    std::cout << "  Channel [ " << name << " ]:"    << "\n"
              << "      Status   : " << status      << "\n"
              << "      ID       : " << id          << "\n"
              << "      Url      : " << url         << "\n"
              << "      Game     : " << game        << "\n"
              << "      Delay    : " << delay       << " s\n"
              << "      Mature   : " << mature      << "\n"
              << "      Language : " << language    << std::endl;    
}

void response_printer::print_channel_short(const Json::Value &channel)
{
    const auto name = channel["name"].asString();
    const auto game = channel["game"].asString();
    const auto url  = channel["url"].asString();
    
    std::cout << "  " << std::setw(_name_len) << std::left << name
              << "  " << std::setw(_game_len) << std::left << game
              << "  " << url << std::endl;
}


void response_printer::print_stream_full(const Json::Value &stream)
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

void response_printer::print_stream_short(const Json::Value &stream)
{
    const auto channel = stream["channel"];
    
    const auto name    = channel["name"].asString();
    const auto url     = channel["url"].asString();
    const auto viewers = stream["viewers"].asInt();
    const auto game    = stream["game"].asString();
    
    std::cout << "  " << std::setw(_int_len)          << std::right << viewers
              << "  " << std::setw(_name_len) << std::left  << name
              << "  " << std::setw(_game_len) << std::left << game
              << "  " << url << std::endl;
}

void response_printer::print_top_game(const Json::Value &top)
{
    const auto viewers  = top["viewers"].asInt();
    const auto channels = top["channels"].asInt();
    const auto game     = top["game"]["name"].asString();
    
    
    std::cout << "  "  << std::setw(_int_len) << std::right << viewers 
              << " / " << std::setw(_int_len) << std::right << channels 
              << "  "  << game  << std::endl;
}
