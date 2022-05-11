#ifndef CPU_STATE_H
#define CPU_STATE_H

#include "mem_access.h"
#include "specifications.h"

#include <cstdint>
#include <functional>

namespace cpu {

template<SPECS_TEMPLATE> class cpu_state {
  public:
    cpu_state<CURRENT_SPECS>(memory<CURRENT_SPECS>& target_memory) :
        active_memory(target_memory) {}

    register_type general_registers[num_gen_registers];

    address_type  instruction_pointer;
    register_type status_register;

    memory<CURRENT_SPECS>& active_memory;

    bool    waiting_interrupt = false;
    uint8_t waiting_vector;
    uint8_t waiting_priority;

    bool halted = true;

    register_type supervisor_stack_pointer = 0x200;

    std::unordered_map<uint8_t,
                       std::function<void(cpu::cpu_state<CURRENT_SPECS>&)>>
        trap_overrides;
};

} // namespace cpu

#endif
