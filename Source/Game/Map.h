#pragma once
#include <BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h>
#include <string>
namespace gp {
	typedef unsigned char byte_t;
	class HeightField {
	public:
		HeightField(std::string jpegPath);
		~HeightField();
		byte_t get();//const void* get();
	private:
		//Byte Array
		std::string _jpegPath;
	};

	class Map {
	public:
		Map(int width, int length, std::string jpegPath, int maxHeight, int upAxis);
		Map(int width, int length, std::string jpegPath, int maxHeight);
		~Map();
	private:
		int _maxHeight;
		int _length;
		int _width;
		HeightField _hf;
		btHeightfieldTerrainShape _btHTS;
	};
}