//
// Created by isaac on 10/29/2020.
//

#ifndef UNTITLED2_UNIQUEID_H
#define UNTITLED2_UNIQUEID_H
#include <cstdint>
#include <algorithm>
#include <random>
#include <ctime>

/*
 * We are using a small UUID type for now because we don't expect many clients
 * May need to scale this up later on
 */
using UniqueIDType = uint32_t;
const size_t UniqueIDSize = sizeof(UniqueIDType);

class UniqueID {
public:
    /**
     * Assume UniqueIDSize bytes need to be retrieved.
     * Should be used when everything is encoded.
     * Assumes endianness is correct and the same between client and server (will need to be guaranteed later on).
     * @param pointer of the bytes from which to read the unique ID
     */
    inline UniqueID(const char* bytes) {
        std::copy(bytes, bytes + UniqueIDSize, data.bytes);
    }

    /**
     * Create UniqueID from a structure of type UniqueIDType
     * @param value Value to create from
     */
    inline UniqueID(UniqueIDType value) {
        data.value = value;
    }

    /**
     * Create UniqueID from a normal string (reverse of toString())
     * @param string The string to create the ID from. Assuming that no
     */
     inline UniqueID(const std::string& string) {
         data.value = static_cast<uint32_t>(std::stoul(string));
     }

    /**
     * Assuming UniqueIDSize bytes need to be placed in the destination
     * @param dest Destination for the bytes
     */
    inline void fill(char* dest) const {
        std::copy(data.bytes, data.bytes + UniqueIDSize, dest);
    }

    /**
     * For displaying the ID to the console for example
     * @return a string converison of the UUID
     */
    inline std::string to_string() const {
        // May need something like this later on for bigger types:
//        std::string str;
//        for(size_t i = 0; i < UniqueIDSize; ++i) {
//            str += std::to_string((int) (data.bytes[i]));
//        }
        // This suffices for now
        return std::to_string(data.value);
    }

    inline bool operator<(const UniqueID& other) const {
        return data.value < other.data.value;
    }
private:
    union {
        UniqueIDType value;
        char bytes[UniqueIDSize];
    } data;
    friend class UniqueIDGenerator;
};

class UniqueIDGenerator {
public:
    inline UniqueIDGenerator() : gen(time(0)) {}
    inline UniqueID generate() {
        return UniqueID(gen());
    }
private:
    std::mt19937 gen;
};

#endif //UNTITLED2_UNIQUEID_H
