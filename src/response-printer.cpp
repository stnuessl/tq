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
#include <cstring>
#include <iostream>

#include <json/writer.h>

#include "response-printer.hpp"

static void trim_string(std::string &str, unsigned int size)
{
    if (str.length() > size)
        str.erase(size);
}

response_printer::response_printer(std::shared_ptr<const config> conf, 
                                   bool json, bool verbose, 
                                   bool informative)
    : _table ({
          { query::TYPE_CHANNELS, &response_printer::print_channels          },
          { query::TYPE_FEATURED, &response_printer::print_featured          },
          { query::TYPE_SEARCH_C, &response_printer::print_search_channels   },
          { query::TYPE_SEARCH_G, &response_printer::print_search_games      },
          { query::TYPE_SEARCH_S, &response_printer::print_search_streams    },
          { query::TYPE_STREAMS,  &response_printer::print_streams           },
          { query::TYPE_TOP,      &response_printer::print_top               }
      }),
      _reader(),
      _int_len(conf->integer_length()),
      _name_len(conf->name_length()),
      _game_len(conf->game_length()),
      _json(json),
      _verbose(verbose),
      _informative(informative)
{

}

void response_printer::print_response(const query::response &response)
{
    Json::Value val;
    
    auto type = response.first;
    auto str  = response.second;
    
    auto ok = _reader.parse(str, val, false);
    if (!ok) {
        std::cerr << "Unable to parse response." << std::endl;
        return;
    }
    
    if (_json) {
        Json::StyledStreamWriter("    ").write(std::cout, val);
        return;
    }

    if (val.isMember("error")) {
        auto msg   = val["message"].asString();
        auto code  = val["status"].asInt();
        auto error = val["error"].asString();
        
        std::cerr << "** Received error message from server: " << error 
                  << " / " << code << " - " << msg << std::endl;
        return;
    }
    
    try {
        auto f = _table[type];
        
        (this->*f)(val);
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void response_printer::print_top(const Json::Value &val)
{
    std::cout << "[ Top ]:\n";
    
    if (_informative)
        print_top_game_header();
    
    for (auto &x : val["top"])
        print_top_game(x);
}

void response_printer::print_channels(const Json::Value &val)
{    
    print_channel_full(val);
}

void response_printer::print_featured(const Json::Value& val)
{
    std::cout << "[ Featured ]:\n";
    
    auto list = val["featured"];

    if (_verbose) {
        for (auto &x : list) {
            auto stream = x["stream"];
            
            print_stream_full(stream);
        }
    } else {
        if (_informative)
            print_stream_short_header();
        
        for (auto &x : list) {
            auto stream = x["stream"];

            print_stream_short(stream);
        }
    }
}

void response_printer::print_channel_short_header() const
{
    std::string channel_name("Channel name"), game("Game");
    
    trim_string(channel_name, _name_len);
    trim_string(game, _game_len);
    
    std::cout << "  " << std::setw(_name_len) << std::left << channel_name
              << "  " << std::setw(_game_len) << std::left << game
              << "  " << "URL\n" 
              << std::setfill('-') << std::setw(_name_len + _game_len + 40)
              << "" << std::setfill(' ') << std::endl;
}

void response_printer::print_stream_short_header() const
{
    auto line_len = _int_len + _name_len + _game_len + 40;
    
    std::string viewers("Viewers"), stream_name("Stream name"), game("Game");
    
    trim_string(viewers, _int_len);
    trim_string(stream_name, _name_len);
    trim_string(game, _game_len);
    
    std::cout << "  " << std::setw(_int_len)  << std::left << viewers
              << "  " << std::setw(_name_len) << std::left << stream_name
              << "  " << std::setw(_game_len) << std::left << game
              << "  " << "URL\n"
              << std::setfill('-') << std::setw(line_len)
              << "" << std::setfill(' ') << std::endl;
}

void response_printer::print_top_game_header() const
{
    std::string viewers("Viewers"), channels("Channels");
    
    trim_string(viewers, _int_len);
    trim_string(channels, _int_len);
    
    std::cout << "  " << std::setw(_int_len) << std::left << viewers
              << "  " << std::setw(_int_len) << std::left << channels
              << "  " << "Game\n"
              << std::setfill('-') << std::setw(2 * _int_len + _game_len)
              << "" << std::setfill(' ') << std::endl;
}

void response_printer::print_game_header() const
{
    std::string popularity("Popularity");
    
    trim_string(popularity, _int_len);
    
    std::cout << "  " << std::setw(_int_len) << std::left << popularity
              << "  " << "Game\n"
              << std::setfill('-') << std::setw(_int_len + _game_len)
              << "" << std::setfill(' ') << std::endl;
}

void response_printer::print_search_channels(const Json::Value& val)
{
    std::cout << "[ Search Channels ]:\n";
    
    auto channels = val["channels"];
    
    if (_verbose) {
        for (auto &chan : channels)  
            print_channel_full(chan);        
    } else {
        if (_informative)
            print_channel_short_header();
        
        for (auto &chan : channels)
            print_channel_short(chan);
    }   
}

void response_printer::print_search_games(const Json::Value& val)
{
    std::cout << "[ Search Games ]:\n";
    
    if (_informative)
        print_game_header();
    
    for (auto &x : val["games"]) {
        auto name = x["name"].asString();
        auto pop  = x["popularity"].asInt();
        
        std::cout << "  " << std::setw(_int_len) << std::right << pop
                  << "  " << name 
                  << std::endl;
    }
}

void response_printer::print_search_streams(const Json::Value& val)
{
    std::cout << "[ Search Streams ]:\n";
    
    auto streams = val["streams"];
    
    if (_verbose) {
        for (auto &s : streams)
            print_stream_full(s);
    } else {
        if (_informative)
            print_stream_short_header();
        
        for (auto &s : streams)
            print_stream_short(s);
    }
}

void response_printer::print_streams(const Json::Value& val)
{
    auto stream = val["stream"];
    
    if (stream.isNull()) {
        auto self = val["_links"]["self"].asCString();
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
    auto name       = channel["name"].asString();
    auto game       = channel["game"].asString();
    const auto url  = channel["url"].asString();
    
    trim_string(name, _name_len);
    trim_string(game, _game_len);
    
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
    
    std::cout << "  Stream [ " << name << " ]:"   << "\n"
              << "      Url     : " << url        << "\n"
              << "      Viewers : " << viewers    << "\n"
              << "      Id      : " << id         << "\n"
              << "      Game    : " << game       << std::endl;
}

void response_printer::print_stream_short(const Json::Value &stream)
{
    const auto channel = stream["channel"];
    
    auto name          = channel["name"].asString();
    const auto url     = channel["url"].asString();
    const auto viewers = stream["viewers"].asInt();
    auto game          = stream["game"].asString();
    
    trim_string(name, _name_len);
    trim_string(game, _game_len);
    
    std::cout << "  " << std::setw(_int_len)  << std::right << viewers
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