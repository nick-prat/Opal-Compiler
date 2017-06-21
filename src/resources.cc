#include "resources.hh"

#include <iostream>

Vertex::Vertex(std::istream& stream) {
    position = read<decltype(position)>(stream);
    normal = read<decltype(normal)>(stream);
    texCoord = read<decltype(texCoord)>(stream);
}

Vertex::Vertex(vec3 pos, vec3 norm, vec2 tc)
: position(pos)
, normal(norm)
, texCoord(tc) {}

constexpr std::size_t Vertex::size() const {
    return sizeof(position) + sizeof(normal) + sizeof(texCoord);
}

void Vertex::writeToStream(std::ostream& stream) {
    write(stream, position);
    write(stream, normal);
    write(stream, texCoord);
}

Mesh::Mesh(std::istream& stream) {
    std::size_t size = read<std::size_t>(stream);
    stream >> matIndex;
    matName = readString(stream);

    std::size_t vsize = read<std::size_t>(stream);
    vertices.resize(vsize);
    stream.read((char*)vertices.data(), sizeof(Vertex) * vsize);

    std::size_t isize = read<std::size_t>(stream);
    indices.resize(isize);
    stream.read((char*)indices.data(), sizeof(unsigned int) * isize);
}

Mesh::Mesh(std::vector<Vertex>&& verts, std::vector<unsigned int>&& inds)
: vertices(std::move(verts))
, indices(std::move(inds)) {}

std::size_t Mesh::size() const {
    return sizeof(unsigned int)
        + sizeof(size())
        + sizeof(matIndex)
        + matName.size() + 1
        + sizeof(vertices.size())
        + sizeof(Vertex) * vertices.size()
        + sizeof(indices.size())
        + sizeof(unsigned int) * indices.size();
}

void Mesh::writeToStream(std::ostream& stream) {
    write(stream, size());
    write(stream, matIndex);
    writeString(stream, matName);

    write(stream, vertices.size());
    for(auto& vert : vertices) {
        vert.writeToStream(stream);
    }

    write(stream, indices.size());
    for(auto& ind : indices) {
        write(stream, ind);
    }
}

Texture::Texture(std::istream& stream) {
}

Texture::Texture(std::vector<unsigned char>&& bytes_, unsigned int width_, unsigned int height_)
: bytes(std::move(bytes_))
, width(width_)
, height(height_) {}

std::size_t Texture::size() const {
    return bytes.size();
}

void Texture::writeToStream(std::ostream& stream) {
    write(stream, width);
    write(stream, height);
    stream.write((char*)bytes.data(), bytes.size());
}

Model3D::Model3D(std::vector<Mesh>&& meshes_)
: meshes(std::move(meshes_)) {}

std::size_t Model3D::size() const {
    std::size_t size = 0;
    for(auto& mesh : meshes) {
        size += mesh.size();
    }
    return size;
}

void Model3D::writeToStream(std::ostream& stream) {
    write(stream, meshes.size());
    for(auto mesh : meshes) {
        mesh.writeToStream(stream);
    }
}

Object::Object(std::istream& stream) {

}

std::size_t Object::size() const {
    return sizeof(type)
        + resourceName.size()
        + sizeof(position)
        + sizeof(rotation)
        + sizeof(scale);
}

void Object::writeToStream(std::ostream& stream) {
    write(stream, type);
    writeString(stream, resourceName);
    write(stream, position);
    write(stream, rotation);
    write(stream, scale);
}
