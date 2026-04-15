/**
 * @file 028_noreturn_abort.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Checks handling of noreturn functions (like abort) and unreachable code markers.
 * @date 2026-03-20
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

int main()
{
    int x = 10;
    if (x == 10)
    {
        __builtin_trap(); // forces ABORT instruction
    }

    __builtin_unreachable(); // forces UNREACHABLE instruction
}
