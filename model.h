#ifndef MODEL_H
#define MODEL_H

#include <QMatrix4x4>
#include <QVector3D>
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
struct Vertex {
    QVector3D position;
    QVector3D normal;
};
class Model
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    QMatrix4x4 modelMatrix;
    QVector3D computeModelCenter();
    bool loadModel(const std::string& path);
private:
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif // MODEL_H
