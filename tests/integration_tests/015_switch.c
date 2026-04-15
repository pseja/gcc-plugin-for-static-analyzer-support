/**
 * @file 015_switch.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Tests multi-way branching generation for switch statements and case labels.
 * @date 2026-03-06
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
    int x = 42;

    switch (x)
    {
    case 0:
        x += 1;
        break;
    case 1:
        x += 2;
        break;
    case 2 ... 41:
        x += 3;
        break;
    default:
        x += 4;
        break;
    }

    return x;
}
