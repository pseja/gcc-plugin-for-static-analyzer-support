/**
 * @file 023_bitfields.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Tests handling of bit-fields within structures and bit-masking operations in IR.
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

struct Flags
{
    unsigned int is_ready : 1;
    unsigned int status_code : 3;
    unsigned int : 4;
    unsigned int mode : 8;
};

int main()
{
    struct Flags f;
    f.is_ready = 1;
    f.status_code = 5;
    f.mode = 255;

    return f.status_code;
}
