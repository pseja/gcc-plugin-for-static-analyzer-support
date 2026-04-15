/**
 * @file 033_void_pointers.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Verifies usage of void* for generic pointers and casting between pointer types.
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
    int val = 42;
    void *generic_ptr = &val;

    int *int_ptr = (int *)generic_ptr;

    return *int_ptr;
}
