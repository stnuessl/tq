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

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <vector>
#include <utility>
#include <memory>
#include <cstdlib>

#include <json/json.h>

#include <boost/program_options.hpp>

#include "adapter/query-adapter.hpp"
#include "bookmarks.hpp"
#include "stream-opener.hpp"

#define DESC_ADD_B     "Add a new bookmark."
#define DESC_CHANNELS  "Retrieve information about a channel."
#define DESC_CHECK_B   "Check which bookmarks are streaming."
#define DESC_DESC      "Print descriptive line headers, if applicable." 
#define DESC_FEATURED  "Query featured streams."
#define DESC_GAME      "Search streams showcasing game [arg]. The game name "  \
                       "must be a game shortcut or an exact match. "           \
                       "The configuration \"~/.config/tq/tq.conf\" showcases " \
                       "a few examples."
#define DESC_GET_B     "Show all currently saved bookmarks."
#define DESC_HELP      "Print this help message."
#define DESC_JSON      "Pretty print the raw json responses from the server."
#define DESC_LIMIT     "Set the number of returned results."
#define DESC_LIVE      "If searching for games: list only games that are "     \
                       "currently played on live streams."
#define DESC_NO_SEC    "Do not print a section header, if applicable."
#define DESC_REMOVE_B  "Remove a currently saved bookmark."
#define DESC_SEARCH_C  "Search for channels with name [arg]."
#define DESC_SEARCH_G  "Search for games with name [arg]."
#define DESC_SEARCH_S  "Search for streams with name [arg]."
#define DESC_SHOW_SH   "Print all currently loaded game shortcuts. Shortcuts " \
                       "can be enabled in \"~/.config/tq/tq.conf\"."
#define DESC_STREAMS   "Retrieve information about a stream. "                 \
                       "Stream must be currently online."
#define DESC_TOP       "Get a list of the currently top played games."
#define DESC_USER      "Retrieve information about a user."
#define DESC_VERBOSE   "Retrieve more information about queried items."
#define DESC_OPEN      "Open the stream for watching. A stream opener must be "\
                       "specified in \"~/.config/tq/tq.conf\"."
#define DESC_OPEN_ARGS "Overwrite the arguments passed to the stream-opener."

#define VAL(arg)                                                               \
    opt::value((arg))
    
#define VAL_MUL(arg)                                                           \
    opt::value((arg))->multitoken()

namespace opt = boost::program_options;

const std::string home(std::getenv("HOME"));
const std::string bookmarks_path = home + "/.config/tq/bookmarks";
const std::string config_path    = home + "/.config/tq/tq.conf";

bookmarks bookmarks(bookmarks_path);
std::shared_ptr<config> conf(new config(config_path));

int main(int argc, char *argv[])
{
    std::vector<std::string> add_vector;
    std::vector<std::string> channel_vector;
    std::vector<std::string> game_vector;
    std::vector<std::string> open_vector;
    std::vector<std::string> open_args_vector;
    std::vector<std::string> remove_vector;
    std::vector<std::string> search_channel_vector;
    std::vector<std::string> search_game_vector;
    std::vector<std::string> search_stream_vector;
    std::vector<std::string> stream_vector;
    std::vector<std::string> user_vector;
    
    /* 'limit' will get overwritten if it is specified as a program argument */
    unsigned int limit = conf->limit();
    
    std::string usage("Usage: ");
    usage += std::string(argv[0]);
    usage += " option1 [arg1][arg2]... option2 [arg1]...\n\nOptions";
    
    opt::options_description desc(usage);
    
    desc.add_options()
        ("add-bookmark,a",    VAL_MUL(&add_vector),             DESC_ADD_B)
        ("channels,C",        VAL_MUL(&channel_vector),         DESC_CHANNELS)
        ("check-bookmarks,b",                                   DESC_CHECK_B)
        ("descriptive,d",                                       DESC_DESC)
        ("featured,f",                                          DESC_FEATURED)
        ("game,G",            VAL_MUL(&game_vector),            DESC_GAME)
        ("get-bookmarks",                                       DESC_GET_B)
        ("help,h",                                              DESC_HELP)
        ("json,j",                                              DESC_JSON)
        ("limit",             VAL(&limit),                      DESC_LIMIT)
        ("live",                                                DESC_LIVE)
        ("no-section",                                          DESC_NO_SEC)
        ("open,o",            VAL_MUL(&open_vector),            DESC_OPEN)
        ("open-args",         VAL_MUL(&open_args_vector),       DESC_OPEN_ARGS)
        ("remove-bookmark,r", VAL_MUL(&remove_vector),          DESC_REMOVE_B)
        ("search-channels,c", VAL_MUL(&search_channel_vector),  DESC_SEARCH_C)
        ("search-games,g",    VAL_MUL(&search_game_vector),     DESC_SEARCH_G)
        ("search-streams,s",  VAL_MUL(&search_stream_vector),   DESC_SEARCH_S)
        ("show-shortcuts",                                      DESC_SHOW_SH)
        ("streams,S",         VAL_MUL(&stream_vector),          DESC_STREAMS)
        ("top,t",                                               DESC_TOP)
        ("user,u",            VAL_MUL(&user_vector),            DESC_USER)
        ("verbose,v",                                           DESC_VERBOSE);

    try {
        opt::variables_map argv_map;
        
        auto parsed = opt::parse_command_line(argc, argv, desc);
        opt::store(parsed, argv_map);
        opt::notify(argv_map);
        
        if (argv_map.empty() || argv_map.count("help")) {
            std::cout << desc << std::endl;
            std::exit(EXIT_SUCCESS);
        }
        
        auto &shortcut_map = conf->game_shortcut_map();
        auto int_len = conf->integer_length();
        auto name_len = conf->name_length();
        auto game_len = conf->game_length();
        
        auto desc = argv_map.count("descriptive") > 0 || conf->descriptive();
        auto json = argv_map.count("json") > 0 || conf->json();
        auto no_section = argv_map.count("no-section") > 0 || !conf->section();
        auto verbose = argv_map.count("verbose") > 0 || conf->verbose();
        
        if (argv_map.count("show-shortcuts")) {
            std::vector<std::string> shortcuts;
            std::string line;
            
            shortcuts.reserve(shortcut_map.size());
            line.reserve(80);
            
            for (const auto &x : shortcut_map) {
                decltype(game_len) size = x.second.size();
                size = std::min(game_len, size);
                
                line += "  ";
                line.append(x.second, 0, size);
                line.append(game_len - size, ' ');
                line += " : ";
                line += x.first;
                
                std::replace(line.begin(), line.end(), '+', ' ');
                shortcuts.push_back(line);
                line.clear();
            }
            
            std::sort(shortcuts.begin(), shortcuts.end());
            
            if (!no_section)
                std::cout << "[ Game Shortcuts ]:\n";
            
            for (const auto &x : shortcuts)
                std::cout << x << "\n";
        }
        
        auto stream_opener = ::stream_opener(conf);
        for (const auto &x : open_vector)
            stream_opener.run(x, open_args_vector);

        if (argv_map.count("remove-bookmark"))
            bookmarks.remove(remove_vector);

        if (argv_map.count("add-bookmark"))
            bookmarks.add(add_vector);
        
        if (argv_map.count("get-bookmarks"))
            std::cout << bookmarks;

        query_adapter query_adapter;
        auto result_vector = query_adapter::result_vector();
        
        bool live = argv_map.count("live") > 0 || conf->live();
        
        if (argv_map.count("check-bookmarks")) {
            auto result = query_adapter.bookmarks(bookmarks.get());
            
            result_vector.push_back(std::move(result));
        }
        
        for (const auto &x : channel_vector) {
            auto result = query_adapter.channels(x);
            
            result_vector.push_back(std::move(result));
        }
        
        if (argv_map.count("featured")) {
            auto result = query_adapter.featured_streams(limit);
            
            result_vector.push_back(std::move(result));
        }
        
        /* 
         * This query will only work if the game name matches exactly
         * with the one on the server. The config is capable of defining
         * some shortcuts (e.g. sc2 for "StarCraft+II")
         * to make searching for streams of a certain game
         * easier. If no shortcut is found we replace each ' ' with a 
         * '+' (needs no URL encoding) and hope that the other parts of the 
         * string match exactly with the name on the server.
         */
        for (auto &x : game_vector) {
            auto &game = x;
            
            auto it = shortcut_map.find(x);
            if (it == shortcut_map.end())
                std::replace(game.begin(), game.end(), ' ', '+');
            else
                game = it->second;
            
            auto result = query_adapter.streams(game, limit);
            
            result_vector.push_back(std::move(result));
        }
        
        for (const auto &x : search_channel_vector) {
            auto result = query_adapter.search_channels(x, limit);
            
            result_vector.push_back(std::move(result));
        }
        
        for (const auto &x : search_game_vector) {
            auto result = query_adapter.search_games(x, live);
            
            result_vector.push_back(std::move(result));
        }
        
        for (const auto &x : search_stream_vector) {
            auto result = query_adapter.search_streams(x, limit);
            
            result_vector.push_back(std::move(result));
        }
        
        if (argv_map.count("streams")) {
            auto result = query_adapter.streams(stream_vector);
            
            result_vector.push_back(std::move(result));
        }
        
        if (argv_map.count("top")) {
            auto result = query_adapter.top_games(limit);
            
            result_vector.push_back(std::move(result));
        }
        
        for (const auto &x : user_vector) {
            auto result = query_adapter.users(x);
            
            result_vector.push_back(std::move(result));
        }

        /* 
         * All queries are done and the results are collected.
         * Time to print them.
         */

        for (const auto &x : result_vector) {
            x->set_integer_length(int_len);
            x->set_name_length(name_len);
            x->set_game_length(game_len);
            x->set_descriptive(desc);
            x->set_section(!no_section);
            x->set_verbose(verbose);
            
            if (json)
                x->dump_json();
            else
                x->dump();
        }
        
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
