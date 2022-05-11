#ifndef SPECIFICATIONS_H
#define SPECIFICATIONS_H

#include <cstdint>

#define SPECS_TEMPLATE                                          \
    class register_type, class address_type, class opcode_type, \
        class word_size, const unsigned int num_gen_registers

#define CURRENT_SPECS \
    register_type, address_type, opcode_type, word_size, num_gen_registers

#endif // SPECIFICATIONS_H
