#ifndef ENUM_H
#define ENUM_H

/**
* @enum TEXTURE
* テクスチャ配列の配列番号
* texture.txtに"〇〇,"の形式で追加していくこと
*/
enum TEXTURE
{
#include "texture.txt"
	TEXMAX
};
/**
* @enum FONT
* フォント配列の配列番号
* font.txtに"〇〇,"の形式で追加していくこと
*/
enum FONT {
#include "font.txt"
	FONTMAX
};

#endif