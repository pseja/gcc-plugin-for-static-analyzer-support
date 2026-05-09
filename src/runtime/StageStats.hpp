/**
 * @file StageStats.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the StageStats structure for accumulating timing statistics.
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

namespace CodeListener::Core
{

/** Accumulates the total invocation count and elapsed nanoseconds for one named pipeline stage. */
struct StageStats
{
    /** Number of times this stage has been entered. */
    uint64_t count{0};

    /** Total elapsed time across all invocations, in nanoseconds. */
    uint64_t total_ns{0};

    /** Record one additional elapsed @p ns. */
    void record(uint64_t ns) noexcept;

    /** Total elapsed time in milliseconds. */
    [[nodiscard]] double totalMs() const noexcept;

    /** Mean elapsed time per invocation in milliseconds, or 0 if never called. */
    [[nodiscard]] double meanMs() const noexcept;
};

} // namespace CodeListener::Core
