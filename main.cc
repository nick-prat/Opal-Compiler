#include <iostream>
#include <fstream>
#include <string>

#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Opal/Resources/scenehandler.hh>
#include <Opal/Resources/resources.hh>
#include <Opal/Util/util.hh>

#include <gtk/gtk.h>

using namespace Opal;

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

void compile(std::istream &iStream, std::ostream &oStream) {
    Opal::Resources::SceneHandler scene{iStream};
    scene.writeToBIN(oStream);
}

bool open(std::istream &stream) {
    std::string input;

    Opal::Resources::SceneHandler scene{stream};

    while(true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        std::vector<std::string> inputs = Util::splitString(input, ' ');
        try {
            if(inputs[0] == "help") {
                help();
            } else if(inputs[0] == "quit" || inputs[0] == "exit") {
                return false;
            } else if(inputs[0] == "reload") {
                return true;
            } else if(inputs[0] == "compile") {
                std::ofstream file{"output.opl", std::ios::binary | std::ofstream::out | std::ofstream::trunc};
                scene.writeToBIN(file);
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
                        auto [model3d, textures] = Opal::Resources::loadModel3D(inputs[2]);
                        if(!scene.addModel3D(inputs[3], std::move(model3d))) {
                            std::cerr << "Model3D " << inputs[3] << " already added, skipping\n";
                        } else {
                            std::cout << "hi " << textures.size() << '\n';
                            for(auto& [name, texture] : textures) {
                                if(!scene.addTexture(name, std::move(texture))) {
                                    std::cerr << "Texture " << name << " already added, skipping\n";
                                }
                            }
                        }
                    } else if(inputs[1] == "texture") {
                        auto tex = Opal::Resources::loadTexture(inputs[2]);
                        if(!scene.addTexture(inputs[3], std::move(tex))) {
                            std::cerr << "Texture " << inputs[3] << " already added, skipping\n";
                        }
                    } else {
                        std::cerr << "Unknown resource type " << inputs[1] << '\n';
                    }
                } else {
                    std::cerr << "add command invalid\nadd usage: " << "rm <resource type> <file name> <resource name>\n";
                }
            } else {
                std::cerr << "unknown command \"" << input << "\"\n";
            }
        } catch(std::runtime_error &error) {
            std::cerr << error.what() << '\n';
        }
    }
}

static void openFile(GtkWidget *widget, gpointer data) {
    auto scene = static_cast<Resources::SceneHandler*>(data);

    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    auto dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(data),
                                            action,
                                            "Cancel",
                                            GTK_RESPONSE_CANCEL,
                                            "Open",
                                            GTK_RESPONSE_ACCEPT,
                                            nullptr);

    auto res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        auto filename = gtk_file_chooser_get_filename(chooser);
        g_print("%s\n", filename);
        std::ifstream stream{filename};
        if(stream.is_open()) {
            // scene(filename);
        } else {
            std::cerr << "Couldn't open file " << filename << '\n';
        }
        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

static void activate(GtkApplication *app, gpointer data) {
    auto scene = static_cast<Resources::SceneHandler*>(data);

    auto window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Hello GNOME");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

    auto buttonBox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add(GTK_CONTAINER(window), buttonBox);

    auto button = gtk_button_new_with_label("Hello World");
    g_signal_connect(button, "clicked", G_CALLBACK(openFile), scene);
    gtk_container_add(GTK_CONTAINER (buttonBox), button);

    gtk_widget_show_all(window);
}

int startGTK(int argc, char* argv[]) {
    Resources::SceneHandler scene;

    auto app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &scene);
    auto status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return (status);
}

int main(int argc, char* argv[]) {

    if(argc == 1) {
        startGTK(argc, argv);
    } else if(argc >= 2) {
        std::string op{argv[1]};
        if(op == "help") {
            help();
            return 0;
        } else if(op == "open") {
            if(argc >= 3) {
                std::ifstream file{argv[2], std::ios::binary};
                if(file.is_open()) {
                    while(open(file)) {
                        std::cout << "Reloading...\n";
                    }
                } else {
                    std::cerr << "Couldn't open file " << argv[2] << '\n';
                }
            } else {
                open(std::cin);
            }
        } else {
            std::cerr << "unknown option \"" << op << "\"\n";
            help();
            return 0;
        }
    } else {
        help();
    }

    return 0;
}
