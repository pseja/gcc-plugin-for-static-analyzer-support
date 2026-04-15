/**
 * @file 029_short_circuit_logic.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Verifies correct control flow generation for short-circuit evaluation of logical && and ||.
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

int check(int val)
{
    return val > 0;
}

int main()
{
    int a = 1;
    int b = -1;

    // GIMPLE lowers this into multiple basic blocks!
    if (a > 0 && check(b) || a == 5)
    {
        return 1;
    }

    return 0;
}
