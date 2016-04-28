/*
 * Copyright (C) 2016  Steffen Nüssle
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

#include <iomanip>
#include <utility>
#include <unordered_map>

#include "query-results.hpp"

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

result::result()
    : _json_value(),
      _int_len(11),
      _name_len(20),
      _game_len(40),
      _descriptive(false),
      _section(true),
      _verbose(false)
{
}

void result::set_integer_length(unsigned int len)
{
    _int_len = len;
}

void result::set_name_length(unsigned int len)
{
    _name_len = len;
}

void result::set_game_length(unsigned int len)
{
    _game_len = len;
}

void result::set_descriptive(bool val)
{
    _descriptive = val;
}

void result::set_section(bool val)
{
    _section = val;
}

void result::set_verbose(bool val)
{
    _verbose = val;
}

void result::set_json_value (Json::Value &&val)
{
    _json_value = std::move(val);
}

void result::dump_json(std::ostream &out) const
{
    Json::StyledStreamWriter("    ").write(out, _json_value);
}


void result::dump_channel_header(std::ostream &out) const
{
    auto name = std::string("Name");
    auto game = std::string("Game");
    
    trim_string(name, _name_len);
    trim_string(game, _game_len);
    
    out << "  " << std::setw(_name_len) << std::left << name
        << "  " << std::setw(_game_len) << std::left << game
        << "  " << "URL\n" 
        << std::setfill('-') << std::setw(_name_len + _game_len + 40)
        << "" << std::setfill(' ') << "\n";
}

void result::dump_game_header(std::ostream &out) const
{
    auto popularity = std::string ("Popularity");
    
    trim_string(popularity, _int_len);
    
    out << "  " << std::setw(_int_len) << std::left << popularity
        << "  " << "Game\n"
        << std::setfill('-') << std::setw(_int_len + _game_len)
        << "" << std::setfill(' ') << std::endl;
}

void result::dump_stream_header(std::ostream &out) const
{
    auto line_len = _int_len + _name_len + _game_len;
    
    auto viewers = std::string("Viewers");
    auto name = std::string("Name");
    auto game = std::string("Game");
    
    trim_string(viewers, _int_len);
    trim_string(name, _name_len);
    trim_string(game, _game_len);
    
    out << "  " << std::setw(_int_len)  << std::left << viewers
        << "  " << std::setw(_name_len) << std::left << name
        << "  " << std::setw(_game_len) << std::left << game
        << "  " << "URL\n"
        << std::setfill('-') << std::setw(line_len)
        << "" << std::setfill(' ') << "\n";
}

void result::dump_top_header(std::ostream &out) const
{
    auto viewers = std::string("Viewers");
    auto channels = std::string("Channels");
    
    trim_string(viewers, _int_len);
    trim_string(channels, _int_len);
    
    out << "  " << std::setw(_int_len) << std::left << viewers
        << "  " << std::setw(_int_len) << std::left << channels
        << "  " << "Game\n"
        << std::setfill('-') << std::setw(2 * _int_len + _game_len)
        << "" << std::setfill(' ') << "\n";
}

void result::dump_channel(std::ostream &out, const Json::Value &val) const
{
    auto name       = val["name"].asString();
    auto game       = val["game"].asString();
    const auto url  = val["url"].asString();
    
    trim_string(name, _name_len);
    trim_string(game, _game_len);
    
    out << "  " << std::setw(_name_len) << std::left << name
        << "  " << std::setw(_game_len) << std::left << game
        << "  " << url << "\n";
}

void result::dump_channel_full(std::ostream &out, const Json::Value &val) const
{
    const auto name     = val["name"].asString();
    const auto status   = val["status"].asString();
    const auto id       = val["_id"].asUInt64();
    const auto url      = val["url"].asString();
    const auto game     = val["game"].asString();
    const auto delay    = val["delay"].asInt();
    const auto mature   = (val["mature"].asBool()) ? "yes" : "no";
    const auto language = val["language"].asString();
    auto created_at     = val["created_at"].asString();
    auto updated_at     = val["updated_at"].asString();
    
    sanitize_time_string(created_at);
    sanitize_time_string(updated_at);
    
    out << "  Channel [ " << name << " ]:"      << "\n"
        << "      Status     : " << status      << "\n"
        << "      ID         : " << id          << "\n"
        << "      Url        : " << url         << "\n"
        << "      Game       : " << game        << "\n"
        << "      Delay      : " << delay       << " s\n"
        << "      Mature     : " << mature      << "\n"
        << "      Language   : " << language    << "\n"
        << "      Created at : " << created_at  << "\n"
        << "      Updated at : " << updated_at  << "\n";
}


void result::dump_stream(std::ostream &out, const Json::Value &val) const
{
    const auto channel = val["channel"];
    
    auto name           = channel["name"].asString();
    const auto url      = channel["url"].asString();
    const auto viewers  = val["viewers"].asInt();
    auto game           = val["game"].asString();
    
    trim_string(name, _name_len);
    trim_string(game, _game_len);
    
    out << "  " << std::setw(_int_len)  << std::right << viewers
        << "  " << std::setw(_name_len) << std::left  << name
        << "  " << std::setw(_game_len) << std::left << game
        << "  " << url << "\n";
}

void result::dump_stream_full(std::ostream &out, const Json::Value &val) const
{
    const auto chan = val["channel"];
    
    const auto name    = chan["name"].asString();
    const auto url     = chan["url"].asString();
    const auto viewers = val["viewers"].asInt();
    const auto id      = val["_id"].asUInt64();
    const auto game    = val["game"].asString();
    
    out << "  Stream [ " << name << " ]:"   << "\n"
        << "      Url     : " << url        << "\n"
        << "      Viewers : " << viewers    << "\n"
        << "      Id      : " << id         << "\n"
        << "      Game    : " << game       << "\n";
}

void result::dump_stream_list(std::ostream &out, const Json::Value &val) const
{
    if (_verbose) {
        for (const auto &x : val["streams"])
            dump_stream_full(out, x);
    } else {
        if (_descriptive)
            dump_stream_header(out);
        
        for (const auto &x : val["streams"])
            dump_stream(out, x);
    }
}


void error_result::dump(std::ostream &out) const
{
    auto msg   = _json_value["message"].asString();
    auto code  = _json_value["status"].asInt();
    auto error = _json_value["error"].asString();
    
    out << "** ERROR: received error message from server: " << error
        << " / " << code << " - " << msg << std::endl;
}

void bookmarks_result::dump(std::ostream &out) const
{
    static const std::string begin_str = "channel=";
    static const std::string sep_str = "%2C";
    
    if (_section)
        std::cout << "[ Bookmarks ]:\n";
    
    auto stream_map = std::unordered_map<std::string, const Json::Value *>();
    for (auto &x : _json_value["streams"]) {
        const auto name = x["channel"]["name"].asString();
        
        stream_map[name] = &x;
    }

    /* 
     * Iterate over all queried streams and print them.
     * The queried names are extracted from link 'self'.
     */
    auto self = _json_value["_links"]["self"].asString();

    
    auto index = self.find(begin_str);
    if (index == std::string::npos)
        throw std::runtime_error("Invalid response from server\n");
    
    auto begin = index + begin_str.size();
    
    auto end = self.find("&", begin);
    if (end == std::string::npos)
        end = self.size();
    
    while (begin < end) {
        index = self.find(sep_str, begin);
        if (index == std::string::npos || index > end)
            index = end;
        
        const auto name = self.substr(begin, index - begin);
        begin = index + sep_str.size();
        
        auto it = stream_map.find(name);
        
        if (it != stream_map.end())
            dump_stream_full(out, *it->second);
        else
            std::cout << "  Stream [ " << name << " ]: offline\n";
    }
}

void channels_result::dump(std::ostream &out) const
{
    dump_channel_full(out, _json_value);
}

void featured_streams_result::dump(std::ostream &out) const
{
    if (_section)
        out << "[ Featured ]:\n";
    
    auto featured_list = _json_value["featured"];
    
    if (_verbose) {
        for (const auto &x : featured_list)
            dump_stream_full(out, x["stream"]);
        
    } else {
        if (_descriptive)
            dump_stream_header(out);
        
        for (const auto &x : featured_list)
            dump_stream(out, x["stream"]);
    }
}

void search_channels_result::dump(std::ostream &out) const
{
    if (_section)
        out << "[ Search Channels ]:\n";
    
    if (_verbose) {
        for (const auto &x : _json_value["channels"])
            dump_channel_full(out, x);
    } else {
        if (_descriptive)
            dump_channel_header(out);
        
        for (const auto &x : _json_value["channels"])
            dump_channel(out, x);
    }
}

void search_games_result::dump(std::ostream &out) const
{
    if (_section)
       out << "[ Search Games ]:\n";
    
    if (_descriptive)
        dump_game_header(out);
    
    for (auto &x : _json_value["games"]) {
        auto name = x["name"].asString();
        auto pop  = x["popularity"].asInt();
        
        out << "  " << std::setw(_int_len) << std::right << pop
            << "  " << name << "\n";
    }
}

void search_streams_result::dump(std::ostream &out) const
{
    if (_section)
        out << "[ Search Streams ]:\n";
    
    dump_stream_list(out, _json_value);
}

void streams_result::dump(std::ostream &out) const
{
    if (_section)
        out << "[ Streams ]:\n";
    
    dump_stream_list(out, _json_value);
}

void top_games_result::dump(std::ostream &out) const
{
    if (_section)
        out << "[ Top ]:\n";
    
    if (_descriptive)
        dump_top_header(out);
    
    for (auto &x : _json_value["top"]) {
        const auto viewers = x["viewers"].asInt();
        const auto channels = x["channels"].asInt();
        const auto game = x["game"]["name"].asString();
        
        out << "  " << std::setw(_int_len) << std::right << viewers 
            << "  " << std::setw(_int_len) << std::right << channels 
            << "  " << game  << std::endl;
    }
}

void users_result::dump(std::ostream &out) const
{
    const auto display_name = _json_value["display_name"].asString();
    const auto name         = _json_value["name"].asString();
    const auto bio          = _json_value["bio"].asString();
    const auto id           = _json_value["_id"].asUInt64();
    auto created_at         = _json_value["created_at"].asString();
    auto updated_at         = _json_value["updated_at"].asString();
    
    sanitize_time_string(created_at);
    sanitize_time_string(updated_at);
    
    out << "  User [ " << display_name << " ]:\n"
        << "      Name       : " << name << "\n"
        << "      ID         : " << id << "\n"
        << "      Biography  : " << bio << "\n"
        << "      Created at : " << created_at << "\n"
        << "      Updated at : " << updated_at << "\n";
}
