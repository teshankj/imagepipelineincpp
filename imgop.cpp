#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

struct ImageMetaData {
    int width;
    int height;
    int maxColorValue;
    int numChannels = 3; // Default to 3 channels for RGB
    std::string magicNumber;
};

// check the image extension for larger than 4 characters and either .ppm or .PPM
bool checkImageExtension(const std::string& filepath, ImageMetaData& metaData) {
    if (filepath.length() < 4) {
        std::cout<<"Error:Filename is too short to have a valid extension." << std::endl;
        return false; // Not enough characters for a valid extension
    }
    
    std::string extension = filepath.substr(filepath.length() - 4);
    if (extension != ".ppm" && extension != ".PPM") {
        std::cout<<"Error:Invalid file extension: " << extension << std::endl;
    return false; // Invalid extension
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout<<"Error:Could not open file: " << filepath << std::endl;
        return false;
    }

    file >> metaData.magicNumber;
    if (metaData.magicNumber != "P6" && metaData.magicNumber != "P3") {
        std::cout<<"Error:Invalid PPM format. Expected P6 or P3, got: " << metaData.magicNumber << std::endl;
        return false; // Invalid PPM format
    }
    std::cout << "Success: Valid " << metaData.magicNumber << " PPM file detected."<< "\n-----------------------"<<std::endl;
    

    file >> std::ws; // Skip any whitespace after the magic number

    while (file.peek() == '#') {
        std::string comment;
        std::getline(file, comment); // Read and completely ignore the line
        std::cout << "Info: Skipping comment line: " << comment << std::endl;
        file >> std::ws;             // Consume any remaining whitespace
    }

    file >> metaData.width >> metaData.height >> metaData.maxColorValue;
    std::cout << "Info: Image dimensions: " << metaData.width << "x" << metaData.height 
              << ", Max Color Value: " << metaData.maxColorValue<< "\n-----------------------" << std::endl;
              
    file.close();
    return true;
    
}
int main()
{
    std::string testDir = "images";
    if(std::filesystem ::exists(testDir) && std::filesystem::is_directory(testDir)) {
        std::cout << "Info: Directory '" << testDir << "' exists." << std::endl;
    } else {
        std::cout << "Error: Directory '" << testDir << "' does not exist." << std::endl;
        return 1; // Exit with error code
    }
    std::cout << "Scanning folder for files:\n-------------------" << std::endl;
    for (const auto& entry : std::filesystem::directory_iterator(testDir)) {
        if (std::filesystem::is_regular_file(entry.path())) {
            std::cout << "Found file: " << entry.path().filename() << std::endl;
            ImageMetaData metaData;
            std::cout << "Verifying Stage 1 & 2 for: " << entry.path().filename() <<std::endl;
            checkImageExtension(entry.path().string(), metaData);
        }
    }
    return 0;
}
