/**
 * @file 039_inline_asm.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Tests that inline assembly blocks are fully represented.
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
int add_one(int x)
{
    int result;
    // output: "=r"(result), input: "r"(x), clobbers: "cc"
    __asm__("addl $1, %1\n\t"
            "movl %1, %0"
            : "=r"(result)
            : "r"(x)
            : "cc");
    return result;
}

int main()
{
    // volatile asm with no operands (just a clobber)
    __asm__ __volatile__("nop" ::: "memory");

    return add_one(41);
}
