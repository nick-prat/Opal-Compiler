#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <fstream>
#include <vector>
#include <array>

#include "util.hh"

struct Vertex {
    Vertex() = default;
    Vertex(std::istream& stream);
    Vertex(vec3 pos, vec3 norm, vec2 tc);

    constexpr std::size_t size() const;
    void writeToStream(std::ostream& stream);

    vec3 position;
    vec3 normal;
    vec2 texCoord;
};

struct Mesh {
    Mesh() = default;
    Mesh(std::istream& stream);
    Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices);

    std::size_t size() const;
    void writeToStream(std::ostream& stream);

    unsigned int matIndex;
    std::string matName;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

struct Texture {
    Texture() = default;
    Texture(std::istream& stream);
    Texture(std::vector<unsigned char>&& bytes, unsigned int width, unsigned int height);

    std::size_t size() const;
    void writeToStream(std::ostream& stream);

    std::vector<unsigned char> bytes;
    unsigned int width, height;
};

struct Model3D {
    Model3D() = default;
    Model3D(std::istream& stream);
    Model3D(std::vector<Mesh>&& meshes);

    std::size_t size() const;
    void writeToStream(std::ostream& stream);

    std::vector<Mesh> meshes;
};

struct Object {
    Object() = default;
    Object(std::istream& stream);

    std::size_t size() const;
    void writeToStream(std::ostream& stream);

    char type;
    std::string resourceName;
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

#endif // _RESOURCES_H
