#ifndef MEMORY_ACCESS_TPP
#define MEMORY_ACCESS_TPP

#include "specifications.h"

#include <cstdint>
#include <fstream>
#include <functional>
#include <set>

template<class address_type, class word_size>
word_size ram_region<address_type, word_size>::read(address_type address) {
    return buffer[address];
}
template<class address_type, class word_size>
void ram_region<address_type, word_size>::write(address_type address,
                                                word_size    value) {
    buffer[address] = value;
}

template<class address_type, class word_size>
word_size custom_region<address_type, word_size>::read(address_type address) {
    return read_function(address);
}
template<class address_type, class word_size>
void custom_region<address_type, word_size>::write(address_type address,
                                                   word_size    value) {
    write_function(address, value);
}

template<class address_type, class word_size>
word_size region_descriptor<address_type, word_size>::read(
    address_type address) const {
    return region->read(address - start);
}
template<class address_type, class word_size>
void region_descriptor<address_type, word_size>::write(address_type address,
                                                       word_size value) const {
    region->write(address - start, value);
}

template<SPECS_TEMPLATE>
word_size memory<CURRENT_SPECS>::read(address_type address) {
    auto find_result = regions.template find<address_type>(address);

    if (find_result != regions.end()) {
        return find_result->read(address);
    } else {
        return 0;
    }
}

template<SPECS_TEMPLATE>
template<class T>
T memory<CURRENT_SPECS>::read_t(address_type address) {
    constexpr int needed_reads = (sizeof(T) / sizeof(word_size)) +
                                 (sizeof(T) % sizeof(word_size) ? 1 : 0);
    T result = 0;

    if constexpr (needed_reads == 1) {
        result = static_cast<T>(read(address));
    } else {
        for (int i = 0; i < needed_reads; i++) {
            result = result | (read(address) << (i * sizeof(word_size)));
            address++;
        }
    }
    return result;
}
template<SPECS_TEMPLATE>
void memory<CURRENT_SPECS>::write(address_type address, word_size value) {
    auto find_result = regions.find(address);

    if (find_result != regions.end()) {
        find_result->write(address, value);
    }
}

template<SPECS_TEMPLATE>
template<class T>
void memory<CURRENT_SPECS>::write_t(address_type address, T value) {
    constexpr int needed_writes = (sizeof(T) / sizeof(word_size)) +
                                  (sizeof(T) % sizeof(word_size) ? 1 : 0);

    if constexpr (needed_writes == 1) {
        write(address, static_cast<word_size>(value));
    } else {
        for (int i = 0; i < needed_writes; i++) {
            write(address,
                  ((value >> (i * sizeof(word_size))) & (word_size) ~(0)));
            address++;
        }
    }
}
template<SPECS_TEMPLATE>
void memory<CURRENT_SPECS>::read_block(address_type start_address,
                                       word_size* buffer, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        buffer[i] = read(start_address + i);
    }
}
template<SPECS_TEMPLATE>
void memory<CURRENT_SPECS>::write_block(address_type start_address,
                                        word_size* buffer, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        write(start_address + i, buffer[i]);
    }
}

template<SPECS_TEMPLATE>
void memory<CURRENT_SPECS>::load_raw(address_type   start_address,
                                     std::ifstream& file) {

    while (file.peek() != EOF) {
        word_size next_data;
        file.read((char*)&next_data, sizeof(word_size));

        // Need to reverse order of bytes
        word_size reversed_data = 0;
        for (size_t i = 0; i < sizeof(word_size); i++) {
            reversed_data |= ((next_data >> (i * 8)) & 0xff)
                             << ((sizeof(word_size) - i - 1) * 8);
        }

        write(start_address, reversed_data);
        start_address++;
    }
}

#endif // MEMORY_ACCESS_TPP
