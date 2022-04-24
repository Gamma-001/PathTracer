#pragma once

#include <fstream>
#include <vector>

uint32_t gMod4(uint32_t n) {
	if (n % 4 == 0) return n;
	else return gMod4(n + 1);
}

// total size : 14 bytes
struct BMPfH {
	uint16_t fType = 0x4D42;		// "BM"
	uint32_t fSize = 0;			// size of the file in bytes
	uint16_t reserved1 = 0;			// reserved : 0
	uint16_t reserved2 = 0;			// reserved : 0
	uint32_t offsetData = 0;		// start position of the pixel data

	void writeData(std::fstream &fs) {
		fs.write(reinterpret_cast <char*> (&fType), sizeof(fType));
		fs.write(reinterpret_cast <char*> (&fSize), sizeof(fSize));				// size of the entire file (54 + 16) + sizeof(color data)
		fs.write(reinterpret_cast <char*> (&reserved1), sizeof(reserved1));
		fs.write(reinterpret_cast <char*> (&reserved2), sizeof(reserved2));
		fs.write(reinterpret_cast <char*> (&offsetData), sizeof(offsetData));

	}
};

// total size : 40 bytes
struct BMPiH {
	uint32_t size = 0;				// size of this header in bytes
	uint32_t width = 0;
	uint32_t height = 0;

	uint16_t planes = 1;			// number of planes for target device, always 1
	uint16_t bitCount = 0;			// number of bits per pixel
	uint32_t compression = 0;		// 0 : uncompressed
	uint32_t size_image = 0;		// size of pixel data in bits
	int32_t res_x = 0;
	int32_t res_y = 0;
	
	uint32_t colors_used = 0;		// number of colors in the color pallet
	uint32_t colors_imp = 0;	// number of important colors used

	void writeData(std::fstream &fs) {
		fs.write(reinterpret_cast <char*> (&size), sizeof(size));
		fs.write(reinterpret_cast <char*> (&width), sizeof(width));
		fs.write(reinterpret_cast <char*> (&height), sizeof(height));

		fs.write(reinterpret_cast <char*> (&planes), sizeof(planes));
		fs.write(reinterpret_cast <char*> (&bitCount), sizeof(bitCount));
		fs.write(reinterpret_cast <char*> (&compression), sizeof(compression));
		fs.write(reinterpret_cast <char*> (&size_image), sizeof(size_image));
		fs.write(reinterpret_cast <char*> (&res_x), sizeof(res_x));
		fs.write(reinterpret_cast <char*> (&res_y), sizeof(res_y));

		fs.write(reinterpret_cast <char*> (&colors_used), sizeof(colors_used));
		fs.write(reinterpret_cast <char*> (&colors_imp), sizeof(colors_imp));
	}
};

struct BMPColor_rgb {
	int8_t r;
	int8_t g;
	int8_t b;
};

class Bitmap {
private:
	BMPfH fh;
	BMPiH ih;

	uint32_t padding;

	std::vector <std::vector <BMPColor_rgb>> colorData;
public:
	Bitmap(uint32_t width, uint32_t height) {
		padding = gMod4(3 * width) - 3 * width;

		fh = {0x4D42, 54 + 3 * height * width + height * padding, 0, 0, 54};
		ih = {40, width, height, 1, 24, 0, 3 * height * width + height * padding, 2835, 2835, 0, 0};
	
		// the canvas starts as all black
		colorData = std::vector <std::vector <BMPColor_rgb>> (height , std::vector <BMPColor_rgb> (width, {0x00, 0x00, 0x00}));
	}

	void writeHeader(std::fstream &fs) {
		fh.writeData(fs);
		ih.writeData(fs);
	}

	void writeColors(std::fstream &fs) {
		int8_t zero = 0x00;

		for (int i = 0; i < colorData.size(); i++) {
			// write the colors
			for (int j = 0; j < colorData[i].size(); j++) {
				// colors in a bitmap are written in BGR format
				fs.write(reinterpret_cast <char*> (&colorData[i][j].b), 1);
				fs.write(reinterpret_cast <char*> (&colorData[i][j].g), 1);
				fs.write(reinterpret_cast <char*> (&colorData[i][j].r), 1);
			}

			// write the padding bits
			for (int j = 0; j < padding; j++) {
				fs.write(reinterpret_cast <char*> (&zero), 1);
			}
		}
	}

	std::vector <BMPColor_rgb>& operator [] (int index) {
		return colorData[index];
	}
};
