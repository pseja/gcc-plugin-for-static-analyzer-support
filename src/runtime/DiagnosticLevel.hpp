/**
 * @file DiagnosticLevel.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Defines severity levels used by diagnostic reporters.
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

namespace CodeListener::Core
{

/** Severity level attached to one diagnostic message. */
enum class DiagnosticLevel
{
    Debug,   /**< Verbose debugging information. */
    Info,    /**< Informational message for normal operation. */
    Warning, /**< Recoverable issue or suspicious condition. */
    Error,   /**< Non-fatal error that should be surfaced to the user. */
    Fatal    /**< Unrecoverable error that aborts the current operation. */
};

} // namespace CodeListener::Core
