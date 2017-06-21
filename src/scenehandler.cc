#include <FreeImage.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "scenehandler.hh"
#include "resources.hh"
#include "util.hh"

using json = nlohmann::json;

void SceneHandler::readFromJSON(std::ifstream& file) {
    std::cout << "Reading JSON file...\n";
    std::string contents;
    file.seekg(0, std::ios::end);
    contents.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&contents[0], contents.size());
    file.close();

    json scene = json::parse(contents);

    if(auto jname = scene.find("name"); jname != scene.end()) {
        m_sceneName = *jname;
    } else {
        throw std::runtime_error("JSON scene doesn't contain a name");
    }

    if(auto jresources = scene.find("resources"); jresources != scene.end()) {
        for(auto& resource : *jresources) {
            if(!resource.count("resourcename")) {
                throw std::runtime_error("resource missing resourcename field");
            }

            std::string resourcename = resource["resourcename"];

            if(!resource.count("type")) {
                throw std::runtime_error(resourcename + " missing type field");
            }

            if(!resource.count("filename")) {
                throw std::runtime_error(resourcename + " missing filename field");
            }

            std::string type = resource["type"];
            std::string filename = resource["filename"];

            if(type == "model3d") {
                loadModel3D(filename, resourcename);
            } else if(type == "texture") {
                loadTexture(filename, resourcename);
            } else {
                std::cerr << "Unknown resource type " << type << '\n';
            }
        }
    } else {
        throw std::runtime_error("Scene contains no resources");
    }
    std::cout << "Done\n";
}

void SceneHandler::readFromBIN(std::ifstream& file) {
    std::cout << "Reading OPL file...\n";
    std::array<char, 3> tag = read<decltype(tag)>(file);
    unsigned short version = read<decltype(version)>(file);
    m_sceneName = readString(file);
    std::cout << "Loading scene " << m_sceneName << '\n';

    std::size_t modelcount = read<std::size_t>(file);
    for(unsigned int i = 0; i < modelcount; i++) {
        if(read<char>(file) == RES_MODEL3D) {
            loadModel3D(file);
        }
    }

    std::size_t texturecount = read<std::size_t>(file);
    for(unsigned int i = 0; i < texturecount; i++) {
        if(read<char>(file) == RES_TEXTURE) {
            loadTexture(file);
        }
    }
    std::cout << "Done\n";
}

void SceneHandler::writeToJSON() {

}

void SceneHandler::writeToBIN () {
    std::cout << "Compiling to output.opl...\n";
    std::ofstream file("output.opl", std::ios::binary);
    file.write(OPLTAG, sizeof(OPLTAG));
    write(file, VERSION);
    writeString(file, m_sceneName);
    std::cout << m_sceneName << '\n';

    write(file, m_model3ds.size());
    for(auto& [name, model] : m_model3ds) {
        write(file, RES_MODEL3D);
        writeString(file, name);
        model.writeToStream(file);
    }

    write(file, m_textures.size());
    for(auto& [name, texture] : m_textures) {
        write(file, RES_TEXTURE);
        writeString(file, name);
        texture.writeToStream(file);
    }
    std::cout << "Done\n";
}

void SceneHandler::deleteModel3D(const std::string& name) {
    if(auto iter = m_model3ds.find(name); iter != m_model3ds.end()) {
        m_model3ds.erase(iter);
    } else {
        throw std::runtime_error("Model3D " + name + " doesn't exist");
    }
}

void SceneHandler::deleteTexture(const std::string& name) {
    if(auto iter = m_textures.find(name); iter != m_textures.end()) {
        m_textures.erase(iter);
    } else {
        throw std::runtime_error("Texture " + name + " doesn't exist");
    }
}

void SceneHandler::info() {
    std::cout << "Texture count: " << m_textures.size() << '\n';
    for(auto& [name, texture] : m_textures) {
        std::cout << '\t' << name << " [" << texture.width << ',' << texture.height << "]\n";
    }
    std::cout << "Model3D count: " << m_model3ds.size() << '\n';
    for(auto& [name, model] : m_model3ds) {
        std::cout << '\t' << name << " [" << model.meshes.size() << "]\n";
    }
}

void SceneHandler::model3DInfo() {
    for(auto& model : m_model3ds) {
        std::cout << model.first << '\n';
        for(int i = 0; i < model.second.meshes.size(); i++) {
            auto& mesh = model.second.meshes[i];
            std::cout << "Mesh [" << i << "]\n";
            std::cout << '\t' << "Texture: " << mesh.matName << '\n';
            std::cout << '\t' << "Vertex Size: " << mesh.vertices.size() << '\n';
            std::cout << '\t' << "Index Size: " << mesh.indices.size() << "\n\n";
        }
    }
}

void SceneHandler::textureInfo() {
    for(auto& texture : m_textures) {
        std::cout << texture.first << '\n';
        std::cout << '\t' << "Width: " << texture.second.width << '\n';
        std::cout << '\t' << "Height: " << texture.second.height << '\n';
    }
}

void loadNode(const aiScene* scene, const aiNode* node, const glm::mat4& parentTransform, std::vector<Mesh>& meshes) {
    glm::mat4x4 transformation;
    copyaiMat(&node->mTransformation, transformation);
    transformation = parentTransform * transformation;

    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        loadNode(scene, node->mChildren[i], transformation, meshes);
    }

    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for(unsigned int j = 0; j < mesh->mNumVertices; j++) {
            Vertex vertex;

            glm::vec4 position = transformation * glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f);
            vertex.position = vec3{{position.x, position.y, position.z}};

            vertex.normal = (mesh->HasNormals())
                ? vec3{{mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z}}
                : vec3{{0.0f, 0.0f, 0.0f}};

            vertex.texCoord = (mesh->HasTextureCoords(0))
                ? vec2{{mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y}}
                : vec2{{0.0f, 0.0f}};

            vertices.push_back(vertex);
        }

        if(mesh->HasFaces()) {
            for(unsigned int j = 0; j < mesh->mNumFaces; j++) {
                aiFace face = mesh->mFaces[j];
                for(unsigned int k = 0; k < face.mNumIndices; k++) {
                    indices.emplace_back(face.mIndices[k]);
                }
            }
        } else {
            std::cout << "Node was missing faces, quitting...\n";
            exit(-1);
        }

        meshes.emplace_back(std::move(vertices), std::move(indices));
        meshes.back().matIndex = mesh->mMaterialIndex;
    }
}

void SceneHandler::loadModel3D(const std::string& modelname, const std::string& resourcename) {
    if(m_model3ds.find(resourcename) != m_model3ds.end()) {
        throw std::runtime_error("Model3D " + resourcename + " already exists, skipping");
    }

    std::string filename = "Resources/Models/" + modelname + ".3ds";
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename.c_str(),
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    std::vector<Mesh> meshes;

    if(!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) {
        throw std::runtime_error("Loading model " + filename + " failed");
        exit(-1);
    }

    aiMaterial** materials = scene->mMaterials;
    for(unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiString aName;
        materials[i]->Get(AI_MATKEY_NAME, aName);
        std::string texname = std::string(aName.C_Str());
        std::string texresname = "tex_" + modelname + "_" + texname;
        std::string texfilename = modelname + "/" + texname;
        try {
            loadTexture(modelname + "/" + texname, texresname);
        } catch(std::runtime_error& error) {
            std::cerr << error.what() << '\n';
        }
    }

    loadNode(scene, scene->mRootNode, glm::mat4(1.0f), meshes);

    for(auto& mesh : meshes) {
        aiString aName;
        unsigned int index = mesh.matIndex;
        if(index < scene->mNumMaterials) {
            materials[mesh.matIndex]->Get(AI_MATKEY_NAME, aName);
            std::string resname = "tex_" + modelname + "_" + aName.C_Str();
            if(auto iter = m_textures.find(resname); iter != m_textures.end()) {
                mesh.matName = iter->first;
            } else {
                throw std::runtime_error(modelname + " missing unknown texture " + resname);
            }
        }
    }
    m_model3ds.emplace(resourcename, Model3D{std::move(meshes)});
}

void SceneHandler::loadModel3D(std::istream& stream) {
    std::string name = readString(stream);

    if(auto iter = m_model3ds.emplace(name, Model3D()); iter.second) {
        auto& m3d = iter.first->second;
        auto meshSize = read<std::size_t>(stream);
        for(auto i = 0; i < meshSize; i++) {
            m3d.meshes.emplace_back();
            auto& mesh = m3d.meshes.back();

            std::size_t size = read<std::size_t>(stream);
            mesh.matIndex = read<decltype(mesh.matIndex)>(stream);
            mesh.matName = readString(stream);

            std::size_t vsize = read<std::size_t>(stream);
            mesh.vertices.resize(vsize);
            stream.read((char*)mesh.vertices.data(), sizeof(Vertex) * vsize);

            std::size_t isize = read<std::size_t>(stream);
            mesh.indices.resize(isize);
            stream.read((char*)mesh.indices.data(), sizeof(unsigned int) * isize);
        }
    } else {
        throw std::runtime_error(name + " is duplicated in OPL file");
    }
}

void SceneHandler::loadTexture(const std::string& name, const std::string& resourcename) {
    if(m_textures.find(resourcename) != m_textures.end()) {
        std::cerr << "Texture " << resourcename << " already exists, skipping\n";
        return;
    }

    FIBITMAP *img;
    auto filename = "Resources/Textures/" + name + ".tga";
    FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filename.c_str());

    if(!FreeImage_FIFSupportsReading(format)) {
        throw std::runtime_error(filename + " couldn't be read by FreeImage");
    }

    if(format == FIF_UNKNOWN) {
        throw std::runtime_error(filename + " has unknown format");
    }

    img = FreeImage_Load(format, filename.c_str());

    if(!img) {
        throw std::runtime_error(filename + " image data couldn't be loaded");
    }

    if(FreeImage_GetBPP(img) != 32) {
        FIBITMAP* oldImg = img;
        img = FreeImage_ConvertTo32Bits(oldImg);
        FreeImage_Unload(oldImg);
    }

    unsigned int height, width;
    width = FreeImage_GetWidth(img);
    height = FreeImage_GetHeight(img);

    unsigned char* bytes = FreeImage_GetBits(img);

    if(bytes == nullptr) {
        FreeImage_Unload(img);
        std::cout << "couldn't load image bytes" << filename << '\n';
        return;
    }

    std::vector<unsigned char> vb{bytes, bytes + width * height * RES_TEXTURE_BPP};
    m_textures.emplace(resourcename, Texture{std::move(vb), width, height});
    FreeImage_Unload(img);
}

void SceneHandler::loadTexture(std::istream& stream) {
    std::string name = readString(stream);
    if(auto iter = m_textures.emplace(name, Texture()); iter.second) {
        auto& texture = iter.first->second;
        texture.width = read<decltype(texture.width)>(stream);
        texture.height = read<decltype(texture.height)>(stream);
        texture.bytes.resize(texture.width * texture.height * RES_TEXTURE_BPP);
        stream.read((char*)texture.bytes.data(), texture.bytes.size());
    } else {
        throw std::runtime_error(name + " is duplicated in OPL file");
    }
}

void SceneHandler::loadShader(const std::string& name, const std::string& resourcename) {

}

void SceneHandler::loadShader(std::istream& stream) {

}
