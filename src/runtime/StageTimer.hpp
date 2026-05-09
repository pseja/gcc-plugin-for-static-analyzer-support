/**
 * @file StageTimer.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the RAII stage timer.
 * @date 2026-05-08
 *
 * @copyright Copyright (c) 2026 Lukáš Pšeja
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <chrono>

#include "StageStats.hpp"

namespace CodeListener::Core
{

/** RAII guard that measures wall-clock elapsed time and records it into a @ref StageStats on destruction. */
class StageTimer
{
  public:
    /**
     * Start the timer.
     *
     * @param enabled If false this guard is a complete no-op.
     * @param stats   Accumulator that will receive the elapsed time on destruction.
     */
    StageTimer(bool enabled, StageStats &stats) noexcept;

    /** Stop the timer and record the elapsed nanoseconds into @c stats. */
    ~StageTimer();
    /** Deleted copy constructor */
    StageTimer(const StageTimer &) = delete;
    /** Deleted copy assignment operator */
    StageTimer &operator=(const StageTimer &) = delete;

  private:
    /** When `false` the constructor and destructor are both no-ops. */
    bool enabled;
    /** Accumulator that will receive the elapsed time on destruction. */
    StageStats &stats;
    /** Start time point of the timer. */
    std::chrono::steady_clock::time_point start;
};

} // namespace CodeListener::Core
