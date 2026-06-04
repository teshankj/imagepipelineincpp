#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

struct ImageMetaData {
    int width;
    int height;
    int maxColorValue;
    int numChannels = 3; 
    std::string magicNumber;
};

struct ResizedImageData {
    int newWidth = 256;
    int newHeight = 256;
    int newMaxColorValue = 255;
    float xscaleFactor;
    float yscaleFactor;
};

// Writes the processed memory buffer back to a file in the /output folder
void savePPMImage(const std::string& filename, const ResizedImageData& resizedData, const std::vector<uint8_t>& pixelBuffer) {
    std::string outputDir = "output";
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
        std::cout << "Info: Created directory '" << outputDir << "'" << std::endl;
    }

    std::string fullOutputPath = outputDir + "/" + filename;

    // Open file in binary mode to write raw bytes exactly as they are
    std::ofstream outFile(fullOutputPath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file target: " << fullOutputPath << std::endl;
        return;
    }

    // Write plain text header metadata
    outFile << "P6\n"
            << resizedData.newWidth << " " << resizedData.newHeight << "\n"
            << resizedData.newMaxColorValue << "\n";

    // Write the raw binary block
    outFile.write(reinterpret_cast<const char*>(pixelBuffer.data()), pixelBuffer.size());

    std::cout << "Success: Saved resized image to: " << fullOutputPath << std::endl;
}

// Full implementation of parsing, reading, nearest-neighbor resizing, and exporting
void NNIresizer(const std::string& filepath, ImageMetaData& metaData, ResizedImageData& resizedData, const std::string& filename) {
    
    // ---- Step 1: Open the source file ----
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for resizing: " << filepath << std::endl;
        return;
    }

    // ---- Step 2: Parse and validate Header tokens (Stage 1 & 2) ----
    file >> metaData.magicNumber;
    if (metaData.magicNumber != "P6") {
        std::cerr << "Error: NNIresizer currently only supports binary P6 PPM files directly." << std::endl;
        return;
    }

    file >> std::ws; // Consume spaces/newlines
    while (file.peek() == '#') {
        std::string comment;
        std::getline(file, comment);
        file >> std::ws;
    }

    file >> metaData.width >> metaData.height >> metaData.maxColorValue;
    
    // Crucial: Consume the single whitespace character (usually a newline) right after the Max Color Value 
    // before attempting to read raw binary bytes!
    file.get(); 

    std::cout << "Info: Source dimensions: " << metaData.width << "x" << metaData.height << std::endl;

    // ---- Step 3: Read original pixels from disk into a vector ----
    std::vector<uint8_t> srcPixelBuffer(metaData.width * metaData.height * 3);
    file.read(reinterpret_cast<char*>(srcPixelBuffer.data()), srcPixelBuffer.size());
    file.close(); // Done reading the source file completely!

    // ---- Step 4: Setup scaling configurations ----
    resizedData.xscaleFactor = static_cast<float>(metaData.width) / resizedData.newWidth;
    resizedData.yscaleFactor = static_cast<float>(metaData.height) / resizedData.newHeight;

    std::vector<uint8_t> targetPixelBuffer(resizedData.newWidth * resizedData.newHeight * 3);

    // ---- Step 5: Nearest Neighbor Interpolation Loop ----
    for (int y = 0; y < resizedData.newHeight; ++y) {
        for (int x = 0; x < resizedData.newWidth; ++x) {
            
            // Map target pixel center back to continuous coordinate space in the source image
            int srcX = static_cast<int>((x + 0.5f) * resizedData.xscaleFactor);
            int srcY = static_cast<int>((y + 0.5f) * resizedData.yscaleFactor);
            
            // Safety clip boundaries to guarantee we stay inside index boundaries
            if (srcX >= metaData.width)  srcX = metaData.width - 1;
            if (srcY >= metaData.height) srcY = metaData.height - 1;
            if (srcX < 0) srcX = 0;
            if (srcY < 0) srcY = 0;

            // Compute 1D structural offsets for both arrays
            int srcIdx = (srcY * metaData.width + srcX) * 3;
            int dstIdx = (y * resizedData.newWidth + x) * 3;
            
            // Map the RGB values over directly
            targetPixelBuffer[dstIdx]     = srcPixelBuffer[srcIdx];     // Red
            targetPixelBuffer[dstIdx + 1] = srcPixelBuffer[srcIdx + 1]; // Green
            targetPixelBuffer[dstIdx + 2] = srcPixelBuffer[srcIdx + 2]; // Blue
        }
    }

    // ---- Step 6: Save the output image ----
    savePPMImage(filename, resizedData, targetPixelBuffer);
}

// Full implementation of parsing, reading, Bilinear interpolation, and exporting
void BilinearResizer(const std::string& filepath, ImageMetaData& metaData, ResizedImageData& resizedData, const std::string& filename) {
    
    // ---- Step 1: Open the source file ----
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for resizing: " << filepath << std::endl;
        return;
    }

    // ---- Step 2: Parse and validate Header tokens (Stage 1 & 2) ----
    file >> metaData.magicNumber;
    if (metaData.magicNumber != "P6") {
        std::cerr << "Error: NNIresizer currently only supports binary P6 PPM files directly." << std::endl;
        return;
    }

    file >> std::ws; // Consume spaces/newlines
    while (file.peek() == '#') {
        std::string comment;
        std::getline(file, comment);
        file >> std::ws;
    }

    file >> metaData.width >> metaData.height >> metaData.maxColorValue;
    
    // Crucial: Consume the single whitespace character (usually a newline) right after the Max Color Value 
    // before attempting to read raw binary bytes!
    file.get(); 

    std::cout << "Info: Source dimensions: " << metaData.width << "x" << metaData.height << std::endl;

    // ---- Step 3: Read original pixels from disk into a vector ----
    std::vector<uint8_t> srcPixelBuffer(metaData.width * metaData.height * 3);
    file.read(reinterpret_cast<char*>(srcPixelBuffer.data()), srcPixelBuffer.size());
    file.close(); // Done reading the source file completely!

    // ---- Step 4: Setup scaling configurations ----
    resizedData.xscaleFactor = static_cast<float>(metaData.width) / resizedData.newWidth;
    resizedData.yscaleFactor = static_cast<float>(metaData.height) / resizedData.newHeight;

    std::vector<uint8_t> targetPixelBuffer(resizedData.newWidth * resizedData.newHeight * 3);

    // ---- Step 5: Bilinear Interpolation Loop ----
    for (int y = 0; y < resizedData.newHeight; ++y) {
        for (int x = 0; x < resizedData.newWidth; ++x) {
            
            // Map target pixel center back to continuous coordinate space in the source image
            int srcX = static_cast<int>((x + 0.5f) * resizedData.xscaleFactor);
            int srcY = static_cast<int>((y + 0.5f) * resizedData.yscaleFactor);
            
            // Safety clip boundaries to guarantee we stay inside index boundaries
            if (srcX >= metaData.width)  srcX = metaData.width - 1;
            if (srcY >= metaData.height) srcY = metaData.height - 1;
            if (srcX < 0) srcX = 0;
            if (srcY < 0) srcY = 0;

            // Compute 1D structural offsets for both arrays
            int srcIdx = (srcY * metaData.width + srcX) * 3;
            int dstIdx = (y * resizedData.newWidth + x) * 3;
            
            // Map the RGB values over directly
            targetPixelBuffer[dstIdx]     = srcPixelBuffer[srcIdx];     // Red
            targetPixelBuffer[dstIdx + 1] = srcPixelBuffer[srcIdx + 1]; // Green
            targetPixelBuffer[dstIdx + 2] = srcPixelBuffer[srcIdx + 2]; // Blue
        }
    }

    // ---- Step 6: Save the output image ----
    savePPMImage(filename, resizedData, targetPixelBuffer);
}

int main() {
    std::string testDir = "images";
    if (fs::exists(testDir) && fs::is_directory(testDir)) {
        std::cout << "Info: Directory '" << testDir << "' found." << std::endl;
    } else {
        std::cout << "Error: Directory '" << testDir << "' does not exist." << std::endl;
        return 1;
    }

    std::cout << "Scanning folder for files:\n-------------------" << std::endl;
    for (const auto& entry : fs::directory_iterator(testDir)) {
        if (fs::is_regular_file(entry.path())) {
            std::string filename = entry.path().filename().string();
            std::string ext = entry.path().extension().string();

            if (ext == ".ppm" || ext == ".PPM") {
                std::cout << "Processing: " << filename << std::endl;

                ImageMetaData metaData;
                ResizedImageData resizedData;
                
                // Let's run the fully unified pipeline engine
                NNIresizer(entry.path().string(), metaData, resizedData, filename);
                std::cout << "-----------------------------------" << std::endl;
            }
        }
    }
    return 0;
}