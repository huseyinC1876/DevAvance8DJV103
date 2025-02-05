#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

void showInfos(const std::string& uprojectPath) {
    std::ifstream file(uprojectPath);
    if (!file) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << uprojectPath << "\n";
        return;
    }
    
    json projectJson;
    file >> projectJson;
    file.close();
    
    std::cout << "Nom du jeu : " << projectJson["Name"] << "\n";
    std::cout << "Version d'Unreal : " << projectJson["EngineAssociation"] << "\n";
    if (projectJson.contains("Plugins")) {
        std::cout << "Plugins utilisés :\n";
        for (const auto& plugin : projectJson["Plugins"]) {
            std::cout << " - " << plugin["Name"] << "\n";
        }
    }
}

void buildProject(const std::string& uprojectPath) {
    std::string command = "./Engine/Build/BatchFiles/Build.bat " + uprojectPath;
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur lors de la compilation.\n";
    }
}

void packageProject(const std::string& uprojectPath, const std::string& packagePath) {
    std::string command = "./Engine/Build/BatchFiles/RunUAT.bat BuildCookRun -project=" + uprojectPath +
                          " -noP4 -clientconfig=Development -serverconfig=Development -nocompile -nocompileeditor -installed -nop4 "
                          " -cook -stage -archive -archivedirectory=" + packagePath + " -package -build -targetplatform=Win64";
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur lors du packaging.\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage : MyTool [CHEMIN DU UPROJECT] [COMMAND] [OPTIONS]\n";
        return 1;
    }
    
    std::string uprojectPath = argv[1];
    std::string command = argv[2];
    
    if (command == "show-infos") {
        showInfos(uprojectPath);
    } else if (command == "build") {
        buildProject(uprojectPath);
    } else if (command == "package") {
        if (argc < 4) {
            std::cerr << "Erreur : Le chemin de destination du package est requis.\n";
            return 1;
        }
        std::string packagePath = argv[3];
        packageProject(uprojectPath, packagePath);
    } else {
        std::cerr << "Commande inconnue : " << command << "\n";
        return 1;
    }
    
    return 0;
}
