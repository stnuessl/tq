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

#include "query.hpp"
#include "query-builder.hpp"
#include "response-handler.hpp"
#include "bookmarks.hpp"


#define DESC_ADD_F    ""
#define DESC_CHANNELS "Retrieve information about a channel."
#define DESC_CHECK_F  ""
#define DESC_DEBUG    "Run in debug mode"
#define DESC_FEATURED "Query featured streams."
#define DESC_GET_F    ""
#define DESC_HELP     "Print this help message."
#define DESC_LIMIT    "Set the number of returned results."
#define DESC_LIVE     "If searching for games: list only games that are live."
#define DESC_REMOVE_F ""
#define DESC_SEARCH_C "Search for channels."
#define DESC_SEARCH_G "Search for games."
#define DESC_SEARCH_S "Search for streams."
#define DESC_STREAMS  "Retrieve information about a steam. Stream must be live."
#define DESC_TOP      "Get a list of the currently top played games."
#define DESC_VERBOSE  "Retrieve more information about queried items."

namespace opt = boost::program_options;

bookmarks bookmarks(std::string(std::getenv("HOME")) + "/.config/tq/bookmarks");

int main(int argc, char *argv[])
{
    opt::options_description desc("Available commands");
    bool verbose, debug;
    std::string query, bookmark;
    unsigned int limit;
    
    desc.add_options()
        ("add-bookmark,a",    opt::value(&bookmark), DESC_ADD_F)
        ("channels,C",        opt::value(&query),    DESC_CHANNELS)
        ("check-bookmarks,b",                         DESC_CHECK_F)
        ("debug,d",                                  DESC_DEBUG)
        ("featured,f",                               DESC_FEATURED)
        ("get-bookmarks",                            DESC_GET_F)
        ("help,h",                                   DESC_HELP)
        ("limit",             opt::value(&limit),    DESC_LIMIT)
        ("live,l",                                   DESC_LIVE)
        ("remove-bookmark,r", opt::value(&bookmark), DESC_REMOVE_F)
        ("search-channels,c", opt::value(&query),    DESC_SEARCH_C)
        ("search-games,g",    opt::value(&query),    DESC_SEARCH_G)
        ("search-streams,s",  opt::value(&query),    DESC_SEARCH_S)
        ("streams,S",         opt::value(&query),    DESC_STREAMS)
        ("top,t",                                    DESC_TOP)
        ("verbose,v",                                DESC_VERBOSE);
    
    opt::variables_map argv_map;
    
    try {
        auto parsed = opt::parse_command_line(argc, argv, desc);
        opt::store(parsed, argv_map);
        opt::notify(argv_map);
                
        if (argv_map.count("help")) {
            std::cout << desc << std::endl;
            std::exit(EXIT_SUCCESS);
        }
        
        debug   = (argv_map.count("debug") > 0);
        verbose = (argv_map.count("verbose") > 0);
        
        response_handler handler(verbose);
        
        if (argv_map.count("check-bookmarks")) {
            bookmarks.check(handler);
            std::exit(EXIT_SUCCESS);
        }
        
        if (argv_map.count("get-bookmarks")) {
            std::cout << bookmarks;
            std::exit(EXIT_SUCCESS);
        }
        
        if (argv_map.count("add-bookmark")) {
            bookmarks.add(bookmark);
            std::exit(EXIT_SUCCESS);
        }
        
        if (argv_map.count("remove-bookmark")) {
            bookmarks.remove(bookmark);
            std::exit(EXIT_SUCCESS);
        }

        std::vector<query_builder::type> type_vec;
        
        if (argv_map.count("channels"))
            type_vec.push_back(query_builder::TYPE_CHANNELS);
        
        if (argv_map.count("featured"))
            type_vec.push_back(query_builder::TYPE_FEATURED);
        
        if (argv_map.count("search-channels"))
            type_vec.push_back(query_builder::TYPE_SEARCH_CHANNELS);
        
        if (argv_map.count("search-games"))
            type_vec.push_back(query_builder::TYPE_SEARCH_GAMES);
        
        if (argv_map.count("search-streams"))
            type_vec.push_back(query_builder::TYPE_SEARCH_STREAMS);
        
        if (argv_map.count("streams"))
            type_vec.push_back(query_builder::TYPE_STREAMS);
        
        if (argv_map.count("top"))
            type_vec.push_back(query_builder::TYPE_TOP);
        
        if (type_vec.size() == 0) {
            std::cerr << "** Error: no query type specified. " 
                      << "Use one of the following:\n"
                      << " --search-channels   --channels      --top\n"
                      << " --search-games      --featured\n"
                      << " --search-streams    --streams" << std::endl;
            std::exit(EXIT_FAILURE);
        } 
        
        if (type_vec.size() > 1) {
            std::cerr << "** Error: multiple query types were passed. "
                      << "Only one query type is allowed."
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if (debug) {
            std::cout << "Query: " << query_builder::as_string(type_vec[0])
                      << "\n"
                      << "Query string: " << query << std::endl;
        }
        
        query_builder builder(type_vec[0]);
        
        builder.set_query(query);
        builder.set_live(argv_map.count("live") > 0);
        
        if (argv_map.count("limit"))
            builder.set_limit(limit);

        auto r = builder.build().get_response();
        
        handler.handle_response(r, type_vec[0]);
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}