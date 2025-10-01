#include "model.h"


QVector3D Model::computeModelCenter()
{
    QVector3D minPt(1e9,1e9,1e9);
    QVector3D maxPt(-1e9,-1e9,-1e9);

    for(auto& v: vertices){
        minPt.setX(qMin(minPt.x(), v.position.x()));
        minPt.setY(qMin(minPt.y(), v.position.y()));
        minPt.setZ(qMin(minPt.z(), v.position.z()));

        maxPt.setX(qMax(maxPt.x(), v.position.x()));
        maxPt.setY(qMax(maxPt.y(), v.position.y()));
        maxPt.setZ(qMax(maxPt.z(), v.position.z()));
    }

    return (minPt + maxPt) * 0.5f; // центр AABB
}

bool Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
                                             aiProcess_Triangulate |
                                                 aiProcess_GenSmoothNormals |
                                                 aiProcess_FlipUVs
                                             );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        qDebug() << "Assimp error:" << importer.GetErrorString();
        return false;
    }

    processNode(scene->mRootNode, scene);
    return true;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = QVector3D(mesh->mVertices[i].x,
                                    mesh->mVertices[i].y,
                                    mesh->mVertices[i].z);
        vertex.normal = mesh->HasNormals() ?
                            QVector3D(mesh->mNormals[i].x,
                                      mesh->mNormals[i].y,
                                      mesh->mNormals[i].z) :
                            QVector3D(0,0,0);

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
}
