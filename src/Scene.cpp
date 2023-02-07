#include "Scene.h"

#include <vector>

#include "Scene.h"
#include "Mesh.h"

SceneData Scene::GetData(std::vector<Model> models) {
	std::vector<Vertex> sceneVertices;
	std::vector<unsigned int> sceneIndices;

	for (unsigned int i = 0; i < models.size(); i++) {
		std::vector<Mesh> meshes = models[i].GetMeshes();

		for (unsigned int j = 0; j < meshes.size(); j++) {
			std::vector<Vertex> meshVertices = meshes[j].GetVertices();
			std::vector<unsigned int> meshIndices = meshes[j].GetIndices();

			for (unsigned int k = 0; k < meshIndices.size(); k++) {
				sceneIndices.push_back(meshIndices[k] + sceneVertices.size());
			}
			sceneVertices.insert(sceneVertices.end(), meshVertices.begin(), meshVertices.end());
		}
	}
	return SceneData(sceneVertices, sceneIndices);
}