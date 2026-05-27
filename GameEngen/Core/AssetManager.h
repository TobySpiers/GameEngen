#pragma once

#include "ServiceLocator.h"

#include <memory>
#include <string>
#include <unordered_map>

class MeshAsset;
class Shader;
class Texture;

class AssetManager
{
public:
    static AssetManager& Get() { return ServiceLocator::GetAssetManager(); }

public:
    // Returns the cached Texture for the given path, loading from disk on first request.
    // Returns nullptr if the file cannot be read.
    std::shared_ptr<const Texture> GetTexture(const std::string& path);

    // Returns the cached Shader for the given vert/frag pair, compiling on first request.
    // Returns nullptr if either file cannot be read.
    std::shared_ptr<const Shader> GetShader(const std::string& vertPath, const std::string& fragPath);

    // Returns the cached MeshAsset for the given key, or nullptr if not registered.
    std::shared_ptr<const MeshAsset> GetMeshAsset(const std::string& key) const;

    // Stores a mesh asset under the given key, replacing any existing entry.
    void RegisterMeshAsset(const std::string& key, std::shared_ptr<MeshAsset> meshAsset);

    void UnloadAllTextures();
    void UnloadAllShaders();
    void UnloadAllMeshAssets();
    void UnloadAllAssets();

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>>   textures;
    std::unordered_map<std::string, std::shared_ptr<Shader>>    shaders;
    std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshAssets;
};
