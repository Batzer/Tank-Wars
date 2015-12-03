#include "Map.h"

namespace gp {
	Map::Map(int width, int length,std::string jpegPath, int maxHeight,int upAxis) : _hf(jpegPath),_btHTS(width,length,_hf.get(),maxHeight,upAxis,1,0){
		_width		= width;
		_length		= length;
		_maxHeight	= maxHeight;
	}
	Map::Map(int width, int length, std::string jpegPath, int maxHeight) : _hf(jpegPath), _btHTS(width, length, _hf.get(), maxHeight, 1, 1, 0) {
		_width = width;
		_length = length;
		_maxHeight = maxHeight;
	}
	Map::~Map() {}
	HeightField::HeightField(std::string jpegPath) {
		_jpegPath = jpegPath;
	}
	HeightField::~HeightField() {}
	byte_t HeightField::get() {

	}
}