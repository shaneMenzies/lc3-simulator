#ifndef MEMORY_ACCESS_H
#define MEMORY_ACCESS_H

#include "specifications.h"

#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <string>

template<class address_type, class word_size> struct memory_region {

    virtual word_size read(address_type address)                   = 0;
    virtual void      write(address_type address, word_size value) = 0;
};

template<class address_type, class word_size>
struct ram_region : public memory_region<address_type, word_size> {

    ram_region<address_type, word_size>(address_type size) :
        buffer(new word_size[size]) {}

    ~ram_region<address_type, word_size>() { delete buffer; }

    word_size read(address_type address) override;
    void      write(address_type address, word_size value) override;

    word_size* buffer;
};

template<class address_type, class word_size>
struct custom_region : public memory_region<address_type, word_size> {

    custom_region<address_type, word_size>(
        std::function<word_size(address_type)>       read_function,
        std::function<void(address_type, word_size)> write_function) :
        read_function(read_function),
        write_function(write_function) {}

    word_size read(address_type address) override;
    void      write(address_type address, word_size value) override;

    std::function<word_size(address_type)>       read_function;
    std::function<void(address_type, word_size)> write_function;
};

template<class address_type, class word_size> struct region_descriptor {
    address_type                            start;
    address_type                            end;
    memory_region<address_type, word_size>* region;

    region_descriptor<address_type, word_size>(
        address_type start, address_type end,
        memory_region<address_type, word_size>* region) :
        start(start),
        end(end), region(region) {}

    word_size read(address_type address) const;
    void      write(address_type address, word_size value) const;

    friend bool operator==(const region_descriptor& region,
                           const address_type       address) {
        return (address > region.start && address < region.end);
    }
    friend bool operator==(const region_descriptor& lhs,
                           const region_descriptor& rhs) {
        return (lhs.start == rhs.start);
    }
    friend bool operator<(const region_descriptor& lhs,
                          const region_descriptor& rhs) {
        return (lhs.start < rhs.start);
    }
    friend bool operator>(const region_descriptor& lhs,
                          const region_descriptor& rhs) {
        return (lhs.start > rhs.start);
    }
    friend bool operator<(const region_descriptor& lhs,
                          const address_type&      rhs) {
        return (lhs.end < rhs);
    }
    friend bool operator>(const region_descriptor& lhs,
                          const address_type&      rhs) {
        return (lhs.start > rhs);
    }
    friend bool operator<(const address_type&      lhs,
                          const region_descriptor& rhs) {
        return (lhs < rhs.start);
    }
    friend bool operator>(const address_type&      lhs,
                          const region_descriptor& rhs) {
        return (lhs > rhs.end);
    }
};

template<SPECS_TEMPLATE> class memory {

  public:
    word_size read(address_type address);

    template<class T> T read_t(address_type address);

    void write(address_type address, word_size value);

    template<class T> void write_t(address_type address, T value);

    void read_block(address_type start_address, word_size* buffer,
                    unsigned int size);

    void write_block(address_type start_address, word_size* buffer,
                     unsigned int size);

    void load_raw(address_type start_address, std::ifstream& file);

    std::set<region_descriptor<address_type, word_size>, std::less<>> regions;
};

#include "template/mem_access.tpp"

#endif // MEMORY_ACCESS_H
