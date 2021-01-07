/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#include "VULKANPCH.hpp"
#ifndef MESH_HPP
#define MESH_HPP
#pragma once

#include "Device.hpp"
#include "CommandPool.hpp"
#include "Buffer.hpp"
#include <array>

#pragma warning(push)
#pragma warning(disable : 26812 26495 26451)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)

namespace VK
{
    struct Vertex
    {
        glm::vec4 pos;
        glm::vec4 normal;

        bool operator==(const Vertex& other) const
        {
            return pos == other.pos && normal == other.normal;
        }
    };

    typedef std::array<VkVertexInputBindingDescription, 1> BindingDesc;
    typedef std::array<VkVertexInputAttributeDescription, 2>  AttributeDesc;

    class Mesh
    {
    public:
        Mesh() = default;
        void Create(VK::Device& device, VK::CommandPool commandPool, VkQueue graphicsQ, std::string path);
        void ShutDown(VK::Device& device);

        void* Data();
        BindingDesc BindingDescription() const;
        AttributeDesc AttributeDescription() const;
        uint32_t IndexCount() const;
        VK::Buffer* Buffer();
        VK::Buffer* IndexBuffer();

        std::vector<Vertex>* vertices() { return &mVertices; }
        std::vector<uint32_t>* indicies() { return &mIndices; }

    private:
        void GetMesh(aiMesh* mesh);

        BindingDesc  mBindingDescription = {};
        AttributeDesc mAttributeDescriptions = {};

        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        VK::Buffer mVBO;
        VK::Buffer mIBO;
    };
}

#endif
