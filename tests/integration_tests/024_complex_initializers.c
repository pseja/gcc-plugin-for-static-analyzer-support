/**
 * @file 024_complex_initializers.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Verifies complex initialization patterns, such as aggregate initialization lists for arrays of structs.
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

struct vec2i
{
    int x;
    int y;
};

int main()
{
    struct vec2i arr[] = {{1, 2}, {3, 4}, {5, 6}};

    int a = arr[1].x;
    return a;
}
