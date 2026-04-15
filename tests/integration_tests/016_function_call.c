/**
 * @file 016_function_call.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Verifies function call semantics, argument passing, and return value handling.
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

float foo(int a, float b)
{
    return a * b;
}

int main()
{
    int x = 69;
    float y = 3.14;

    float result = foo(x, y);

    return 0;
}
