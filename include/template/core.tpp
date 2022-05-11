#ifndef CORE_TPP
#define CORE_TPP

#include "instruction_set.h"
#include "mem_access.h"
#include "specifications.h"

#include <cstdint>

namespace cpu {

template<SPECS_TEMPLATE> void core<CURRENT_SPECS>::run() {
    while (1) {
        execute();
    }
}

template<SPECS_TEMPLATE> void core<CURRENT_SPECS>::execute() {

    // Check for waiting interrupt
    if (state.waiting_interrupt) {
        // Service interrupt
    }

    // Check if halted
    if (state.halted) {
        return;
    }

    // Get next instruction
    opcode_type next_instruction =
        state.active_memory.template read_t<opcode_type>(
            state.instruction_pointer);

    // Increase instruction pointer
    state.instruction_pointer +=
        ((sizeof(opcode_type) / sizeof(word_size)) +
         (sizeof(opcode_type) % sizeof(word_size) ? 1 : 0));

    // Execute instruction
    isa.operate(next_instruction, state);
};

} // namespace cpu

#endif // CORE_TPP
