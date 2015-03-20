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

#ifndef _STREAM_OPENER_HPP_
#define _STREAM_OPENER_HPP_

#include <string>
#include <vector>
#include <memory>

#include "config.hpp"

class stream_opener {
public:
    explicit stream_opener(std::shared_ptr<const config> conf);
    
    void run(const std::string &stream, const std::vector<std::string> &args);
    
private:
    std::shared_ptr<const config> _config;
};

#endif /* _STREAM_OPENER_HPP_ */