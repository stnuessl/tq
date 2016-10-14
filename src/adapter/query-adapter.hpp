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

#ifndef _QUERY_ADAPTER_HPP_
#define _QUERY_ADAPTER_HPP_

#include <string>
#include <vector>
#include <memory>

#include "query-results.hpp"
#include "../query/query.hpp"

class query_adapter {
public:
    typedef std::vector<std::unique_ptr<result>> result_vector;
    
    query_adapter(const std::string &client_id);
    query_adapter(const char *client_id);
    
    std::unique_ptr<result> bookmarks(const std::vector<std::string> &channels);
    std::unique_ptr<result> channels(const std::string &name);
    std::unique_ptr<result> featured_streams(unsigned int limit);
    std::unique_ptr<result> search_channels(const std::string &query, 
                                            unsigned int limit);
    std::unique_ptr<result> search_games(const std::string &query, bool live);
    std::unique_ptr<result> search_streams(const std::string &query, 
                                           unsigned int limit);
    std::unique_ptr<result> streams(const std::string &game, 
                                    unsigned int limit);
    std::unique_ptr<result> streams(const std::vector<std::string> &channels);
    std::unique_ptr<result> top_games(unsigned int limit);
    std::unique_ptr<result> users(const std::string &name);
private:
    template <typename T>
    std::unique_ptr<result> handle_response(const std::string &str);

    Json::Reader _json_reader;
    query _query;
};

#endif
