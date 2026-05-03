/**
 * @file StorageDuration.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Defines storage-duration categories for variables.
 * @date 2026-05-02
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

#include <string_view>

namespace CodeListener::Core
{

/** Describes the storage duration recorded for a variable. */
enum class StorageDuration
{
    AUTO,         /**< Automatic storage tied to block or function lifetime. */
    STATIC,       /**< Static storage lasting for the whole program execution. */
    EXTERN,       /**< External declaration resolved in another translation unit. */
    REGISTER,     /**< Register-qualified automatic storage. */
    THREAD_LOCAL, /**< Thread-local storage with one instance per thread. */
};

/**
 * Convert a storage-duration value to its stable textual name.
 *
 * @param storage_duration Storage-duration value to stringify.
 *
 * @return Short symbolic name used in diagnostics and exports.
 */
constexpr std::string_view toString(StorageDuration storage_duration) noexcept
{
    switch (storage_duration)
    {
    case StorageDuration::AUTO:
        return "AUTO";
    case StorageDuration::STATIC:
        return "STATIC";
    case StorageDuration::EXTERN:
        return "EXTERN";
    case StorageDuration::REGISTER:
        return "REGISTER";
    case StorageDuration::THREAD_LOCAL:
        return "THREAD_LOCAL";
    default:
        return "UNKNOWN";
    }
}

} // namespace CodeListener::Core
