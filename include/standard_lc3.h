#ifndef STANDARD_LC3_H
#define STANDARD_LC3_H

#include "core.h"
#include "specifications.h"

#include <cstdint>

namespace standard_lc3 {

#define STANDARD_LC3_SPECS int16_t, uint16_t, uint16_t, int16_t, 8

constexpr uint16_t default_opcode_mask = 0b1111'0000'0000'0000;
class standard_lc3_isa : public cpu::instruction_set<STANDARD_LC3_SPECS> {
  public:
    standard_lc3_isa() :
        cpu::instruction_set<STANDARD_LC3_SPECS>(
            default_opcodes, default_operations, 16, default_opcode_mask) {}

  private:
    static const uint16_t default_opcodes[16];
    static const std::function<void(uint16_t,
                                    cpu::cpu_state<STANDARD_LC3_SPECS>&)>
        default_operations[16];
};

class standard_lc3_memory : public memory<STANDARD_LC3_SPECS> {
  public:
    standard_lc3_memory() { regions.insert(ram); }

  private:
    // Addresses 0x0 - 0xfdff are ram
    region_descriptor<uint16_t, int16_t> ram =
        region_descriptor<uint16_t, int16_t>(
            0, 0xfdff, new ram_region<uint16_t, int16_t>(0xfe00));

    // Addresses 0xfe00 - 0xffff are for device i/o
};

namespace overrides {
void getc(cpu::cpu_state<STANDARD_LC3_SPECS>& state);
void out(cpu::cpu_state<STANDARD_LC3_SPECS>& state);
void puts(cpu::cpu_state<STANDARD_LC3_SPECS>& state);
void in(cpu::cpu_state<STANDARD_LC3_SPECS>& state);
void putsp(cpu::cpu_state<STANDARD_LC3_SPECS>& state);
void halt(cpu::cpu_state<STANDARD_LC3_SPECS>& state);
void gets(cpu::cpu_state<STANDARD_LC3_SPECS>& state);
} // namespace overrides
const std::pair<uint8_t,
                std::function<void(cpu::cpu_state<STANDARD_LC3_SPECS>&)>>
    default_overrides[] = {std::make_pair(0x20, overrides::getc),
                           std::make_pair(0x21, overrides::out),
                           std::make_pair(0x22, overrides::puts),
                           std::make_pair(0x23, overrides::in),
                           std::make_pair(0x24, overrides::putsp),
                           std::make_pair(0x25, overrides::halt),
                           std::make_pair(0x40, overrides::gets)};

class standard_lc3_core :
    public cpu::core<int16_t, uint16_t, uint16_t, int16_t, 8> {
  public:
    standard_lc3_core() :
        cpu::core<int16_t, uint16_t, uint16_t, int16_t, 8>(default_memory,
                                                           default_isa) {
        state.status_register = 0b111;
        for (std::pair<uint8_t,
                       std::function<void(cpu::cpu_state<STANDARD_LC3_SPECS>&)>>
                 next_value : default_overrides) {
            state.trap_overrides.insert(next_value);
        }
    }

    void load_lc3t_obj(std::ifstream& file);

    standard_lc3_memory default_memory;
    standard_lc3_isa    default_isa;
};

} // namespace standard_lc3

#endif // STANDARD_LC3_H
