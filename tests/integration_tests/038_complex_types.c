/**
 * @file 038_complex_types.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Tests that C99 _Complex types are represented as COMPLEX kind.
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
double _Complex add_complex(double _Complex a, double _Complex b)
{
    return a + b;
}

int main()
{
    double _Complex x = 1.0 + 2.0i;
    double _Complex y = 3.0 + 4.0i;
    double _Complex z = add_complex(x, y);
    (void)z;
    return 0;
}
