/**
 * @file CounterStats.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the CounterStats structure for accumulating integer-valued counters.
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

/** Accumulates integer-valued counters and tracks how many samples contributed to them. */
struct CounterStats
{
    /** Number of samples that contributed to this counter. */
    uint64_t count{0};

    /** Total accumulated counter value across all samples. */
    uint64_t total{0};

    /**
     * Record one additional sample with value @p value.
     *
     * @param value Counter value of this sample.
     */
    void record(uint64_t value) noexcept;

    /**
     * Mean counter value per recorded sample, or 0 if never recorded.
     *
     * @return Arithmetic mean over all recorded samples, or 0.0 if count is zero.
     */
    [[nodiscard]] double mean() const noexcept;
};

} // namespace CodeListener::Core
