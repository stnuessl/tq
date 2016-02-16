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

static void sanitize_time_string(std::string &str)
{
    /* 
     * String is of the form "YYYY-MM-DDTHH:MM:SSZ"
     * Sanitizing it makes it to "YYYY-MM-DD" 
     * which is sufficient here
     */
    auto at = str.find('T');
    if (at != std::string::npos)
        str.erase(at);
}

const unsigned int response_printer::default_max_integer_length = 11;
const unsigned int response_printer::default_max_name_length = 20;
const unsigned int response_printer::default_max_game_length = 40;

response_printer::response_printer()
    : _reader(),
      _int_len(default_max_integer_length),
      _name_len(default_max_name_length),
      _game_len(default_max_game_length),
      _section(false),
      _verbose(false),
      _descriptive(false),
      _json(false)
{

}

void response_printer::set_max_integer_length(unsigned int len)
{
    _int_len = len;
}

void response_printer::set_max_name_length(unsigned int len)
{
    _name_len = len;
}

void response_printer::set_max_game_length(unsigned int len)
{
    _game_len = len;
}

void response_printer::set_json(bool val)
{
    _json = val;
}

void response_printer::set_verbose(bool val)
{
    _verbose = val;
}

void response_printer::set_descriptive(bool val)
{
    _descriptive = val;
}

void response_printer::set_section(bool val)
{
    _section = val;
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
    
    switch (type) {
    case query::TYPE_CHANNELS:
        print_channels(val);
        break;
    case query::TYPE_FEATURED:
        print_featured(val);
        break;
    case query::TYPE_SEARCH_CHANNELS:
        print_search_channels(val);
        break;
    case query::TYPE_SEARCH_GAMES:
        print_search_games(val);
        break;
    case query::TYPE_SEARCH_STREAMS:
        print_search_streams(val);
        break;
    case query::TYPE_STREAMS:
        print_streams(val);
        break;
    case query::TYPE_TOP:
        print_top(val);
        break;
    case query::TYPE_USER:
        print_user(val);
        break;
    default:
        throw std::invalid_argument("Invalid response type");
    }
}

void response_printer::print_channels(const Json::Value &val)
{    
    print_channel_full(val);
}

void response_printer::print_featured(const Json::Value& val)
{
    if (_section)
        std::cout << "[ Featured ]:\n";
    
    auto list = val["featured"];

    if (_verbose) {
        for (auto &x : list)
            print_stream_full(x["stream"]);
    } else {
        if (_descriptive)
            print_stream_short_header();
        
        for (auto &x : list)
            print_stream_short(x["stream"]);
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

void response_printer::print_search_channels(const Json::Value &val)
{
    if (_section)
        std::cout << "[ Search Channels ]:\n";
    
    auto channels = val["channels"];
    
    if (_verbose) {
        for (auto &chan : channels)  
            print_channel_full(chan);        
    } else {
        if (_descriptive)
            print_channel_short_header();
        
        for (auto &chan : channels)
            print_channel_short(chan);
    }   
}

void response_printer::print_search_games(const Json::Value &val)
{
    if (_section)
        std::cout << "[ Search Games ]:\n";
    
    if (_descriptive)
        print_game_header();
    
    for (auto &x : val["games"]) {
        auto name = x["name"].asString();
        auto pop  = x["popularity"].asInt();
        
        std::cout << "  " << std::setw(_int_len) << std::right << pop
                  << "  " << name 
                  << std::endl;
    }
}

void response_printer::print_search_streams(const Json::Value &val)
{
    if (_section)
        std::cout << "[ Search Streams ]:\n";
    
    auto streams = val["streams"];
    
    if (_verbose) {
        for (auto &s : streams)
            print_stream_full(s);
    } else {
        if (_descriptive)
            print_stream_short_header();
        
        for (auto &s : streams)
            print_stream_short(s);
    }
}

void response_printer::print_streams(const Json::Value& val)
{
    static const std::string s_channel = "channel=";
    std::map<std::string, const Json::Value *> received_streams;
    static const std::string s_seperator = "%2C";
    auto streams = val["streams"];
    auto self    = val["_links"]["self"].asString();
    
    
    /* gather all returned streams */
    for (auto &x : streams) {
        const auto name = x["channel"]["name"].asString();
        
        received_streams[name] = &x;
    }
    
    if (_section)
        std::cout << "[ Streams ]:\n";
    
    /* 
     * Iterate over all queried streams and print them.
     * The queried names are extracted from link 'self'.
     */
    
    auto index = self.find(s_channel);
    if (index == std::string::npos) {
        std::cerr << "  Invalid response from server\n";
        return;
    }
    
    auto begin = index + s_channel.size();
    auto end   = self.find("&", begin);
    
    while (begin < end) {
        index = self.find(s_seperator, begin);
        if (index == std::string::npos || index > end)
            index = end;
        
        const auto name = self.substr(begin, index - begin);
        begin = index + s_seperator.size();
        
        auto it = received_streams.find(name);
        
        if (it != received_streams.end())
            print_stream_full(*it->second);
        else
            std::cout << "  Stream [ " << name << " ]: offline\n";
    }
}

void response_printer::print_top(const Json::Value &val)
{
    if (_section)
        std::cout << "[ Top ]:\n";
    
    if (_descriptive)
        print_top_game_header();
    
    for (auto &x : val["top"])
        print_top_game(x);
}

void response_printer::print_user(const Json::Value &user)
{
    const auto display_name = user["display_name"].asString();
    const auto name         = user["name"].asString();
    const auto bio          = user["bio"].asString();
    const auto id           = user["_id"].asUInt64();
    auto created_at         = user["created_at"].asString();
    auto updated_at         = user["updated_at"].asString();
    
    sanitize_time_string(created_at);
    sanitize_time_string(updated_at);
    
    std::cout << "  User [ " << display_name << " ]:\n"
              << "      Name       : " << name << "\n"
              << "      ID         : " << id << "\n"
              << "      Biography  : " << bio << "\n"
              << "      Created at : " << created_at << "\n"
              << "      Updated at : " << updated_at << std::endl;
}


void response_printer::print_channel_full(const Json::Value &channel)
{
    const auto name     = channel["name"].asString();
    const auto status   = channel["status"].asString();
    const auto id       = channel["_id"].asUInt64();
    const auto url      = channel["url"].asString();
    const auto game     = channel["game"].asString();
    const auto delay    = channel["delay"].asInt();
    const auto mature   = (channel["mature"].asBool()) ? "yes" : "no";
    const auto language = channel["language"].asString();
    auto created_at     = channel["created_at"].asString();
    auto updated_at     = channel["updated_at"].asString();
    
    sanitize_time_string(created_at);
    sanitize_time_string(updated_at);
    
    std::cout << "  Channel [ " << name << " ]:"    << "\n"
              << "      Status     : " << status      << "\n"
              << "      ID         : " << id          << "\n"
              << "      Url        : " << url         << "\n"
              << "      Game       : " << game        << "\n"
              << "      Delay      : " << delay       << " s\n"
              << "      Mature     : " << mature      << "\n"
              << "      Language   : " << language    << "\n"
              << "      Created at : " << created_at  << "\n"
              << "      Updated at : " << updated_at  << std::endl;
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