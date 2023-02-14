#include "Scene.h"

#include <vector>

#include "Scene.h"
#include "Mesh.h"
#include "BVH.h"


void Scene::GenerateSSBOs() {
	SceneData sceneData = GetData();

	std::vector<BVHNode> nodes = BVH(sceneData).GetNodes();

	indexCount = sceneData.indices.size();
	nodeCount = nodes.size();

	glGenBuffers(1, &verticesSSBO);
	glGenBuffers(1, &indicesSSBO);
	glGenBuffers(1, &BVHSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 32 * sceneData.vertices.size(), NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < sceneData.vertices.size(); i++) {
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 32, 12, &(sceneData.vertices[i].pos));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 32 + 16, 12, &(sceneData.vertices[i].normal));
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * sceneData.indices.size(), NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < sceneData.indices.size(); i++) {
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 16, 4, &(sceneData.indices[i]));
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, BVHSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, nodes.size() * 48, NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < nodes.size(); i++) {
		//std::cout << nodes[i].isLeaf << " " << nodes[i].left << " " << nodes[i].right << std::endl;
		
		unsigned int isLeaf = unsigned int(nodes[i].isLeaf);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48, 4, &isLeaf);

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48 + 4, 4, &(nodes[i].left));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48 + 8, 4, &(nodes[i].right));

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48 + 16, 12, &(nodes[i].aabb.cornerMin));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48 + 16 * 2, 12, &(nodes[i].aabb.cornerMax));
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::SetMathSpheres(ShaderProgram rayTraceShader) {
	for (unsigned int i = 0; i < mathSpheres.size(); i++) {
		MathSphere mathSphere = mathSpheres[i];
		std::string mathSphereString = "mathSpheres[" + std::to_string(i) + "]";

		rayTraceShader.SetVec3(mathSphereString + ".position", mathSphere.GetPosition());
		rayTraceShader.SetFloat(mathSphereString + ".radius", mathSphere.GetRadius());
		rayTraceShader.SetVec3(mathSphereString + ".albedo", mathSphere.material.albedo);
		rayTraceShader.SetFloat(mathSphereString + ".roughness", mathSphere.material.roughness);
		rayTraceShader.SetFloat(mathSphereString + ".metalness", mathSphere.material.metalness);
		rayTraceShader.SetVec3(mathSphereString + ".emission", mathSphere.material.emission);

	}
	rayTraceShader.SetUnsignedInt("mathSphereCount", mathSpheres.size());
}

void Scene::SetMeshes(ShaderProgram rayTraceShader) {
	rayTraceShader.BindStorageBlock("Vertices", 0);
	rayTraceShader.BindStorageBlock("Indices", 1);
	rayTraceShader.BindStorageBlock("BVH", 2);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, verticesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, indicesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, BVHSSBO);

	rayTraceShader.SetUnsignedInt("indexCount", indexCount);
	rayTraceShader.SetUnsignedInt("nodeCount", nodeCount);
}

SceneData Scene::GetData() {
	std::vector<Vertex> sceneVertices;
	std::vector<unsigned int> sceneIndices;

	for (unsigned int i = 0; i < models.size(); i++) {
		Model model = models[i];
		std::vector<Mesh> meshes = model.GetMeshes();

		for (unsigned int j = 0; j < meshes.size(); j++) {
			Mesh mesh = meshes[j];

			std::vector<Vertex> meshVertices = mesh.GetVertices();
			std::vector<unsigned int> meshIndices = mesh.GetIndices();

			for (unsigned int k = 0; k < meshIndices.size(); k++) {
				sceneIndices.push_back(meshIndices[k] + sceneVertices.size());
			}
			for (unsigned int k = 0; k < meshVertices.size(); k++) {
				Vertex vertex = meshVertices[k];
				glm::vec3 transformedPosition = model.GetModelMatrix() * glm::vec4(vertex.pos, 1);

				sceneVertices.push_back(Vertex(transformedPosition, vertex.normal));
			}

			//sceneVertices.insert(sceneVertices.end(), meshVertices.begin(), meshVertices.end());
		}
	}
	return SceneData(sceneVertices, sceneIndices);
}