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

#ifndef _RESPONSE_HANDLER_HPP_
#define _RESPONSE_HANDLER_HPP_

#include <string>

#include <json/json.h>

namespace tq {
    class response_handler {
    public:
        explicit response_handler();
        ~response_handler();
        
        void streams_response(const std::string &response);
        void channel_response(const std::string &response);
        void game_response(const std::string &response);
        void search_streams_response(const std::string &response);
    private:
        Json::Reader _reader;
    };
}

#endif /* _RESPONSE_HANDLER_HPP_ */