#pragma once
#include <DemonRenderer.hpp>
#include <Grid.hpp>
class Lab2multiPass : public Layer
{
public:
	Lab2multiPass(GLFWWindowImpl& win);
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
	size_t lensIdx;
	size_t thermalIdx;

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
	uint32_t numPointLights = 10;

	glm::vec2 screenSize = glm::vec2(m_winRef.getWidth(), m_winRef.getHeight());
	const float blurRadius = 4.0f;

	float deThreshold = 0.1f;

};
