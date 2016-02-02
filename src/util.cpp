/***************************************************************************
 *   Copyright (C) 2014, 2015 Jan Fostier (jan.fostier@intec.ugent.be)     *
 *   Copyright (C) 2014, 2015 Mahdi Heydari (mahdi.heydari@intec.ugent.be) *
 *   This file is part of Brownie                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "util.h"
#include <ctime>
#include <sstream>
#include <cassert>

using namespace std;
using namespace std::chrono;

time_point<system_clock> Util::startTime[MAX_TIMERS];
int Util::currentTimer = 0;

string Util::humRead(double time)
{
        uint64_t timeInt = uint64_t(time);

        uint64_t days = timeInt / 86400;
        timeInt -= days * 86400;
        uint64_t hours = timeInt / 3600;
        timeInt -= hours * 3600;
        uint64_t min = timeInt / 60;
        timeInt -= min * 60;
        uint64_t sec = timeInt;
        uint64_t ms = uint64_t((time - timeInt) * 1000);

        ostringstream iss;
        if (days > 0) {
                iss << days << "d " << hours << "h " << min << "min";
        } else if (hours > 0) {
                iss << hours << "h " << min << "min " << sec << "s";
        } else if (min > 0) {
                iss << min << "min " << sec << "s";
        } else if (sec > 0) {
                iss << sec << "s " << ms << "ms";
        } else {
                iss << ms << "ms";
        }

        return iss.str();
}

void Util::startChrono()
{
        // make sure we don't use too many timers
        assert(currentTimer < MAX_TIMERS);
        startTime[currentTimer] = system_clock::now();
        currentTimer++;
}

double Util::stopChrono()
{
        // make sure stopChrono isn't called too often
        currentTimer--;
        assert(currentTimer >= 0);

        chrono::duration<double> elapsed = system_clock::now() - startTime[currentTimer];
        return (elapsed.count());
}

string Util::getTime()
{
        time_t time = system_clock::to_time_t(system_clock::now());
        return string(ctime(&time));
}

