/**
 * @file 037_anonymous_structs.c
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Tests that anonymous struct and union types are given synthesised names derived from their source location
 * rather than left as empty strings. This exercises the anonymous type naming fix in GCCAdapter::getOrCreateType.
 * anonymous struct (no tag, no typedef) used directly
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
struct
{
    int x;
    int y;
} point;

// anonymous union inside a named struct
struct Wrapper
{
    union {
        int as_int;
        float as_float;
    } value;
};

int main()
{
    point.x = 1;
    point.y = 2;

    struct Wrapper w;
    w.value.as_int = 42;

    return point.x + w.value.as_int;
}
