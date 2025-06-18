#pragma once
#include <DemonRenderer.hpp>
#include <Grid.hpp>

//A struct to help make the view and projection matrices, that are used as part of the shadow prepass.
struct ShadowMapVars {

	glm::vec3 centre;
	float distanceAlongLightVector;
	float orthoSize;
	const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

	ShadowMapVars(glm::vec3 c = glm::vec3(0.0), float dist = 60.0f) : centre(c), distanceAlongLightVector(dist) {

		orthoSize = distanceAlongLightVector * 0.75f;

	};

};

class Lab4 : public Layer
{
public:
	Lab4(GLFWWindowImpl& win);
private:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onKeyPressed(KeyPressedEvent& e) override;
	void onImGUIRender() override;
private:
	std::shared_ptr<Scene> m_mainScene;
	std::shared_ptr<Scene> m_screenScene;

	Renderer m_mainRenderer;
	// Actor positions for ease of use and to avoid additonal magic numbers
	// I have made separate variables of size_t for the model (modelIdx), skybox (skyIdx), screenPass (screenIdx) and floor (floorIdx)
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

	// Quad for the passes
	const std::vector<float> screenVertices = {
		//Position         //UV
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, //Bottom left corner
		width, 0.0f, 0.0f, 1.0f, 1.0f, //Bottom right corner
		width, height, 0.0f, 1.0f, 0.0f, //Top right corner
		0.0f, height, 0.0f, 0.0f, 0.0f //Top left corner

	};
	const std::vector<uint32_t> screenIndices = { 0,1,2,2,3,0 };


};
