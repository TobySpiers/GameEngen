#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class MeshAsset
{
public:
    MeshAsset(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~MeshAsset();

    MeshAsset(const MeshAsset&)            = delete;
    MeshAsset& operator=(const MeshAsset&) = delete;

    // Returns a unit cube (half-extent 0.5) centred at the origin, CCW winding.
    static std::shared_ptr<MeshAsset> CreateCube();

    GLuint GetVao()        const { return vao; }
    int    GetIndexCount() const { return indexCount; }

    bool GetBackfaceCulling() const { return bBackfaceCulling; }

    // Sets backface culling for this asset. Note: this affects ALL objects
    // sharing this MeshAsset, not just the one you have a reference to.
    void SetBackfaceCulling(bool value) { bBackfaceCulling = value; }

private:
    GLuint vao              = 0;
    GLuint vbo              = 0;
    GLuint ebo              = 0;
    int    indexCount       = 0;
    bool   bBackfaceCulling = false;
};
