/**
 * @file StatisticsReport.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the stage-timing and counter report accumulator and formatted printer.
 *
 * Has to be included before @c gcc-plugin.h because of conflicts in @c chrono.
 *
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

#include <cstdint>
#include <deque>
#include <string>

#include "DiagnosticReporter.hpp"
#include "StageStats.hpp"
#include "CounterStats.hpp"

namespace CodeListener::Core
{

/** Collects per-stage timing measurements, integer counters, and renders summary tables. */
class StatisticsReport
{
  public:
    /**
     * Return a mutable reference to the StageStats bucket for @p stage.
     *
     * If @p stage has not been seen before a new zero-initialised bucket is appended, preserving
     * insertion order in the final report.
     *
     * @param stage Human-readable stage identifier.
     *
     * @return Reference to the StageStats for the given stage name.
     */
    StageStats &getStage(const std::string &stage);

    /**
     * Return a mutable reference to the counter bucket for @p counter.
     *
     * If @p counter has not been seen before a new zero-initialised bucket is appended,
     * preserving insertion order in the final report.
     *
     * @param counter Human-readable counter identifier.
     *
     * @return Reference to the CounterStats for the given counter name.
     */
    CounterStats &getCounter(const std::string &counter);

    /**
     * Add one counter sample for @p counter.
     *
     * @param counter Human-readable counter identifier.
     * @param value Counter value to accumulate.
     */
    void recordCounter(const std::string &counter, uint64_t value);

    /**
     * Print formatted timing and counter tables through @p reporter at Info level.
     *
     * @param reporter Diagnostic sink to write the table to.
     */
    void print(DiagnosticReporter &reporter) const;

  private:
    /** Stages in insertion order. */
    std::deque<std::pair<std::string, StageStats>> stages;

    /** Counters in insertion order. */
    std::deque<std::pair<std::string, CounterStats>> counters;
};

} // namespace CodeListener::Core
