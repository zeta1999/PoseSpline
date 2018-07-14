/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Modified and adapted by
 *  Copyright (c) 2015, Autonomous Systems Lab / ETH Zurich
 *
 *  Original Copyright (c) 2010, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/
/*
 *  Modified: Stefan Leutenegger (s.leutenegger@imperial.ac.uk)
 *  Modified: Andreas Forster (an.forster@gmail.com)
 */

/**
 * @file okvis/Time.hpp
 * @brief Header file for the TimeBase, Time and WallTime class.
 * @author Willow Garage Inc.
 * @author Stefan Leutenegger
 * @author Andreas Forster
 */

#ifndef INCLUDE_OKVIS_TIME_HPP_
#define INCLUDE_OKVIS_TIME_HPP_

/*********************************************************************
 ** Pragmas
 *********************************************************************/

#ifdef _MSC_VER
// Okvistime has some magic interface that doesn't directly include
// its implementation, this just disables those warnings.
#pragma warning(disable: 4244)
#pragma warning(disable: 4661)
#endif

#ifdef _WIN32
#include <windows.h>
#endif

/*********************************************************************
 ** Headers
 *********************************************************************/

//#include <ros/platform.h>
#include <iostream>
#include <cmath>
//#include <ros/exception.h>
#include "Duration.hpp"
//#include "rostime_decl.h"

/*********************************************************************
 ** Cross Platform Headers
 *********************************************************************/

#ifdef WIN32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif


/*********************************************************************
 ** Exceptions
 *********************************************************************/

/**
 * @brief Thrown if windoze high perf. timestamping is unavailable.
 *
 * @sa getWallTime
 */
    class NoHighPerformanceTimersException : std::runtime_error {
    public:
        NoHighPerformanceTimersException()
                : std::runtime_error("This windows platform does not "
                                             "support the high-performance timing api.") {
        }
    };

/*********************************************************************
 ** Functions
 *********************************************************************/

    void normalizeSecNSec(uint64_t& sec, uint64_t& nsec);
    void normalizeSecNSec(uint32_t& sec, uint32_t& nsec);
    void normalizeSecNSecUnsigned(int64_t& sec, int64_t& nsec);

/*********************************************************************
 ** Time Classes
 *********************************************************************/

/**
 * \brief Base class for Time implementations.  Provides storage, common functions and operator overloads.
 * This should not need to be used directly.
 */
template<class T, class D>
class TimeBase {
public:
    uint32_t sec, nsec;

    TimeBase()
            : sec(0),
              nsec(0) {
    }
    TimeBase(uint32_t _sec, uint32_t _nsec)
            : sec(_sec),
              nsec(_nsec) {
        normalizeSecNSec(sec, nsec);
    }
    explicit TimeBase(double t) {
        fromSec(t);
    }
    ~TimeBase() {
    }
    D operator-(const T &rhs) const;
    T operator+(const D &rhs) const;
    T operator-(const D &rhs) const;
    T& operator+=(const D &rhs);
    T& operator-=(const D &rhs);
    bool operator==(const T &rhs) const;
    inline bool operator!=(const T &rhs) const {
        return !(*static_cast<const T*>(this) == rhs);
    }
    bool operator>(const T &rhs) const;
    bool operator<(const T &rhs) const;
    bool operator>=(const T &rhs) const;
    bool operator<=(const T &rhs) const;

    double toSec() const {
        return (double) sec + 1e-9 * (double) nsec;
    }
    ;
    T& fromSec(double t) {
        sec = (uint32_t) floor(t);
        nsec = (uint32_t) std::round((t - sec) * 1e9);
        return *static_cast<T*>(this);
    }

    uint64_t toNSec() const {
        return (uint64_t) sec * 1000000000ull + (uint64_t) nsec;
    }
    T& fromNSec(uint64_t t);

    inline bool isZero() const {
        return sec == 0 && nsec == 0;
    }
    inline bool is_zero() const {
        return isZero();
    }

};

/**
* \brief Time representation.  May either represent wall clock time or ROS clock time.
*
* okvis::TimeBase provides most of its functionality.
*/
class Time : public TimeBase<Time, Duration> {
public:
    Time()
            : TimeBase<Time, Duration>() {
    }

    Time(uint32_t _sec, uint32_t _nsec)
            : TimeBase<Time, Duration>(_sec, _nsec) {
    }

    explicit Time(double t) {
        fromSec(t);
    }

    /**
     * \brief Retrieve the current time.  Returns the current wall clock time.
     */
    static Time now();
    /**
     * \brief Sleep until a specific time has been reached.
     */
    static bool sleepUntil(const Time& end);

    static void init();
    static void shutdown();
    static void setNow(const Time& new_now);
    static bool useSystemTime();
    static bool isSimTime();
    static bool isSystemTime();

    /**
     * \brief Returns whether or not the current time is valid.  Time is valid if it is non-zero.
     */
    static bool isValid();
    /**
     * \brief Wait for time to become valid
     */
    static bool waitForValid();
    /**
     * \brief Wait for time to become valid, with timeout
     */
    static bool waitForValid(const WallDuration& timeout);
};

extern const Time TIME_MAX;
extern const Time TIME_MIN;

/**
 * \brief Time representation.  Always wall-clock time.
 *
 * okvis::TimeBase provides most of its functionality.
 */
class WallTime : public TimeBase<WallTime, WallDuration> {
public:
    WallTime()
            : TimeBase<WallTime, WallDuration>() {
    }

    WallTime(uint32_t _sec, uint32_t _nsec)
            : TimeBase<WallTime, WallDuration>(_sec, _nsec) {
    }

    explicit WallTime(double t) {
        fromSec(t);
    }

    /**
     * \brief Returns the current wall clock time.
     */
    static WallTime now();

    /**
     * \brief Sleep until a specific time has been reached.
     */
    static bool sleepUntil(const WallTime& end);

    static bool isSystemTime() {
        return true;
    }
};

std::ostream &operator <<(std::ostream &os, const Time &rhs);
std::ostream &operator <<(std::ostream &os, const WallTime &rhs);


inline int64_t NanoFromSeconds(int32_t value) {
    return static_cast<int64_t>(value * 1e9);
}

inline int64_t TimeToNanoseconds(const Time& time) {
    int64_t t = NanoFromSeconds(static_cast<int64_t>(time.sec)) +
                static_cast<int64_t>(time.nsec);
    return t;
}

inline Time NanosecondsToTime(const uint64_t& time) {
    uint32_t sec = static_cast<uint32_t>(time/1e9);
    uint32_t nsec = static_cast<uint32_t>(time - static_cast<uint64_t>(sec*1e9));
    return Time(sec, nsec);
}

#include "Time_imp.hpp"


#endif // INCLUDE_OKVIS_TIME_HPP_

