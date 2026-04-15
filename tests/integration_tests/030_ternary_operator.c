/**
 * @file 030_ternary_operator.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Tests the ternary conditional operator and its translation to conditional branches or specific instructions.
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
    int a = 10;
    int b = 20;

    // MAX is a ternary operator in GIMPLE, so this should be represented as a single instruction with opcode MAX
    int max = (a > b) ? a : b;

    return max;
}
