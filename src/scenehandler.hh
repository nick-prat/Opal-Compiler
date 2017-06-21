#ifndef _RESOURCEHANDLER_H
#define _RESOURCEHANDLER_H

#include <string>
#include <fstream>
#include <unordered_map>
#include <json.hpp>

#include "resources.hh"

class SceneHandler {
public:
    SceneHandler() = default;

    void readFromJSON(std::ifstream& file);
    void readFromBIN(std::ifstream& file);
    void writeToJSON();
    void writeToBIN();

    void deleteModel3D(const std::string& name);
    void deleteTexture(const std::string& name);
    void deleteShader(const std::string& name);

    void loadModel3D(const std::string& filename, const std::string& resourcename);
    void loadTexture(const std::string& filename, const std::string& resourcename);
    void loadShader(const std::string& filename, const std::string& resourcename);

    void info();
    void model3DInfo();
    void textureInfo();

private:
    void loadModel3D(std::istream& stream);

    void loadTexture(std::istream& stream);

    void loadShader(std::istream& stream);

private:
    std::string m_sceneName;
    std::unordered_map<std::string, Texture> m_textures;
    std::unordered_map<std::string, Model3D> m_model3ds;
};

#endif // _RESOURCEHANDLER_H
