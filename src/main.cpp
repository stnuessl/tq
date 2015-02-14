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
#include <memory>
#include <cstdlib>

#include <json/json.h>

#include <boost/program_options.hpp>

#include "url-client.hpp"
#include "query.hpp"
#include "response-printer.hpp"
#include "bookmarks.hpp"
#include "stream-opener.hpp"

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
#define DESC_OPEN     "Open the stream for watching."

#define VAL(arg)                                                               \
    opt::value((arg))
    
#define VAL_MUL(arg)                                                           \
    opt::value((arg))->multitoken()

namespace opt = boost::program_options;

struct args {
    std::vector<std::string> add_vec {};
    std::vector<std::string> remove_vec {};
    std::vector<std::string> channel_vec {};
    std::vector<std::string> s_channel_vec {};
    std::vector<std::string> s_game_vec {};
    std::vector<std::string> s_stream_vec {};
    std::vector<std::string> stream_vec {};
    std::vector<std::string> open_vec {};
    bool live = false;
    bool json = false;
    bool verbose = false;
    bool info = false;
    unsigned int limit = query::default_limit;
};

const std::string home(std::getenv("HOME"));
const std::string bookmarks_path = home + "/.config/tq/bookmarks";
const std::string config_path    = home + "/.config/tq/tq.conf";

bookmarks bookmarks(bookmarks_path);
std::shared_ptr<config> tq_config(new config(config_path));

int main(int argc, char *argv[])
{
    std::string usage("Usage: ");
    usage += std::string(argv[0]);
    usage += " option1 [arg1][arg2]... option2 [arg1]...\n";
    
    opt::options_description desc(usage + "\nOptions");
    args args;
    query query;
    stream_opener stream_opener(tq_config);

    desc.add_options()
        ("add-bookmark,a",    VAL_MUL(&args.add_vec),       DESC_ADD_B)
        ("channels,C",        VAL_MUL(&args.channel_vec),   DESC_CHANNELS)
        ("check-bookmarks,b",                               DESC_CHECK_B)
        ("descriptive,d",                                   DESC_DESC)
        ("featured,f",                                      DESC_FEATURED)
        ("get-bookmarks",                                   DESC_GET_F)
        ("help,h",                                          DESC_HELP)
        ("json,j",                                          DESC_JSON)
        ("limit",             VAL(&args.limit),             DESC_LIMIT)
        ("live",                                            DESC_LIVE)
        ("open,o",            VAL_MUL(&args.open_vec),      DESC_OPEN)
        ("remove-bookmark,r", VAL_MUL(&args.remove_vec),    DESC_REMOVE_B)
        ("search-channels,c", VAL_MUL(&args.s_channel_vec), DESC_SEARCH_C)
        ("search-games,g",    VAL_MUL(&args.s_game_vec),    DESC_SEARCH_G)
        ("search-streams,s",  VAL_MUL(&args.s_stream_vec),  DESC_SEARCH_S)
        ("streams,S",         VAL_MUL(&args.stream_vec),    DESC_STREAMS)
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
        args.info    = (argv_map.count("descriptive") > 0);
        
        for (const auto &x : args.open_vec)
            stream_opener.run(x);
        
        response_printer printer(tq_config, args.json, args.verbose, args.info);
        
        if (argv_map.count("add-bookmark"))
            bookmarks.add(args.add_vec);
        
        if (argv_map.count("remove-bookmark"))
            bookmarks.remove(args.remove_vec);
        
        if (argv_map.count("check-bookmarks"))
            bookmarks.check(printer, query);
        
        if (argv_map.count("get-bookmarks"))
            std::cout << bookmarks;
        
        std::vector<query::response> response_vec;
        
        if (argv_map.count("featured"))
            response_vec.push_back(query.featured(args.limit));
        
        for (const auto &x : args.channel_vec)
            response_vec.push_back(query.channels(x));
        
        for (const auto &x : args.s_channel_vec)
            response_vec.push_back(query.search_channels(x, args.limit));
        
        for (const auto &x : args.s_game_vec)
            response_vec.push_back(query.search_games(x, args.live));
        
        for (const auto &x : args.s_stream_vec)
            response_vec.push_back(query.search_streams(x, args.limit));
        
        for (const auto &x : args.stream_vec)
            response_vec.push_back(query.streams(x));
        
        if (argv_map.count("top"))
            response_vec.push_back(query.top(args.limit));
        
        /* Print responses from server */
        for (const auto &x : response_vec)
            printer.print_response(x);
        
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}