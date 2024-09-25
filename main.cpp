#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

class BMPReader {
private:
    std::ifstream file;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    unsigned char* pixelData;

public:
    BMPReader() : pixelData(nullptr) {}

    bool openBMP(const std::string& fileName) {
        file.open(fileName, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file!" << std::endl;
            return false;
        }

        // Чтение заголовков
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BITMAPFILEHEADER));
        file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER));

        // Проверка формата файла
        if (fileHeader.bfType != 0x4D42) {  // Проверяем "BM" сигнатуру
            std::cerr << "Error: Not a BMP file!" << std::endl;
            return false;
        }

        if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
            std::cerr << "Error: Only 24 or 32-bit BMP files are supported!" << std::endl;
            return false;
        }

        // Выделение памяти для хранения пикселей
        int rowSize = ((infoHeader.biWidth * (infoHeader.biBitCount / 8) + 3) & (~3));
        pixelData = new unsigned char[rowSize * infoHeader.biHeight];

        // Перемещение в начало пиксельных данных и чтение
        file.seekg(fileHeader.bfOffBits, std::ios::beg);
        file.read(reinterpret_cast<char*>(pixelData), rowSize * infoHeader.biHeight);

        return true;
    }

    void displayBMP() {
        if (!pixelData) {
            std::cerr << "Error: No BMP data to display!" << std::endl;
            return;
        }

        int rowSize = ((infoHeader.biWidth * (infoHeader.biBitCount / 8) + 3) & (~3));

        for (int y = 0; y < infoHeader.biHeight; ++y) {
            for (int x = 0; x < infoHeader.biWidth; ++x) {
                int index = (infoHeader.biHeight - y - 1) * rowSize + x * (infoHeader.biBitCount / 8);
                unsigned char blue = pixelData[index];
                unsigned char green = pixelData[index + 1];
                unsigned char red = pixelData[index + 2];

                // Отображение черного и белого цветов
                if (red == 255 && green == 255 && blue == 255) {
                    std::cout << " "; 
                }
                else if (red == 0 && green == 0 && blue == 0) {
                    std::cout << "#";  
                }
                else {
                    std::cerr << "Error: Unsupported color detected!" << std::endl;
                    return;
                }
            }
            std::cout << std::endl;
        }
    }

    void closeBMP() {
        if (file.is_open()) {
            file.close();
        }
        if (pixelData) {
            delete[] pixelData;
            pixelData = nullptr;
        }
    }

    ~BMPReader() {
        closeBMP();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: drawBmp.exe <path_to_bmp_file>" << std::endl;
        return 1;
    }

    //const char* path = "C:/Users/Professional/Downloads/in.bmp";

    BMPReader* bmpReader = new BMPReader;
    if (!bmpReader->openBMP(argv[1])) {
        std::cout << "ERROR " << *argv << std::endl;
        return 1;
    }

    bmpReader->displayBMP();
    bmpReader->closeBMP();

    return 0;
}
