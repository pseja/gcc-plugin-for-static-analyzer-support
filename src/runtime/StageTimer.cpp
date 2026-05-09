/**
 * @file StageTimer.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the RAII stage timer.
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

#include "StageTimer.hpp"

namespace CodeListener::Core
{

StageTimer::StageTimer(bool enabled, StageStats &stats) noexcept
    : enabled(enabled), stats(stats),
      start(enabled ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{})
{
}

StageTimer::~StageTimer()
{
    if (enabled)
    {
        const auto ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start).count();
        stats.record(static_cast<uint64_t>(ns));
    }
}

} // namespace CodeListener::Core
