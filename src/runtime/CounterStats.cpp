/**
 * @file CounterStats.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the CounterStats structure for accumulating integer-valued counters.
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

#include "CounterStats.hpp"

namespace CodeListener::Core
{

void CounterStats::record(uint64_t value) noexcept
{
    count++;
    total += value;
}

double CounterStats::mean() const noexcept
{
    return count ? static_cast<double>(total) / static_cast<double>(count) : 0.0;
}

} // namespace CodeListener::Core
