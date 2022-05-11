#ifndef INSTRUCTION_SET_TPP
#define INSTRUCTION_SET_TPP

#include "cpu_state.h"

#include <functional>
#include <unordered_map>

namespace cpu {

template<SPECS_TEMPLATE>
instruction_set<CURRENT_SPECS>::instruction_set(
    const opcode_type opcodes[],
    const std::function<void(opcode_type, cpu_state<CURRENT_SPECS>&)>
                 definitions[],
    unsigned int size, const opcode_type opcode_mask) :
    opcode_mask(opcode_mask) {
    for (unsigned int i = 0; i < size; i++) {
        operations.insert(std::make_pair(opcodes[i], definitions[i]));
    }
    operations.rehash(size);
}

template<SPECS_TEMPLATE>
void instruction_set<CURRENT_SPECS>::operate(
    opcode_type instruction, cpu_state<CURRENT_SPECS>& current_state) {
    auto target_op = operations.find(instruction & opcode_mask);
    if (target_op != operations.end()) {
        target_op->second(instruction, current_state);
    }
}

} // namespace cpu

#endif // INSTRUCTION_SET_TPP
