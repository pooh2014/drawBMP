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
            std::cerr << "Ошибка: не могу открыть файл!" << std::endl;
            return false;
        }

        // Чтение заголовков
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BITMAPFILEHEADER));
        file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER));

        // Проверка формата файла
        if (fileHeader.bfType != 0x4D42) {  // Проверяем "BM" сигнатуру
            std::cerr << "Ошибка: файл не BMP!" << std::endl;
            return false;
        }

        if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
            std::cerr << "Ошибка: поддерживаются только 24 или 32 битные файлы BMP!" << std::endl;
            return false;
        }

        // Выделение памяти для хранения
        int rowSize = ((infoHeader.biWidth * (infoHeader.biBitCount / 8) + 3) & (~3));
        pixelData = new unsigned char[rowSize * infoHeader.biHeight];

        // Перемещение в начало пиксельных данных и чтение
        file.seekg(fileHeader.bfOffBits, std::ios::beg);
        file.read(reinterpret_cast<char*>(pixelData), rowSize * infoHeader.biHeight);

        return true;
    }

    // Отрисовка изображения
    void displayBMP() {
        if (!pixelData) {
            std::cerr << "Ошибка: отсутствуют данные для отрисовки!" << std::endl;
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
                    std::cerr << "Ошибка: присутствуют цветные пиксели!" << std::endl;
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
    setlocale(LC_ALL, "Russian");

    if (argc != 2) {
        std::cerr << "Не указан путь до картинки BMP!" << std::endl;
        return 1;
    }

    BMPReader* bmpReader = new BMPReader;
    if (!bmpReader->openBMP(argv[1])) {
        std::cout << "ERROR " << std::endl;
        return 1;
    }

    bmpReader->displayBMP();
    bmpReader->closeBMP();

    return 0;
}
