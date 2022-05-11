/**
 * @file main.cpp
 * @author Shane Menzies
 * @brief LC-3 Simulator initialization and main executable routine
 * @date 04/24/22
 *
 *
 */

#include "core.h"
#include "standard_lc3.h"

int main(int argc, char* argv[]) {

    standard_lc3::standard_lc3_core lc3;

    if (argc > 1) {
        std::ifstream target(argv[1]);
        lc3.load_lc3t_obj(target);
        lc3.state.halted = false;

        lc3.run();
    }

    return 0;
}