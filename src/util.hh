#ifndef _UTIL_H
#define _UTIL_H

#include <fstream>
#include <string>
#include <array>
#include <vector>

#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>

constexpr char OPLTAG[3]{'\x4F', '\x50', '\x4C'};
constexpr char RES_MODEL3D = '\x01';
constexpr char RES_TEXTURE = '\x02';
constexpr unsigned char RES_TEXTURE_BPP = 4;
constexpr unsigned short VERSION = 1;

typedef std::array<float, 3> vec3;
typedef std::array<float, 2> vec2;

std::string readString(std::istream& stream);

std::vector<std::string> splitString(const std::string& string, const char delim);

void writeString(std::ostream& stream, const std::string& buff);

void copyaiMat(const aiMatrix4x4* from, glm::mat4& to);

template<typename data_t>
std::enable_if_t<!std::is_pointer<data_t>::value> write(std::ostream& stream, data_t data) {
    static_assert(std::is_standard_layout<data_t>(), "write failed, data isn't standard layout");
    stream.write((char*)&data, sizeof(std::decay_t<data_t>));
}

template<typename data_t>
std::enable_if_t<std::is_pointer<data_t>::value> write(std::ostream& stream, data_t data, std::size_t size) {
    static_assert(std::is_standard_layout<std::decay<data_t>>(), "write failed, data isn't standard layout");
    stream.write(data, size);
}

template<typename data_t>
std::enable_if_t<!std::is_pointer<data_t>::value, data_t> read(std::istream& stream) {
    static_assert(std::is_standard_layout<data_t>(), "read failed, data isn't standard layout");
    data_t data;
    stream.read((char*)&data, sizeof(std::decay_t<data_t>));
    return data;
}

template<std::size_t length>
bool bytecmp(const char* const l, const char* const r) {
    for(unsigned int i = 0; i < length; i++) {
        if(l[i] != r[i]) {
            return false;
        }
    }
    return true;
}

#endif // _UTIL_h
