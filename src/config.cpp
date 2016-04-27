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

#include <string>
#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

#include "config.hpp"
#include "query.hpp"
#include "response-printer.hpp"

namespace opt = boost::program_options;

config::config(const std::string &path)
    : file(path),
      _limit(query::default_limit),
      _live(false),
      _json(false),
      _verbose(false),
      _descriptive(false),
      _section(true),
      _int_len(response_printer::default_max_integer_length),
      _name_len(response_printer::default_max_name_length),
      _game_len(response_printer::default_max_game_length),
      _opener(),
      _args(),
      _shortcuts(),
      _shortcut_map()
{
    _shortcuts.reserve(128);
    _shortcut_map.reserve(128);
    
    opt::options_description desc;
    
    desc.add_options()
        ("args.limit",             opt::value(&_limit))
        ("args.live",              opt::value(&_live))
        ("args.json",              opt::value(&_json))
        ("args.verbose",           opt::value(&_verbose))
        ("args.descriptive",       opt::value(&_descriptive))
        ("args.section",           opt::value(&_section))
        ("printer.integer-length", opt::value(&_int_len))
        ("printer.name-length",    opt::value(&_name_len))
        ("printer.game-length",    opt::value(&_game_len))
        ("stream.opener",          opt::value(&_opener))
        ("stream.arg",             opt::value(&_args))
        ("game-shortcuts.arg",     opt::value(&_shortcuts));
    
    try {
        opt::variables_map conf_var_map;
        std::ifstream i_file(path);
        
        auto parsed = opt::parse_config_file(i_file, desc);
        opt::store(parsed, conf_var_map);
        opt::notify(conf_var_map);

        if (conf_var_map.empty()) {
            std::ofstream o_file(path);
            
            o_file.setf(std::ios::boolalpha);
            
            /*
             * Since twitch.tv changed the behaviour of "search/streams" to
             * stronger match on the stream/channel name instead of the game
             * played we have to use the "/streams" API call. However,
             * this call needs a exact game name match so here are a few
             * shortcuts to help with this. The user may uncomment and specify
             * shortcuts as he wishes.
             */
            o_file << "[args]\n"
                   << "limit       = " << _limit        << "\n"
                   << "live        = " << _live         << "\n"
                   << "section     = " << _section      << "\n"
                   << "verbose     = " << _verbose      << "\n"
                   << "descriptive = " << _descriptive  << "\n"
                   << "json        = " << _json         << "\n\n"
                   << "[printer]\n"
                   << "integer-length = " << _int_len     << "\n"
                   << "name-length    = " << _name_len    << "\n"
                   << "game-length    = " << _game_len    << "\n\n"
                   << "[stream]\n"
                   << "#opener = /usr/bin/livestreamer\n"
                   << "#arg = --default-stream=best\n"
                   << "#arg = --player=/usr/bin/mpv\n"
                   << "#arg = --no-version-check\n"
                   << "#arg = --loglevel=debug\n\n"
                   << "#opener = /usr/bin/chromium\n"
                   << "#arg = --incognito\n"
                   << "#arg = --start-maximized\n\n"
                   << "#opener = /usr/bin/firefox\n\n"
                   << "[game-shortcuts]\n"
                   << "#arg = ?=1979+Revolution\n"
                   << "#arg = ?=7+Days+to+Die\n"
                   << "#arg = ?=Alan+Wake\n"
                   << "#arg = ?=ARK\n"
                   << "#arg = ?=ArmA+III\n"
                   << "arg = bf4=Battlefield+4\n"
                   << "#arg = ?=Black+Desert\n"
                   << "#arg = ?=Blade+and+Soul\n"
                   << "#arg = ?=Call+of+Duty:+Black+Ops+III\n"
                   << "#arg = ?=Chrono+Cross\n"
                   << "#arg = ?=Clash+Royale\n"
                   << "arg = csgo=Counter-Strike:+Global+Offensive\n"
                   << "#arg = ?=Creative\n"
                   << "#arg = ?=Danganronpa:+Trigger+Happy+Havoc\n"
                   << "#arg = ?=Dark+Souls+III\n"
                   << "#arg = ?=Dark+Souls+II:+Scholar+of+the+First+Sin\n"
                   << "#arg = ?=Dark+Souls\n"
                   << "#arg = ?=DayZ\n"
                   << "#arg = ?=Destiny\n"
                   << "arg = d3=Diablo+III:+Reaper+of+Souls\n"
                   << "arg = d2=Diablo+II:+Lord+of+Destruction\n"
                   << "#arg = ?=Don't+Starve+Together\n"
                   << "arg = dota=Dota+2\n"
                   << "#arg = ?=EA+Sports+UFC+2\n"
                   << "#arg = ?=Empire:+Total+War\n"
                   << "#arg = ?=Enter+the+Gungeon\n"
                   << "#arg = ?=EVE+Online\n"
                   << "#arg = ?=Fallout+4\n"
                   << "#arg = ?=FIFA+16\n"
                   << "#arg = ?=Final+Fantasy+XIV:+Heavensward\n"
                   << "#arg = ?=Games+++Demos\n"
                   << "#arg = ?=Gaming+Talk+Shows\n"
                   << "#arg = ?=Gears+of+War+4\n"
                   << "arg = gta4=Grand+Theft+Auto+IV\n"
                   << "arg = gta5=Grand+Theft+Auto+V\n"
                   << "#arg = ?=Guild+Wars+2\n"
                   << "#arg = ?=H1Z1:+King+of+the+Kill\n"
                   << "#arg = ?=Halo+5:+Guardians\n"
                   << "#arg = ?=Hearthstone:+Heroes+of+Warcraft\n"
                   << "#arg = ?=Heroes+of+Newerth\n"
                   << "arg = hots=Heroes+of+the+Storm\n"
                   << "#arg = ?=HEX:+Shards+of+Fate\n"
                   << "#arg = ?=HITMAN\n"
                   << "#arg = ?=Kerbal+Space+Program\n"
                   << "#arg = ?=Kingdom+Hearts+χ[chi]\n"
                   << "#arg = ?=King's+Field\n"
                   << "arg = lol=League+of+Legends\n"
                   << "#arg = ?=Madden+NFL+16\n"
                   << "#arg = ?=Mafia+LIVE!\n"
                   << "#arg = ?=Magic:+The+Gathering\n"
                   << "#arg = ?=MapleStory\n"
                   << "#arg = ?=Minecraft\n"
                   << "#arg = ?=MLB+The+Show+16\n"
                   << "#arg = ?=Monopoly\n"
                   << "#arg = ?=Mortal+Kombat+X\n"
                   << "arg = mugen=M.U.G.E.N\n"
                   << "#arg = ?=Music\n"
                   << "#arg = ?=NBA+2K16\n"
                   << "#arg = ?=Ni-Oh\n"    
                   << "#arg = ?=osu!\n"
                   << "#arg = ?=Paragon\n"
                   << "arg = poe=Path+of+Exile\n"
                   << "#arg = ?=Pinball\n"
                   << "#arg = ?=Pokémon+Omega+Ruby/Alpha+Sapphire\n"
                   << "#arg = ?=Poker\n"
                   << "#arg = ?=Quiplash\n"
                   << "#arg = ?=Ratchet+&+Clank\n"
                   << "#arg = ?=Retro\n"
                   << "#arg = ?=RimWorld\n"
                   << "#arg = ?=Rocket+League\n"
                   << "#arg = ?=RuneScape\n"
                   << "#arg = ?=Rust\n"
                   << "#arg = ?=ShellShock+Live\n"
                   << "#arg = ?=slither.io\n"
                   << "#arg = ?=Smite\n"
                   << "arg = sc2=StarCraft+II\n"
                   << "#arg = ?=Stardew+Valley\n"
                   << "#arg = ?=Star+Wars+Battlefront\n"
                   << "#arg = ?=Street+Fighter+V\n"
                   << "#arg = ?=Summoners+War:+Sky+Arena\n"
                   << "#arg = ?=Sunset+Overdrive\n"
                   << "#arg = ?=Super+Mario+Maker\n"
                   << "#arg = ?=Super+Smash+Bros.+for+Wii+U\n"
                   << "#arg = ?=Super+Smash+Bros.+Melee\n"
                   << "#arg = ?=Team+Fortress+2\n"
                   << "#arg = ?=The+Binding+of+Isaac:+Afterbirth\n"
                   << "#arg = ?=The+Culling\n"
                   << "#arg = ?=The+Elder+Scrolls+Online:+Tamriel+Unlimited\n"
                   << "#arg = ?=The+Legend+of+Zelda:+Ocarina+of+Time\n"
                   << "#arg = ?=The+Legend+of+Zelda:+Twilight+Princess+HD\n"
                   << "#arg = ?=Tibia\n"
                   << "#arg = ?=Tom+Clancy's+Rainbow+Six:+Siege\n"
                   << "#arg = ?=Tom+Clancy's+The+Division\n"
                   << "#arg = ?=Town+of+Salem\n"
                   << "#arg = ?=Tree+of+Savior\n"
                   << "#arg = ?=Twitch+Plays\n"
                   << "arg = wc3=Warcraft+III:+The+Frozen+Throne\n"
                   << "#arg = ?=World+of+Tanks\n"
                   << "arg = wow=World+of+Warcraft\n"
                   << "#arg = ?=World+of+Warships\n\n";
        } else {
            for (auto &&x : _shortcuts) {
                auto i = x.find('=');
                if (i != std::string::npos) {
                    auto game = x.substr(i + sizeof(char));
                    
                    x.erase(i);
                    
                    _shortcut_map[std::move(x)] = std::move(game);
                }
            }
            
            _shortcuts.clear();
        }
        
    } catch (std::exception &e) {
        std::cerr << "** Warning: unable to parse config \"" << path << "\" - " 
                  << e.what() << "\n";
    }
}

unsigned int config::limit() const
{
    return _limit;
}

bool config::live() const
{
    return _live;
}

bool config::json() const
{
    return _json;
}

bool config::verbose() const
{
    return _verbose;
}

bool config::descriptive() const
{
    return _descriptive;
}

bool config::section() const
{
    return _section;
}

unsigned int config::integer_length() const
{
    return _int_len;
}

unsigned int config::name_length() const
{
    return _name_len;
}

unsigned int config::game_length() const
{
    return _game_len;
}

const std::string &config::stream_opener() const
{
    return _opener;
}

const std::vector<std::string> &config::stream_opener_args() const
{
    return _args;
}

const std::unordered_map<std::string, std::string> &
config::game_shortcut_map() const
{
    return _shortcut_map;
}
