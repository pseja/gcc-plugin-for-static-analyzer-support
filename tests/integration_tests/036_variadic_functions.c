/**
 * @file 036_variadic_functions.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Tests that variadic functions are correctly identified in the CodeModel. A variadic function type must have
 * is_variadic == true. A non-variadic function type must have is_variadic == false.
 * @date 2026-04-14
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
#include <stdarg.h>

// variadic: sum of 'count' integers
int sum(int count, ...)
{
    va_list args;
    va_start(args, count);
    int total = 0;
    for (int i = 0; i < count; i++)
    {
        total += va_arg(args, int);
    }
    va_end(args);
    return total;
}

// non-variadic: plain two-argument function
int add(int a, int b)
{
    return a + b;
}

int main()
{
    int s = sum(3, 10, 20, 30);
    int r = add(1, 2);
    return s + r;
}
