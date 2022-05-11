#ifndef CORE_H
#define CORE_H

#include "instruction_set.h"
#include "mem_access.h"
#include "specifications.h"

#include <cstdint>

namespace cpu {

template<SPECS_TEMPLATE> class core {

  public:
    core(memory<CURRENT_SPECS>&          core_memory,
         instruction_set<CURRENT_SPECS>& target_isa) :
        state(core_memory),
        isa(target_isa) {}

    void run();

    void execute();

    cpu_state<CURRENT_SPECS>        state;
    instruction_set<CURRENT_SPECS>& isa;
};

} // namespace cpu

#include "template/core.tpp"

#endif // CORE_H
