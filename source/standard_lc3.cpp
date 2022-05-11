/**
 * @file standard_lc3.cpp
 * @author Shane Menzies
 * @brief Standard LC3 core, memory, and isa
 * @date 05/02/22
 *
 *
 */

#include "standard_lc3.h"

namespace standard_lc3 {

void standard_lc3_core::load_lc3t_obj(std::ifstream& file) {
    // lc3tools' lc3as creates an object with the first 2 bytes being the start
    // address, and in a big-endian format

    uint16_t address;
    file.read((char*)&address, 2);
    address = ((address & 0xff00) >> 8) | ((address & 0xff) << 8);
    state.instruction_pointer = address;

    state.active_memory.load_raw(address, file);
}

const uint16_t standard_lc3_isa::default_opcodes[16] = {
    // ADD-DR -SR -Switch(SR2/IMM5)
    0b0001'000'000'0'00000,

    // AND-DR -SR -Switch(SR2/IMM5)
    0b0101'000'000'0'00000,

    // BR -n-z-p-PCoffset9
    0b0000'0'0'0'000000000,

    // JMP -  -Base
    0b1100'000'000'000000,

    // JSR - Switch(PCO/BaseR)
    0b0100'0'00000000000,

    // LD -DR - PCoffset
    0b0010'000'000000000,

    // LDI-DR - PCoffset
    0b1010'000'000000000,

    // LDR-DR -Base-offset
    0b0110'000'000'000000,

    // LEA-DR - PCoffset
    0b1110'000'000000000,

    // NOT-DR -SR
    0b1001'000'000'0'0'0000,

    // RTI
    0b1000'000000000000,

    // ST -SR - PCoffset
    0b0011'000'000000000,

    // STI-SR - PCoffset
    0b1011'000'000000000,

    // STR-SR -Base-offset
    0b0111'000'000'000000,

    // TRAP -  -vector8
    0b1111'0000'0000'0000,

    // Reserved
    0b1101'0000'0000'0000

};

void update_conds(int16_t source, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {
    state.status_register &= ~(0b111);

    if (source > 0) {
        state.status_register |= 0b001;
    } else if (source < 0) {
        state.status_register |= 0b100;
    } else {
        state.status_register |= 0b010;
    }
}

void ADD(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    int16_t& DR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];
    int16_t& SR1 =
        state.general_registers[(opcode & 0b0000'000'111'0'00000) >> 6];

    // Two different types of ADD
    if (opcode & 0b0000'000'000'1'00000) {
        struct {
            signed int x : 5;
        } s;
        int16_t imm = s.x = opcode & 0b1'1111;

        DR = (SR1 + imm);

#ifdef DEBUG
        std::cout << std::hex << state.instruction_pointer << ": " << opcode
                  << " = ADD ( R" << ((opcode & 0b111'000'0'00000) >> 9)
                  << " = (R" << ((opcode & 0b0000'000'111'0'00000) >> 6)
                  << " + " << (imm) << ")\n";
#endif
    } else {
        int16_t& SR2 =
            state.general_registers[(opcode & 0b0000'000'000'0'00'111)];
        DR = (SR1 + SR2);

#ifdef DEBUG
        std::cout << std::hex << state.instruction_pointer << ": " << opcode
                  << " = ADD ( R" << ((opcode & 0b111'000'0'00000) >> 9)
                  << " = (R" << ((opcode & 0b0000'000'111'0'00000) >> 6)
                  << " + R" << ((opcode & 0b111)) << ")\n";
#endif
    }

    update_conds(DR, state);
}

void AND(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    int16_t& DR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];
    int16_t& SR1 =
        state.general_registers[(opcode & 0b0000'000'111'0'00000) >> 6];

    // Two different types of ADD
    if (opcode & 0b0000'000'000'1'00000) {
        int16_t imm = (opcode & 0b0000'000'000'0'11111);

        DR = (SR1 & imm);

#ifdef DEBUG
        std::cout << std::hex << state.instruction_pointer << ": " << opcode
                  << " = AND ( R" << ((opcode & 0b111'000'0'00000) >> 9)
                  << " = (R" << ((opcode & 0b0000'000'111'0'00000) >> 6)
                  << " & " << (imm) << ")\n";
#endif
    } else {
        int16_t& SR2 =
            state.general_registers[(opcode & 0b0000'000'000'0'00'111)];
        DR = (SR1 & SR2);

#ifdef DEBUG
        std::cout << std::hex << state.instruction_pointer << ": " << opcode
                  << " = AND ( R" << ((opcode & 0b111'000'0'00000) >> 9)
                  << " = (R" << ((opcode & 0b0000'000'111'0'00000) >> 6)
                  << " & R" << ((opcode & 0b111)) << ")\n";
#endif
    }

    update_conds(DR, state);
}

void BR(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = BR";
    if (opcode & 0b100'000000000)
        std::cout << 'n';
    if (opcode & 0b010'000000000)
        std::cout << 'z';
    if (opcode & 0b001'000000000)
        std::cout << 'p';
#endif

    if (((opcode & 0b0000'111'0'0000'0000) >> 9) &
        (state.status_register & 0b111)) {
        struct {
            signed int x : 9;
        } s;
        int16_t offset = s.x = opcode & 0b1'1111'1111;

        state.instruction_pointer += offset;

#ifdef DEBUG
        std::cout << std::hex << "-> " << state.instruction_pointer;
#endif
    }

#ifdef DEBUG
    std::cout << '\n';
#endif
}

void JMP(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    int16_t& BaseR =
        state.general_registers[(opcode & 0b0000'000'111'000000) >> 6];
    state.instruction_pointer = BaseR;

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = JMP R" << ((opcode & 0b0000'000'111'000000) >> 6) << "\n";
#endif
}

void JSR(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    state.general_registers[7] = state.instruction_pointer;

    if (opcode & 0b0000'1'00000000000) {
#ifdef DEBUG
        std::cout << std::hex << state.instruction_pointer << ": " << opcode
                  << " = JSR -> ";
#endif

        // JSR
        struct {
            signed int x : 11;
        } s;
        int16_t offset = s.x = opcode & 0b111'1111'1111;

        state.instruction_pointer += offset;

#ifdef DEBUG
        std::cout << std::hex << state.instruction_pointer << '\n';
#endif

    } else {

#ifdef DEBUG
        std::cout << std::hex << state.instruction_pointer << ": " << opcode
                  << " = JSRR -> ";
#endif
        // JSRR
        int16_t& BaseR =
            state.general_registers[(opcode & 0b0000'000'111'000000) >> 6];
        state.instruction_pointer += BaseR;

#ifdef DEBUG
        std::cout << std::hex << state.instruction_pointer << '\n';
#endif
    }
}

void LD(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    struct {
        signed int x : 9;
    } s;
    int16_t  offset = s.x = opcode & 0b1'1111'1111;
    int16_t& DR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];

    DR = state.active_memory.read(state.instruction_pointer + offset);

    update_conds(DR, state);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = LD R" << ((opcode & 0b0000'111'000'0'00000) >> 9) << " <- "
              << (state.instruction_pointer + offset) << '\n';
#endif
}

void LDI(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    struct {
        signed int x : 9;
    } s;
    int16_t  offset = s.x = opcode & 0b1'1111'1111;
    int16_t& DR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];

    DR = state.active_memory.read(
        state.active_memory.read(state.instruction_pointer + offset));

    update_conds(DR, state);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = LDI R" << ((opcode & 0b0000'111'000'0'00000) >> 9)
              << " <- "
              << state.active_memory.read(state.instruction_pointer + offset)
              << '\n';
#endif
}

void LDR(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    struct {
        signed int x : 6;
    } s;
    int16_t  offset = s.x = opcode & 0b011'1111;
    int16_t& DR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];
    int16_t& BaseR =
        state.general_registers[(opcode & 0b0000'000'111'0'00000) >> 6];

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = LDR R" << ((opcode & 0b0000'111'000'0'00000) >> 9)
              << " <- " << (BaseR + offset) << '\n';
#endif

    DR = state.active_memory.read(BaseR + offset);

    update_conds(DR, state);
}

void LEA(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    struct {
        signed int x : 9;
    } s;
    int16_t  offset = s.x = opcode & 0b1'1111'1111;
    int16_t& DR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];

    DR = state.instruction_pointer + offset;

    update_conds(DR, state);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = LEA R" << ((opcode & 0b0000'111'000'0'00000) >> 9) << " = "
              << (state.instruction_pointer + offset) << '\n';
#endif
}

void NOT(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    int16_t& DR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];
    int16_t& SR =
        state.general_registers[(opcode & 0b0000'000'111'0'00000) >> 6];

    DR = ~(SR);

    update_conds(DR, state);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = NOT ( R" << ((opcode & 0b0000'111'000'0'00000) >> 9)
              << " = ~R" << ((opcode & 0b0000'000'111'0'00000) >> 6) << '\n';
#endif
}

void RTI(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {
    // TODO

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = RTI \n";
#endif
}

void ST(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    struct {
        signed int x : 9;
    } s;
    int16_t  offset = s.x = opcode & 0b1'1111'1111;
    int16_t& SR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];

    state.active_memory.write(state.instruction_pointer + offset, SR);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = ST R" << ((opcode & 0b0000'111'000'0'00000) >> 9) << " -> "
              << (state.instruction_pointer + offset) << '\n';
#endif
}

void STI(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    struct {
        signed int x : 9;
    } s;
    int16_t  offset = s.x = opcode & 0b1'1111'1111;
    int16_t& SR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];

    state.active_memory.write(
        state.active_memory.read(state.instruction_pointer + offset), SR);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = STI R" << ((opcode & 0b0000'111'000'0'00000) >> 9)
              << " -> "
              << state.active_memory.read(state.instruction_pointer + offset)
              << '\n';
#endif
}

void STR(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    struct {
        signed int x : 6;
    } s;
    int16_t  offset = s.x = opcode & 0b11'1111;
    int16_t& SR =
        state.general_registers[(opcode & 0b0000'111'000'0'00000) >> 9];
    int16_t& BaseR =
        state.general_registers[(opcode & 0b0000'000'111'0'00000) >> 6];

    state.active_memory.write(BaseR + offset, SR);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = STR R" << ((opcode & 0b0000'111'000'0'00000) >> 9)
              << " -> " << (BaseR + offset) << '\n';
#endif
}

void TRAP(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    uint8_t vector = (opcode & 0b00000000'1111'1111);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = TRAP 0x" << (unsigned int)vector << '\n';
#endif

    state.general_registers[7] = state.instruction_pointer;

    auto override_check = state.trap_overrides.find(vector);
    if (override_check != state.trap_overrides.end()) {
        override_check->second(state);
    } else {
        state.instruction_pointer = state.active_memory.read(vector);
    }
}

void Reserved(uint16_t opcode, cpu::cpu_state<STANDARD_LC3_SPECS>& state) {
    (void)(opcode);
    (void)(state);

#ifdef DEBUG
    std::cout << std::hex << state.instruction_pointer << ": " << opcode
              << " = Reserved Opcode! \n";
#endif
}

const std::function<void(uint16_t, cpu::cpu_state<STANDARD_LC3_SPECS>&)>
    standard_lc3_isa::default_operations[16] = {
        ADD, AND, BR,  JMP, JSR, LD,  LDI,  LDR,
        LEA, NOT, RTI, ST,  STI, STR, TRAP, Reserved};

namespace overrides {
void getc(cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    char buffer;
    std::cin >> buffer;
    state.general_registers[0] = buffer;
}
void out(cpu::cpu_state<STANDARD_LC3_SPECS>& state) {
    std::cout << (char)(state.general_registers[0] & 0xff);
}
void puts(cpu::cpu_state<STANDARD_LC3_SPECS>& state) {
    uint16_t address = state.general_registers[0];

    uint16_t next_word = state.active_memory.read(address);

    while (next_word != 0) {
        std::cout << (char)(next_word & 0xff);
        address++;
        next_word = state.active_memory.read(address);
    }
}
void in(cpu::cpu_state<STANDARD_LC3_SPECS>& state) {

    std::cout << '>';

    char buffer;
    std::cin >> buffer;
    state.general_registers[0] = buffer;

    std::cout << '\n';
}
void putsp(cpu::cpu_state<STANDARD_LC3_SPECS>& state) {
    uint16_t address = state.general_registers[0];

    uint16_t next_word = state.active_memory.read(address);

    while (next_word != 0) {
        std::cout << (char)((next_word & 0xff00) >> 8)
                  << (char)(next_word & 0xff);
        address++;
        next_word = state.active_memory.read(address);
    }
}
void halt(cpu::cpu_state<STANDARD_LC3_SPECS>& state) {
    state.halted = true;

    std::cout << "Processor Halted \n";
    std::exit(0);
}
void gets(cpu::cpu_state<STANDARD_LC3_SPECS>& state) {
    // Get line of input from cin
    std::string buffer = "";
    std::cin >> buffer;

    // Copy into memory
    uint16_t address = state.general_registers[0];
    for (unsigned int i = 0; i < buffer.size(); i++, address++) {
        char data = buffer[i];
        state.active_memory.write(address, data);
    }
    state.active_memory.write(address, 0);
}
} // namespace overrides

} // namespace standard_lc3