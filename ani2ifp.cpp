#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void convertAniToIfp(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream input(inputPath, std::ios::binary | std::ios::ate);
    std::vector<char> aniData(input.tellg());
    input.seekg(0);
    input.read(aniData.data(), aniData.size());
    input.close();

    if (aniData.size() < 36 || std::string(aniData.data(), 4) != "ANP3") {
        std::cerr << "ERROR: " << inputPath << std::endl;
        return;
    }

    std::vector<char> ifpData(aniData.size());
    std::copy(aniData.begin(), aniData.begin() + 4, ifpData.begin());
    ifpData[4] = 0x14; ifpData[5] = 0x3D; ifpData[6] = 0x23; ifpData[7] = 0x00;
    std::copy(aniData.begin() + 4, aniData.begin() + 32, ifpData.begin() + 8);
    std::copy(aniData.begin() + 36, aniData.end(), ifpData.begin() + 36);

    std::ofstream output(outputPath, std::ios::binary);
    output.write(ifpData.data(), ifpData.size());
    std::cout << inputPath << " -> " << outputPath << std::endl;
}

int main() {
    setlocale(LC_ALL, "");
    int count = 0;
    
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.path().extension() == ".ani") {
            fs::path outPath = entry.path();
            outPath.replace_extension(".ifp");
            convertAniToIfp(entry.path().string(), outPath.string());
            count++;
        }
    }
    
    std::cout << "Done: " << count << " files" << std::endl;
    std::cin.get();
}