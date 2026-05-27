#include "MeshAsset.h"

MeshAsset::MeshAsset(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : indexCount(static_cast<int>(indices.size()))
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);

    // Position: vec3 at location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal: vec3 at location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    // Texture coordinate: vec2 at location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)(offsetof(Vertex, texCoord)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

MeshAsset::~MeshAsset()
{
    if (ebo) { glDeleteBuffers(1,      &ebo); ebo = 0; }
    if (vbo) { glDeleteBuffers(1,      &vbo); vbo = 0; }
    if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
}

std::shared_ptr<MeshAsset> MeshAsset::CreateCube()
{
    // 24 vertices (4 per face). Each face has its own normal and UV set.
    // UV mapping: BL=(0,0), BR=(1,0), TR=(1,1), TL=(0,1) — same on all six faces.
    // Winding is CCW when viewed from outside (from the direction of the face normal).
    const std::vector<Vertex> vertices = {
        // Front face (+Z)
        {{ -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }},
        {{  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }},
        {{  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }},
        {{ -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }},
        // Back face (-Z)
        {{  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }},
        {{ -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }},
        {{ -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }},
        {{  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }},
        // Right face (+X)
        {{  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }},
        {{  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }},
        {{  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }},
        {{  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }},
        // Left face (-X)
        {{ -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }},
        {{ -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }},
        {{ -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }},
        {{ -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }},
        // Top face (+Y)
        {{ -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }},
        {{  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }},
        {{  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }},
        {{ -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }},
        // Bottom face (-Y)
        {{ -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }},
        {{  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }},
        {{  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }},
        {{ -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }},
    };

    const std::vector<unsigned int> indices = {
         0,  1,  2,   0,  2,  3,  // Front
         4,  5,  6,   4,  6,  7,  // Back
         8,  9, 10,   8, 10, 11,  // Right
        12, 13, 14,  12, 14, 15,  // Left
        16, 17, 18,  16, 18, 19,  // Top
        20, 21, 22,  20, 22, 23,  // Bottom
    };

    return std::make_shared<MeshAsset>(vertices, indices);
}
