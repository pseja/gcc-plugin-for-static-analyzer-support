/**
 * @file SwitchToIf.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the switch-to-if annotation.
 * @date 2026-05-02
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

#include <cstdlib>
#include <string>
#include <variant>
#include <vector>

#include "Block.hpp"
#include "CodeModel.hpp"
#include "CondInstruction.hpp"
#include "EnumType.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "IntegerType.hpp"
#include "Operand.hpp"
#include "OpCode.hpp"
#include "SwitchInstruction.hpp"
#include "SwitchToIf.hpp"
#include "Type.hpp"

namespace CodeListener::AnnotationServices
{

// Expands one SwitchInstruction's cases into a flat list of single-value equality checks, and resolves the default-case
// block name.
std::vector<SwitchToIf::FlatCheck> SwitchToIf::flattenSwitch(const Core::SwitchInstruction &sw,
                                                             const Core::CodeModel &model,
                                                             std::string &out_default_target)
{
    out_default_target.clear();
    std::vector<SwitchToIf::FlatCheck> flat;

    for (const auto &c : sw.cases)
    {
        if (!c.low_value.has_value())
        {
            // default case
            const auto *bb = model.getBlock(c.target_block_id);
            out_default_target = bb ? bb->name : "";
            continue;
        }

        const auto *target_bb = model.getBlock(c.target_block_id);
        std::string case_target = target_bb ? target_bb->name : "L_unknown";

        // determine signedness from the case constant's type
        bool is_unsigned = false;
        if (const auto *lo_cst = std::get_if<Core::ConstantOperand>(&*c.low_value))
        {
            const auto *tp = model.getType(lo_cst->type_id);
            if (tp)
            {
                if (const auto *it = std::get_if<Core::IntegerType>(&tp->data))
                {
                    is_unsigned = it->is_unsigned;
                }
                else if (const auto *et = std::get_if<Core::EnumType>(&tp->data))
                {
                    is_unsigned = et->is_unsigned;
                }
            }
        }

        // parse lo
        long lo_val = 0;
        if (const auto *lo_cst = std::get_if<Core::ConstantOperand>(&*c.low_value))
        {
            lo_val = is_unsigned ? static_cast<long>(std::strtoull(lo_cst->value.c_str(), nullptr, 10))
                                 : std::strtol(lo_cst->value.c_str(), nullptr, 10);
        }

        // parse hi (same as lo for single-value cases)
        long hi_val = lo_val;
        if (c.high_value.has_value())
        {
            if (const auto *hi_cst = std::get_if<Core::ConstantOperand>(&*c.high_value))
            {
                hi_val = is_unsigned ? static_cast<long>(std::strtoull(hi_cst->value.c_str(), nullptr, 10))
                                     : std::strtol(hi_cst->value.c_str(), nullptr, 10);
            }
        }

        // expand range into individual value checks
        for (long v = lo_val; v <= hi_val; ++v)
        {
            unsigned long uval = static_cast<unsigned long>(v);
            std::string val_str = std::to_string(uval);
            if (is_unsigned)
            {
                val_str += "U";
            }
            if (v < 0)
            {
                val_str = "(" + val_str + ")";
            }
            flat.push_back({std::move(val_str), case_target});
        }
    }

    return flat;
}

SwitchToIf SwitchToIf::build(const Core::CodeModel &model)
{
    SwitchToIf result;

    // switch_counter drives %rGswN / %rGcondN names
    // global across the entire model
    int switch_counter = 0;

    for (const auto &func : model.functions())
    {
        // synth_label_counter drives L1000000, L1000001, ...
        // reset per function
        int synth_label_counter = 0;

        for (Core::BlockId block_id : func.block_ids)
        {
            const Core::Block *block = model.getBlock(block_id);
            if (!block || block->name == "ENTRY" || block->name == "EXIT")
            {
                continue;
            }

            for (Core::InstructionId inst_id : block->instruction_ids)
            {
                const Core::Instruction *inst = model.getInstruction(inst_id);
                if (!inst)
                {
                    continue;
                }

                if (const auto *cond = std::get_if<Core::CondInstruction>(&inst->data))
                {
                    if (cond->opcode != Core::OpCode::NONE)
                    {
                        result.conds[inst_id] = {"%rGcond" + std::to_string(switch_counter++)};
                    }
                }
                else if (const auto *sw = std::get_if<Core::SwitchInstruction>(&inst->data))
                {
                    std::string default_target_block_name;
                    auto checks = flattenSwitch(*sw, model, default_target_block_name);
                    int base = 1000000 + synth_label_counter;
                    synth_label_counter += static_cast<int>(checks.size());
                    result.switches[inst_id] = {"%rGsw" + std::to_string(switch_counter++),
                                                std::move(default_target_block_name), std::move(checks), base};
                }
            }
        }
    }

    return result;
}

} // namespace CodeListener::AnnotationServices
