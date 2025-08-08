#include "pch.h"
#include "FileManager.h"
#include "../Datas/FileData.h"
#include "../Datas/TextureData.h"

void FileManager::Init()
{
    // 각종 모든 데이터 로드
    fs::path directory = fs::current_path() / L"../GameResources";
    LoadTextureData(directory);
}

void FileManager::Destroy()
{
    for (auto data : _dataMap)
    {
        SAFE_DELETE(data.second);
    }
    _dataMap.clear();
}

void FileManager::LoadDataObject(fs::path directory, string key, FileData* fileData)
{
    if (_dataMap.find(key) != _dataMap.end())
    {
        // 이미 존재하는 키라면 리턴
        return;
    }

    fs::path path = directory / fileData->GetFileName();
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
        return;
    }

    json data = json::parse(file);
    fileData->Load(data);

    _dataMap.emplace(key, fileData);
}

void FileManager::LoadTextureData(fs::path directory)
{
}
