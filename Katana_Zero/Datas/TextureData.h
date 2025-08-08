#pragma once
#include "FileData.h"
class TextureData : public FileData
{
public:
	virtual ~TextureData();

	static string Key()
	{
		return "TexturesData";
	}
	virtual string GetFileName() override;

	virtual void Load(const json& data) override;

	string _type;
	string _name;
	int32 _transparent;
	int32 _width;
	int32 _height;
	int32 _frameCountX;
	int32 _frameCountY;
};

