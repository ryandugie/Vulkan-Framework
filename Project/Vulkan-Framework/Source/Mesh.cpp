/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "VULKANPCH.hpp"
#include "Mesh.hpp"

/*============================================================================*\
|| --------------------------- GLOBAL VARIABLES ----------------------------- ||
\*============================================================================*/

/*============================================================================*\
|| -------------------------- STATIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

/*============================================================================*\
|| -------------------------- PUBLIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  create the mesh
*/
/****************************************************************************/
void VK::Mesh::Create(VK::Device& device, VK::CommandPool commandPool, VkQueue graphicsQ, std::string path)
{
    /* read file via ASSIMP */
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    /* check for errors */
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    /* store data */
    for (unsigned i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[i];
        GetMesh(mesh);
    }

    /* set up shader interface */
    mBindingDescription[0].binding = 0;
    mBindingDescription[0].stride = sizeof(Vertex);
    mBindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    mAttributeDescriptions[0].binding = 0;
    mAttributeDescriptions[0].location = 0;
    mAttributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    mAttributeDescriptions[0].offset = offsetof(Vertex, pos);

    mAttributeDescriptions[1].binding = 0;
    mAttributeDescriptions[1].location = 1;
    mAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    mAttributeDescriptions[1].offset = offsetof(Vertex, normal);

    /* create VBO and IBO */
    VkDeviceSize vertexSize = sizeof(mVertices[0]) * mVertices.size();
    VkDeviceSize indicesSize = sizeof(mIndices[0]) * mIndices.size();

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = vertexSize > indicesSize ? vertexSize : indicesSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VK::Buffer staging;
    staging.Create(device, bufferInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = vertexSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    mVBO.Create(device, bufferInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = indicesSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    mIBO.Create(device, bufferInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    staging.Fill(device, mVertices.data(), vertexSize);
    mVBO.Copy(device, commandPool, graphicsQ, staging, vertexSize);

    staging.Fill(device, mIndices.data(), indicesSize);
    mIBO.Copy(device, commandPool, graphicsQ, staging, indicesSize);
    staging.ShutDown(device);
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::Mesh::ShutDown(VK::Device& device)
{
    mVBO.ShutDown(device);
    mIBO.ShutDown(device);
}

/****************************************************************************/
/*!
\brief
  get vertex info
*/
/****************************************************************************/
void* VK::Mesh::Data()
{
    return mVertices.data();
}

/****************************************************************************/
/*!
\brief
  get the description
*/
/****************************************************************************/
VK::BindingDesc VK::Mesh::BindingDescription() const
{
    return mBindingDescription;
}

/****************************************************************************/
/*!
\brief
  get teh attribute description
*/
/****************************************************************************/
VK::AttributeDesc VK::Mesh::AttributeDescription() const
{
    return mAttributeDescriptions;
}
/****************************************************************************/
/*!
\brief
  get the number of indicies
*/
/****************************************************************************/
uint32_t VK::Mesh::IndexCount() const
{
    return uint32_t(mIndices.size());
}

/****************************************************************************/
/*!
\brief
  get the vbo
*/
/****************************************************************************/
VK::Buffer* VK::Mesh::Buffer()
{
    return &mVBO;
}

/****************************************************************************/
/*!
\brief
  get the index buffer object
*/
/****************************************************************************/
VK::Buffer* VK::Mesh::IndexBuffer()
{
    return &mIBO;
}

/*============================================================================*\
|| ------------------------- PRIVATE FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  Extract the verticies and indecies from the assimp object.
*/
/****************************************************************************/
void VK::Mesh::GetMesh(aiMesh* mesh)
{    // verticies
    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        VK::Vertex vertex;

        // position
        vertex.pos = glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1);

        // normals
        if (mesh->mNormals != nullptr)
        {
            vertex.normal = glm::vec4(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 1);
            vertex.normal = glm::normalize(vertex.normal);
        }

        mVertices.push_back(vertex);
    }

    // indicies
    for (unsigned i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned j = 0; j < face.mNumIndices; ++j)
        {
            mIndices.push_back(face.mIndices[j]);
        }
    }
}
