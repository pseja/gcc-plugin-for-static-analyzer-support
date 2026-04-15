/**
 * @file 034_inline_asm_simple.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Tests that simple inline assembly blocks are represented.
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
int main()
{
    int result = 0;
    __asm__ __volatile__("nop" : "=r"(result));

    return result;
}
