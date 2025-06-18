#pragma once
#include <DemonRenderer.hpp>
#include <Grid.hpp>
#include "Lab4.hpp"

class Lab8 : public Layer
{
public:
	Lab8(GLFWWindowImpl& win);
private:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onKeyPressed(KeyPressedEvent& e) override;
	void onImGUIRender() override;
private:
	std::shared_ptr<Scene> m_mainScene;
	std::shared_ptr<Scene> m_screenScene;
	std::shared_ptr<Scene> m_skyboxScene;

	Renderer m_mainRenderer;
	// Actor positions for ease of use and to avoid additonal magic numbers
	size_t modelIdx;
	size_t skyIdx;
	size_t finalIdx;
	size_t floorIdx;
	size_t cameraIdx;
	size_t invertIdx;
	size_t rlIdx;
	size_t blurIdx;
	size_t vigIdx;
	size_t sepIdx;
	size_t edgeIdx;
	size_t vizDepthIdx;
	size_t fogIdx;
	size_t dofIdx;
	size_t depthFloorIdx;
	size_t depthModelIdx;
	size_t lensIdx;
	size_t shadowIdx;
	size_t shadowVizIdx;
	size_t GBIdx;
	size_t lightIdx;
	size_t bbIdx;

	//Below is a vector containing the positions for the skybox implemented in the in the scene.
	std::vector<float> skyboxVertices = {
		// positions          
		-100.f,  100.f, -100.f,
		-100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		 100.f,  100.f, -100.f,
		-100.f,  100.f, -100.f,

		-100.f, -100.f,  100.f,
		-100.f, -100.f, -100.f,
		-100.f,  100.f, -100.f,
		-100.f,  100.f, -100.f,
		-100.f,  100.f,  100.f,
		-100.f, -100.f,  100.f,

		 100.f, -100.f, -100.f,
		 100.f, -100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f, -100.f,
		 100.f, -100.f, -100.f,

		-100.f, -100.f,  100.f,
		-100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f, -100.f,  100.f,
		-100.f, -100.f,  100.f,

		-100.f,  100.f, -100.f,
		 100.f,  100.f, -100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		-100.f,  100.f,  100.f,
		-100.f,  100.f, -100.f,

		-100.f, -100.f, -100.f,
		-100.f, -100.f,  100.f,
		 100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		-100.f, -100.f,  100.f,
		 100.f, -100.f,  100.f
	};

	//Gui
	glm::vec3 m_colour{ 1.0 };

	// The boolean m_wireFrame is used to hide or show the wireFrame in the scene view, using a checkbox as part of
	// the GUI.
	bool m_wireFrame{ false };

	// The vec3 m_pointLightConstants is used to constantly set the value for the point lights in the scene
	// to the optimal setting. However, these can be changed via the GUI.
	glm::vec3 m_pointLightConstants{ glm::vec3(1.0f, 0.14f, 0.07f) };

	// Here, I am setting the number of point lights in the scene. At the moment, this is 10.
	uint32_t numPointLights = 25;

	uint32_t m_numModels = 10;

	uint32_t numBillboards = 10;
	std::vector<float> billbPositions;
	const std::vector<uint32_t> billboardIndices = { 0,1,2,2,3,0 };

	glm::vec2 screenSize = glm::vec2(m_winRef.getWidth(), m_winRef.getHeight());
	float blurRadius = 5.0f;
	float deThreshold = 0.1f;
	float focusValue = 0.005f;
	float depthFallOff = 10.0f;
	glm::vec3 lightDir;
	ShadowMapVars m_shadowMapVars;
	float near = m_shadowMapVars.orthoSize / 2;
	float far = m_shadowMapVars.orthoSize * 2;
	float xValue = 0.0f;

	DirectionalLight dl;
	float dl1 = -0.2f;
	float dl2 = -1.0f;
	float dl3 = -0.5f;


	float width = m_winRef.getWidthf();
	float height = m_winRef.getHeightf();
	float channels = 0;
	bool isHDR = false;

	// Quad for the passes
	const std::vector<float> screenVertices = {
		//Position         //UV
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, //Bottom left corner
		width, 0.0f, 0.0f, 1.0f, 1.0f, //Bottom right corner
		width, height, 0.0f, 1.0f, 0.0f, //Top right corner
		0.0f, height, 0.0f, 0.0f, 0.0f //Top left corner

	};
	const std::vector<uint32_t> screenIndices = { 0,1,2,2,3,0 };

	float warp = 5.0f;
};
