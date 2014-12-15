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
#define DESC_DESC     "Print descriptive line headers, if applicable." 
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

#define VAL(arg)                                                               \
    opt::value((arg))
    
#define VAL_MUL(arg)                                                           \
    opt::value((arg))->multitoken()

struct args {
    std::vector<std::string> adds;
    std::vector<std::string> removes;
    std::vector<std::string> channels;
    std::vector<std::string> s_channels;
    std::vector<std::string> s_games;
    std::vector<std::string> s_streams;
    std::vector<std::string> streams;
    bool live;
    bool json;
    bool verbose;
    bool info;
    unsigned int limit;
};

namespace opt = boost::program_options;

const std::string home(std::getenv("HOME"));

bookmarks bookmarks(home + "/.config/tq/bookmarks");
std::string config(home + "/.config/tq/tq.conf");

int main(int argc, char *argv[])
{
    std::string usage("Usage: ");
    usage += std::string(argv[0]);
    usage += " option1 [arg1][arg2]... option2 [arg1]...\n";
    
    opt::options_description desc(usage + "\nOptions");
    args args;
    query query;

    desc.add_options()
        ("add-bookmark,a",    VAL_MUL(&args.adds),       DESC_ADD_B)
        ("channels,C",        VAL_MUL(&args.channels),   DESC_CHANNELS)
        ("check-bookmarks,b",                            DESC_CHECK_B)
        ("descriptive,d",                                DESC_DESC)
        ("featured,f",                                   DESC_FEATURED)
        ("get-bookmarks",                                DESC_GET_F)
        ("help,h",                                       DESC_HELP)
        ("json,j",                                       DESC_JSON)
        ("limit",             VAL(&args.limit),          DESC_LIMIT)
        ("live",                                         DESC_LIVE)
        ("remove-bookmark,r", VAL_MUL(&args.removes),    DESC_REMOVE_B)
        ("search-channels,c", VAL_MUL(&args.s_channels), DESC_SEARCH_C)
        ("search-games,g",    VAL_MUL(&args.s_games),    DESC_SEARCH_G)
        ("search-streams,s",  VAL_MUL(&args.s_streams),  DESC_SEARCH_S)
        ("streams,S",         VAL_MUL(&args.streams),    DESC_STREAMS)
        ("top,t",                                        DESC_TOP)
        ("verbose,v",                                    DESC_VERBOSE);

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
        args.info    = (argv_map.count("descriptive") > 0);
        
        response_printer printer(config, args.json, args.verbose, args.info);
        
        if (argv_map.count("add-bookmark"))
            bookmarks.add(args.adds);
        
        if (argv_map.count("remove-bookmark"))
            bookmarks.remove(args.removes);
        
        if (argv_map.count("check-bookmarks"))
            bookmarks.check(printer, query);
        
        if (argv_map.count("get-bookmarks"))
            std::cout << bookmarks;
        
        std::vector<std::pair<query::type, std::string>> arg_vec;
        
        /* 
         * Push all arguments into a single vector which will be handled 
         * later on in a single for-loop. 
         */
        
        if (argv_map.count("featured")) {
            auto pair = std::make_pair(query::TYPE_FEATURED, std::string());
            arg_vec.push_back(pair);
        }
        
        for (const auto &x : args.channels)
            arg_vec.push_back(std::make_pair(query::TYPE_CHANNELS, x));
        
        for (const auto &x : args.s_channels)
            arg_vec.push_back(std::make_pair(query::TYPE_SEARCH_C, x));
        
        for (const auto &x : args.s_games)
            arg_vec.push_back(std::make_pair(query::TYPE_SEARCH_G, x));
        
        for (const auto &x : args.s_streams)
            arg_vec.push_back(std::make_pair(query::TYPE_SEARCH_S, x));
        
        for (const auto &x : args.streams)
            arg_vec.push_back(std::make_pair(query::TYPE_STREAMS, x));
        
        if (argv_map.count("top")) {
            auto pair = std::make_pair(query::TYPE_TOP, std::string());
            arg_vec.push_back(pair);
        }
        
        if (args.live)
            query.set_live(true);
        
        if (argv_map.count("limit"))
            query.set_limit(args.limit);
        
        for (const auto &x : arg_vec) {
            query.set_name(x.second);
            
            auto response = query.get_response(x.first);
            
            printer.print_response(response);
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}