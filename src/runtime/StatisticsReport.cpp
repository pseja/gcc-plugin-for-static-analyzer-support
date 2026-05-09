/**
 * @file StatisticsReport.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the stage-timing and counter report accumulator and formatted printer.
 * @date 2026-05-08
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

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>

#include "StatisticsReport.hpp"

namespace CodeListener::Core
{

namespace
{

std::string formatDecimal(double value)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(3) << value;
    return out.str();
}

std::string alignCell(const std::string &value, size_t width, bool left_align)
{
    if (value.size() >= width)
    {
        return value;
    }

    const size_t padding = width - value.size();
    if (left_align)
    {
        return value + std::string(padding, ' ');
    }

    return std::string(padding, ' ') + value;
}

std::string makeTitleBorder(const std::vector<size_t> &widths)
{
    const size_t inner_width = std::accumulate(widths.begin(), widths.end(), size_t{0}) + 3 * widths.size() - 1;

    std::ostringstream out;
    out << '+' << std::string(inner_width, '-') << '+';
    return out.str();
}

std::string makeBorder(const std::vector<size_t> &widths)
{
    std::ostringstream out;
    out << '+';
    for (size_t width : widths)
    {
        out << std::string(width + 2, '-') << '+';
    }
    return out.str();
}

std::string makeTitleRow(const std::string &title, const std::vector<size_t> &widths)
{
    const size_t inner_width = std::accumulate(widths.begin(), widths.end(), size_t{0}) + 3 * widths.size() - 1;
    const size_t total_padding = inner_width > title.size() ? inner_width - title.size() : 0;
    const size_t left_padding = total_padding / 2;
    const size_t right_padding = total_padding - left_padding;

    std::ostringstream out;
    out << '|' << std::string(left_padding, ' ') << title << std::string(right_padding, ' ') << '|';
    return out.str();
}

std::string makeRow(const std::vector<std::string> &cells, const std::vector<size_t> &widths,
                    const std::vector<bool> &left_align)
{
    std::ostringstream out;
    out << '|';
    for (size_t i = 0; i < cells.size(); i++)
    {
        out << ' ' << alignCell(cells[i], widths[i], left_align[i]) << ' ' << '|';
    }
    return out.str();
}

void printTable(DiagnosticReporter &reporter, const SourceLocation &loc, const std::string &title,
                const std::vector<std::string> &headers, const std::vector<bool> &header_align,
                const std::vector<std::vector<std::string>> &rows, const std::vector<bool> &row_align)
{
    std::vector<size_t> widths(headers.size(), 0);
    for (size_t i = 0; i < headers.size(); i++)
    {
        widths[i] = headers[i].size();
    }

    for (const auto &row : rows)
    {
        for (size_t i = 0; i < row.size(); i++)
        {
            widths[i] = std::max(widths[i], row[i].size());
        }
    }

    const std::string border = makeBorder(widths);
    const std::string title_border = makeTitleBorder(widths);

    reporter.report(DiagnosticLevel::Info, loc, title_border);
    reporter.report(DiagnosticLevel::Info, loc, makeTitleRow(title, widths));
    reporter.report(DiagnosticLevel::Info, loc, border);
    reporter.report(DiagnosticLevel::Info, loc, makeRow(headers, widths, header_align));
    reporter.report(DiagnosticLevel::Info, loc, border);
    for (const auto &row : rows)
    {
        reporter.report(DiagnosticLevel::Info, loc, makeRow(row, widths, row_align));
    }
    reporter.report(DiagnosticLevel::Info, loc, border);
}

std::vector<std::string> formatTimingRow(const std::string &name, const StageStats &stats)
{
    return {name, std::to_string(stats.count), formatDecimal(stats.totalMs()), formatDecimal(stats.meanMs())};
}

std::vector<std::string> formatCounterRow(const std::string &name, const CounterStats &stats)
{
    return {name, std::to_string(stats.count), std::to_string(stats.total), formatDecimal(stats.mean())};
}

} // namespace

StageStats &StatisticsReport::getStage(const std::string &stage)
{
    for (auto &[name, s] : stages)
    {
        if (name == stage)
        {
            return s;
        }
    }
    stages.emplace_back(stage, StageStats{});
    return stages.back().second;
}

CounterStats &StatisticsReport::getCounter(const std::string &counter)
{
    for (auto &[name, c] : counters)
    {
        if (name == counter)
        {
            return c;
        }
    }
    counters.emplace_back(counter, CounterStats{});
    return counters.back().second;
}

void StatisticsReport::recordCounter(const std::string &counter, uint64_t value)
{
    getCounter(counter).record(value);
}

void StatisticsReport::print(DiagnosticReporter &reporter) const
{
    // don't attach a source-code snippet to each row
    const SourceLocation no_loc{};

    std::vector<std::vector<std::string>> timing_rows;

    for (const auto &[name, s] : stages)
    {
        if (s.count == 0)
        {
            continue;
        }

        timing_rows.push_back(formatTimingRow(name, s));
    }

    printTable(reporter, no_loc, "Timing Summary", {"Stage", "Calls", "Total (ms)", "Mean (ms)"},
               {true, true, true, true}, timing_rows, {true, false, false, false});

    bool have_counters = false;
    for (const auto &[_, c] : counters)
    {
        if (c.count != 0)
        {
            have_counters = true;
            break;
        }
    }

    if (!have_counters)
    {
        return;
    }

    std::vector<std::vector<std::string>> counter_rows;

    for (const auto &[name, c] : counters)
    {
        if (c.count == 0)
        {
            continue;
        }

        counter_rows.push_back(formatCounterRow(name, c));
    }

    printTable(reporter, no_loc, "Counter Summary", {"Counter", "Samples", "Total", "Mean"}, {true, true, true, true},
               counter_rows, {true, false, false, false});
}

} // namespace CodeListener::Core
