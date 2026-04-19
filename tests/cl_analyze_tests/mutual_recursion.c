/**
 * @file mutual_recursion.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Simple even/odd mutual recursion used by cl_analyze_tests.
 * Taken from https://en.wikipedia.org/wiki/Mutual_recursion#Basic_examples.
 * @date 2026-04-18
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

int even(int n);
int odd(int n);

int even(int n)
{
    if (n == 0)
    {
        return 1;
    }

    return odd(n - 1);
}

int odd(int n)
{
    if (n == 0)
    {
        return 0;
    }

    return even(n - 1);
}

int main(void)
{
    return even(4) - 1;
}
