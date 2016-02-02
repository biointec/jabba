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

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#define MAX_TIMERS 16

/**
 * Utility class with timers
 */
class Util
{
private:
        static int currentTimer;
        static std::chrono::time_point<std::chrono::system_clock> startTime[MAX_TIMERS];

public:
        /**
         * Create a string with a human readable version of a time period
         * @param time Time period (expressed in s)
         * @return String with a human readable version of a time period
         */
        static std::string humRead(double time);

        /**
         * Start a chronometer
         */
        static void startChrono();

        /**
         * Stop the chronometer
         * @return The time in
         */
        static double stopChrono();

        /**
         * Stop the chronometer and return a human readable string
         * @return A human readable string containg the elapsed time
         */
        static std::string stopChronoStr() {
                return humRead(stopChrono());
        }

        static std::string getTime();

        /**
         * Compute the sensitivity
         * @param TN True negatives
         * @param FP False positives
         * The sensitivity
         */
        static double getSpecificity(double TN, double FP) {
                return ((TN+FP) == 0) ? 1.0 : TN / (TN + FP);
        }

        /**
         * Compute the specificity
         * @param TP True positives
         * @param FN False negatives
         * The specificity
         */
        static double getSensitivity(double TP, double FN) {
                return ((TP+FN) == 0) ? 1.0 : TP / (TP + FN);
        }

        static bool fileExists(const std::string& filename) {
                std::ifstream file(filename.c_str(), std::ios::in);
                bool OK = file.good();
                file.close();
                return OK;
        }
};

#endif
