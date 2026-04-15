/**
 * @file 035_callgraph.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Verifies function call graph analysis by calling multiple functions.
 * @date 2026-03-24
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

int foo(int x)
{
    return x * 2;
}

int bar(int y)
{
    return foo(y) + 1;
}

int baz(int z)
{
    return bar(z) - foo(z);
}

int main()
{
    int a = foo(10);
    int b = baz(20);

    return a + b;
}
