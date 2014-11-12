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

#ifndef _QUERY_BUILDER_HPP_
#define _QUERY_BUILDER_HPP_

#include <string>

#include "query.hpp"


/*
 * https://api.twitch.tv/kraken/games/top            get top played games
 * https://api.twitch.tv/kraken/channels/user        get channel info of iser
 * https://api.twitch.tv/kraken/search/channels?q=starcraft
 * https://api.twitch.tv/kraken/streams/test_channel get channel if live
 * https://api.twitch.tv/kraken/search/games?q=star&type=suggest&live=false
 * 
 */

namespace tq {
    class query_builder {
    public:
        enum type {
            CHANNELS,
            GAMES,
            STREAMS,
            SEARCH_CHANNELS,
            SEARCH_GAMES,
            SEARCH_STREAMS,
        };
        
        explicit query_builder();
        ~query_builder();
        
        tq::query_builder &set_type(tq::query_builder::type type);
        tq::query_builder &set_query(const std::string &str);
        tq::query_builder &set_limit(unsigned int limit);
        tq::query_builder &set_live(bool live);
        
        tq::query build() const;
    private:
        static const unsigned int _default_limit;
        static const unsigned int _min_limit;
        static const unsigned int _max_limit;
        
        unsigned int _limit;
        bool _live;
        
        enum type _type;
        std::string _query;
    };
}

#endif /* _QUERY_BUILDER_HPP_ */