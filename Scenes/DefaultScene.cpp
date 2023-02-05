#include "DefaultScene.h"

void DefaultScene::SetMathSpheres(ShaderProgram rayTraceShader) {
	for (unsigned int i = 0; i < sizeof(mathSpheres) / sizeof(MathSphere); i++) {
        MathSphere mathSphere = mathSpheres[i];
        std::string mathSphereString = "mathSpheres[" + std::to_string(i) + "]";

        rayTraceShader.SetVec3(mathSphereString + ".position", mathSphere.GetPosition());
        rayTraceShader.SetFloat(mathSphereString + ".radius", mathSphere.GetRadius());
        rayTraceShader.SetVec3(mathSphereString + ".color", mathSphere.material.albedo);
        rayTraceShader.SetFloat(mathSphereString + ".roughness", mathSphere.material.roughness);
        rayTraceShader.SetFloat(mathSphereString + ".metalness", mathSphere.material.metalness);
	}
}
