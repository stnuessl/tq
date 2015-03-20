/*
 * Copyright (C) 2015  Steffen Nüssle
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

#ifndef _RESOURCE_GUARD_HPP_
#define _RESOURCE_GUARD_HPP_

#include <functional>
#include <utility>
#include <stdexcept>

/* Useful class when interfacing with pure C resources */

template <typename T1, typename T2>
class resource_guard {
public:
    explicit resource_guard(T1 x, std::function<T2> func);
    resource_guard(resource_guard<T1, T2> &&other);
    ~resource_guard();
    
    resource_guard(const resource_guard<T1, T2> &other) = delete;
    
    resource_guard<T1, T2> &
    operator=(const resource_guard<T1, T2> &other) = delete;
private:
    T1 _x;
    std::function<T2> _func;
};

template <typename T1, typename T2>
resource_guard<T1, T2>::resource_guard(T1 x, std::function<T2> func)
    : _x(x),
      _func(func)
{
    if (!func)
        throw std::invalid_argument("No valid function specified.");
}

template <typename T1, typename T2>
resource_guard<T1, T2>::resource_guard(resource_guard<T1, T2> &&other)
    : _x(std::move(other._x)),
      _func(std::move(other._func))
{
    other._func = nullptr;
}

template <typename T1, typename T2>
resource_guard<T1, T2>::~resource_guard()
{
    /* 
     * By using the move constructor there can be objects which do not have
     * to clean up anything
     */
    if (_func)
        _func(_x);
}

template <typename T1, typename T2>
resource_guard<T1, T2> make_resource_guard(T1 x, T2 *func)
{
    return resource_guard<T1, T2>(x, func);
}

#endif /* _RESOURCE_GUARD_HPP_ */