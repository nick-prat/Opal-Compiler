#include <iostream>
#include <fstream>
#include <string>

#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "scenehandler.hh"
#include "resources.hh"
#include "util.hh"

using json = nlohmann::json;

void help() {
    std::cout << "OVERVIEW: Opal opl creation tool\n\n";
    std::cout << "USAGE: oplc [options] <input>\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  compile <input.json>\tReads input json file and outputs .opl blob\n";
    std::cout << "  decompile <input.opl>\tReads input .opl blob and outputs json and resources\n";
    std::cout << "  info <input.opl>\tReads input .opl blob and prints info\n";
    std::cout << "  help\t\t\tDisplays this screen\n";
}

bool open(std::ifstream& file) {
    std::string input;
    std::array<char, 3> tag;
    file.read(tag.data(), 3);
    file.seekg(std::ios::beg);

    SceneHandler scene;
    if(bytecmp<3>(tag.data(), OPLTAG)) {
        scene.readFromBIN(file);
    } else {
        scene.readFromJSON(file);
    }

    while(true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        std::vector<std::string> inputs = splitString(input, ' ');
        try {
            if(inputs[0] == "help") {
                help();
            } else if(inputs[0] == "quit") {
                return false;
            } else if(inputs[0] == "reload") {
                return true;
            } else if(inputs[0] == "compile") {
                scene.writeToBIN();
            } else if(inputs[0] == "info") {
                if(inputs.size() == 1) {
                    scene.info();
                } else if(inputs.size() == 2) {
                    if(inputs[1] == "model3d") {
                        scene.model3DInfo();
                    } else if(inputs[1] == "texture") {
                        scene.textureInfo();
                    } else {
                        std::cerr << "Unknown resource type " << inputs[1] << '\n';
                    }
                } else {
                    std::cerr << "info command invalid\ninfo usage: info <(optional) resource type>\n";
                }
            } else if(inputs[0] == "rm") {
                if(inputs.size() == 3) {
                    if(inputs[1] == "model3d") {
                        scene.deleteModel3D(inputs[2]);
                    } else if(inputs[1] == "texture") {
                        scene.deleteTexture(inputs[2]);
                    } else {
                        std::cerr << "Unknown resource type " << inputs[1] << '\n';
                    }
                } else {
                    std::cerr << "rm command invalid\nrm usage: " << "rm <resource type> <resource name>\n";
                }
            } else if(inputs[0] == "add") {
                if(inputs.size() == 4) {
                    if(inputs[1] == "model3d") {
                        scene.loadModel3D(inputs[2], inputs[3]);
                    } else if(inputs[1] == "texture") {
                        scene.loadTexture(inputs[2], inputs[3]);
                    } else {
                        std::cerr << "Unknown resource type " << inputs[1] << '\n';
                    }
                } else {
                    std::cerr << "add command invalid\nadd usage: " << "rm <resource type> <file name> <resource name>\n";
                }
            } else {
                std::cerr << "unknown command \"" << input << "\"\n";
            }
        } catch(std::runtime_error& error) {
            std::cerr << error.what() << '\n';
        }
    }
}

int main(int argc, char* argv[]) {

    if(argc < 2 || argc > 3) {
        help();
        return 0;
    }

    std::string op{argv[1]};
    if(argc == 2) {
        if(op == "help") {
            help();
            return 0;
        } else {
            std::cerr << "unknown option \"" << op << "\"\n";
            help();
            return 0;
        }
    } else if(argc == 3) {
        std::ifstream input(argv[2], std::ios_base::binary | std::ios_base::out);
        if(!input.is_open()) {
            std::cerr << "couldn't open file \"" << argv[2] << "\"\n";
            return 0;
        }

        if(op == "open") {
            while(open(input)) {
                input.seekg(std::fstream::beg);
                std::cout << "Reloading...\n";
            }
        } else {
            std::cerr << "unknown option \"" << op << "\"\n";
        }
    } else {
        help();
    }

    return 0;
}
