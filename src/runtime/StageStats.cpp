/**
 * @file StageStats.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the StageStats structure for accumulating timing statistics.
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

#include "StageStats.hpp"

namespace CodeListener::Core
{

void StageStats::record(uint64_t ns) noexcept
{
    count++;
    total_ns += ns;
}

double StageStats::totalMs() const noexcept
{
    return static_cast<double>(total_ns) / 1'000'000.0;
}

double StageStats::meanMs() const noexcept
{
    return count ? static_cast<double>(total_ns) / static_cast<double>(count) / 1'000'000.0 : 0.0;
}

} // namespace CodeListener::Core
