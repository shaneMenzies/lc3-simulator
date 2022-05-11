#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include "cpu_state.h"

#include <functional>
#include <unordered_map>

namespace cpu {

template<SPECS_TEMPLATE> class instruction_set {
  public:
    instruction_set<CURRENT_SPECS>(
        const opcode_type opcodes[],
        const std::function<void(opcode_type, cpu_state<CURRENT_SPECS>&)>
                     definitions[],
        unsigned int size, const opcode_type opcode_mask);

    void operate(opcode_type               instruction,
                 cpu_state<CURRENT_SPECS>& current_state);

    const opcode_type opcode_mask;
    std::unordered_map<
        opcode_type,
        std::function<void(opcode_type, cpu_state<CURRENT_SPECS>&)>>
        operations;
};

} // namespace cpu

#include "template/instruction_set.tpp"

#endif // INSTRUCTION_SET_H
