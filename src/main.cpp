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

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <utility>
#include <cstdlib>

#include <json/json.h>

#include <boost/program_options.hpp>

#include "url-client.hpp"
#include "query.hpp"
#include "response-printer.hpp"
#include "bookmarks.hpp"

#define DESC_ADD_B    "Add a new bookmark."
#define DESC_CHANNELS "Retrieve information about a channel."
#define DESC_CHECK_B  "Check which bookmarks are streaming."
#define DESC_FEATURED "Query featured streams."
#define DESC_GET_F    "Show all specified bookmarks."
#define DESC_HELP     "Print this help message."
#define DESC_JSON     "Pretty print the json strings sent from the server."
#define DESC_LIMIT    "Set the number of returned results."
#define DESC_LIVE     "If searching for games: list only games that are live."
#define DESC_REMOVE_B "Remove a bookmark."
#define DESC_SEARCH_C "Search for channels."
#define DESC_SEARCH_G "Search for games."
#define DESC_SEARCH_S "Search for streams."
#define DESC_STREAMS  "Retrieve information about a steam. Stream must be live."
#define DESC_TOP      "Get a list of the currently top played games."
#define DESC_VERBOSE  "Retrieve more information about queried items."

struct args {
    std::string add;
    std::string remove;
    std::string channel;
    std::string s_channels;
    std::string s_games;
    std::string s_streams;
    std::string stream;
    bool live;
    bool json;
    bool verbose;
    unsigned int limit;
};

namespace opt = boost::program_options;

const std::string home(std::getenv("HOME"));

bookmarks bookmarks(home + "/.config/tq/bookmarks");
std::string config(home + "/.config/tq/tq.conf");

int main(int argc, char *argv[])
{
    opt::options_description desc("Available commands");
    args args;

    desc.add_options()
        ("add-bookmark,a",    opt::value(&args.add),        DESC_ADD_B)
        ("channels,C",        opt::value(&args.channel),    DESC_CHANNELS)
        ("check-bookmarks,b",                               DESC_CHECK_B)
        ("featured,f",                                      DESC_FEATURED)
        ("get-bookmarks",                                   DESC_GET_F)
        ("help,h",                                          DESC_HELP)
        ("json,j",                                          DESC_JSON)
        ("limit",             opt::value(&args.limit),      DESC_LIMIT)
        ("live",                                            DESC_LIVE)
        ("remove-bookmark,r", opt::value(&args.remove),     DESC_REMOVE_B)
        ("search-channels,c", opt::value(&args.s_channels), DESC_SEARCH_C)
        ("search-games,g",    opt::value(&args.s_games),    DESC_SEARCH_G)
        ("search-streams,s",  opt::value(&args.s_streams),  DESC_SEARCH_S)
        ("streams,S",         opt::value(&args.stream),     DESC_STREAMS)
        ("top,t",                                           DESC_TOP)
        ("verbose,v",                                       DESC_VERBOSE);

    try {
        opt::variables_map argv_map;
        
        auto parsed = opt::parse_command_line(argc, argv, desc);
        opt::store(parsed, argv_map);
        opt::notify(argv_map);
                
        if (argv_map.empty() || argv_map.count("help")) {
            std::cout << desc << std::endl;
            std::exit(EXIT_SUCCESS);
        }
        
        args.verbose = (argv_map.count("verbose") > 0);
        args.json    = (argv_map.count("json") > 0);
        args.live    = (argv_map.count("live") > 0);
        
        response_printer printer(config, args.json, args.verbose);
        
        if (argv_map.count("check-bookmarks"))
            bookmarks.check(printer);
        
        if (argv_map.count("get-bookmarks"))
            std::cout << bookmarks;
        
        if (argv_map.count("add-bookmark"))
            bookmarks.add(args.add);
        
        if (argv_map.count("remove-bookmark"))
            bookmarks.remove(args.add);

        std::vector<std::pair<query::type, std::string>> arg_vec;
        
        if (argv_map.count("channels")) {
            auto pair = std::make_pair(query::TYPE_CHANNELS, args.channel);
            arg_vec.push_back(pair);
        }
        
        if (argv_map.count("featured")) {
            auto pair = std::make_pair(query::TYPE_FEATURED, std::string());
            arg_vec.push_back(pair);
        }
        
        if (argv_map.count("search-channels")) {
            auto pair = std::make_pair(query::TYPE_SEARCH_C, args.s_channels);
            arg_vec.push_back(pair);
        }
        
        if (argv_map.count("search-games")) {
            auto pair = std::make_pair(query::TYPE_SEARCH_G, args.s_games);
            arg_vec.push_back(pair);
        }
            
        if (argv_map.count("search-streams")) {
            auto pair = std::make_pair(query::TYPE_SEARCH_S, args.s_streams);
            arg_vec.push_back(pair);
        }
        
        if (argv_map.count("streams")) {
            auto pair = std::make_pair(query::TYPE_STREAMS, args.stream);
            arg_vec.push_back(pair);
        }
        
        if (argv_map.count("top")) {
            auto pair = std::make_pair(query::TYPE_TOP, std::string());
            arg_vec.push_back(pair);
        }

        query q;
        
        if (args.live)
            q.set_live(true);
        
        if (argv_map.count("limit"))
            q.set_limit(args.limit);
        
        for (const auto &x : arg_vec) {

            q.set_name(x.second);

            auto response = q.get_response(x.first);
            
            printer.print_response(response);
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}