#include "AssetManager.h"

#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

std::shared_ptr<const Texture> AssetManager::GetTexture(const std::string& path)
{
    auto it = textures.find(path);
    if (it != textures.end())
    {
        return it->second;
    }

    auto texture = std::make_shared<Texture>(path);
    textures[path] = texture;
    return texture;
}

std::shared_ptr<const Shader> AssetManager::GetShader(const std::string& vertPath, const std::string& fragPath)
{
    const std::string key = vertPath + "|" + fragPath;

    auto it = shaders.find(key);
    if (it != shaders.end())
    {
        return it->second;
    }

    auto shader = std::make_shared<Shader>(vertPath, fragPath);
    shaders[key] = shader;
    return shader;
}

void AssetManager::UnloadAllTextures()
{
    textures.clear();
}

void AssetManager::UnloadAllShaders()
{
    shaders.clear();
}

void AssetManager::UnloadAllAssets()
{
    UnloadAllTextures();
    UnloadAllShaders();
}
