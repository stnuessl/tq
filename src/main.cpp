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

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <json/json.h>

#include "query.hpp"
#include "query-builder.hpp"
#include "response-handler.hpp"

/*
 * Commandline Usage:
 * 
 * tq -g --games
 * tq -s --streams <game>
 * tq --channel-info <channel>
 * tq --stream-info <stream> 
 * tq --search-game <name>
 * 
 * additional options:
 * -v --verbose
 * -u  --url
 */

int main(int argc, char *argv[])
{
    if (argc > 1) {
        tq::query_builder builder;
        
        builder.set_type(tq::query_builder::SEARCH_STREAMS).set_query(argv[1]);
        auto response = builder.build().get_response();
        
        tq::response_handler().search_streams_response(response);
    } else {
        tq::query_builder builder;
        
        builder.set_type(tq::query_builder::GAMES);
        
        auto response = builder.build().get_response();
        
        tq::response_handler().game_response(response);
    }
    
    return EXIT_SUCCESS;
}