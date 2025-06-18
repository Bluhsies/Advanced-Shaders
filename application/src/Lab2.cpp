#include "Lab2.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/rotation.hpp"
#include <numeric> // For std::iota
#include <glm/gtc/matrix_transform.hpp>
#include <Grid.hpp>


Lab2::Lab2(GLFWWindowImpl& win) : Layer(win)
{

	//This shader description has been changed to Lab1PhongVert and Frag, because the data inside is for the model as opposed to the floor.
	//Certain values, such as the albedo value, are not used for the model.
	m_mainScene.reset(new Scene);
	m_screenScene.reset(new Scene);
	ShaderDescription phongShaderDesc;
	phongShaderDesc.type = ShaderType::rasterization;
	phongShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/Lab1PhongVert.glsl";
	phongShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/Lab1PhongFrag.glsl";

	//As I am using a separate shader for the floor, I make a new description for the second shader.
	ShaderDescription floorShaderDesc;
	floorShaderDesc.type = ShaderType::rasterization;
	floorShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/PhongVertFloor.glsl";
	floorShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/PhongFragFloor.glsl";

	//Skybox shader.
	ShaderDescription skyboxShaderDesc;
	skyboxShaderDesc.type = ShaderType::rasterization;
	skyboxShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/SkyboxVert.glsl";
	skyboxShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/SkyboxFrag.glsl";

	//Screenpass shader.
	ShaderDescription screenShaderDesc;
	screenShaderDesc.type = ShaderType::rasterization;
	screenShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/FinalPassVert.glsl";
	screenShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/FinalPassFrag.glsl";

	//Setting the grid for the floor.
	std::shared_ptr<Grid> grid = std::make_shared<Grid>();
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	//Setting the shader and VBO layout for the floor.
	std::shared_ptr<Shader> floorShader;
	floorShader = std::make_shared<Shader>(floorShaderDesc);
	VBOLayout floorLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	std::shared_ptr<VAO> floorVAO;
	// use indices when assigning the VAO for the shared pointer.
	floorVAO = std::make_shared<VAO>(floor_indices);
	// Uses floor vertices and the layout.
	floorVAO->addVertexBuffer(floor_vertices, floorLayout);

	//Creating a texture for the floor, using the letterCube texture provided in the labs.
	std::shared_ptr<Texture> floorTexture;
	floorTexture = std::make_shared<Texture>("./assets/models/whiteCube/letterCube.png");

	//Creating the floor texture and sending the values for floorTexture and m_colour to the fragment shader for the floor
	//by setting the value using the setValue function for the floorMaterial.
	std::shared_ptr<Material> floorMaterial;
	floorMaterial = std::make_shared<Material>(floorShader);
	floorMaterial->setValue("u_albedoMap", floorTexture);
	floorMaterial->setValue("u_albedo", m_colour);

	//Creating the actor for the floor and assigning the VAO and Material to the geometry and material, respectively.
	Actor floor;
	floor.geometry = floorVAO;
	floor.material = floorMaterial;
	// Repositions the location of the floor and then recalculates the position by calling recalc().
	floor.translation = glm::vec3(-50.f, -5.f, -50.0);
	floor.recalc();
	//Adds the floor actor to the m_actors vector from the Scene class.
	floorIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(floor);

	// Creating the model shader for the witch. It uses phongShaderDesc, which in turns uses the Lab1Phong and Vert shaders.
	std::shared_ptr<Shader> modelShader;
	modelShader = std::make_shared<Shader>(phongShaderDesc);
	//Setting the VBO layout for the witch model.
	VBOLayout modelLayout = {

		{ GL_FLOAT, 3},
		{ GL_FLOAT,3 },
		{ GL_FLOAT,2 }

	};

	// Creating the model for the witch and creating the modelVAO, assigning the mesh indices to the VAO
	// and the vertices and layout to the vertex buffer.
	Model witchModel("./assets/models/witch/witch.obj");
	std::shared_ptr<VAO> modelVAO;
	modelVAO = std::make_shared<VAO>(witchModel.m_meshes[0].indices);
	modelVAO->addVertexBuffer(witchModel.m_meshes[0].vertices, modelLayout);
	//Creating the texture for the model. The texture uses the diffuse png.
	std::shared_ptr<Texture> modelTexture;
	modelTexture = std::make_shared<Texture>("./assets/models/witch/textures/diffuse.png");
	//Creating the texture for the model. The texture uses the specular png.
	std::shared_ptr<Texture> modelSpecular;
	modelSpecular = std::make_shared<Texture>("./assets/models/witch/textures/specular.png");
	//Creating the texture for the model. The texture uses the normal png.
	std::shared_ptr<Texture> modelNormal;
	modelNormal = std::make_shared<Texture>("./assets/models/witch/textures/normal.png");

	//Creating the material and assigning the diffuse, specular and normal to the modelMaterial. These are sent to the
	// Lab1PhongFrag, which is used for the modelShader.
	std::shared_ptr<Material> modelMaterial;
	modelMaterial = std::make_shared<Material>(modelShader);
	modelMaterial->setValue("u_albedoMap", modelTexture);
	modelMaterial->setValue("u_specularMap", modelSpecular);
	modelMaterial->setValue("u_normalMap", modelNormal);
	//Create the actor, assign the VAO to geometry and the material.
	// Place the actor infront of the camera and scale it to a reasonable size.
	Actor model;
	model.geometry = modelVAO;
	model.material = modelMaterial;
	model.translation = glm::vec3(0.f, -5.f, -11.f);
	model.scale = glm::vec3(5.f, 5.0f, 5.f);
	model.recalc();
	// Add the model to the m_actors vector from the Scene class by pushing it to the back.
	modelIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(model);

	//Create the directional light and add it to the directional lights vector.
	DirectionalLight dl;
	dl.direction = glm::normalize(glm::vec3(-0.2, -1.0, -0.5));
	m_mainScene->m_directionalLights.push_back(dl);

	//Create the pointlight and iterate through a for loop to add each of the (currently 10) point lights to the
	// point lights vector. Currently, the colour and position are randomised through the use of float values
	// but the constants (used for the brightness of the lights) use a set value set in the .hpp file
	// but can be changed via the GUI.
	PointLight pointLight;
	for (int i = 0; i < numPointLights; i++)
	{

		pointLight.colour = glm::vec3(Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0));
		pointLight.position = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), -1.f, Randomiser::uniformFloatBetween(-30.0, 30.0));
		pointLight.constants = m_pointLightConstants;
		m_mainScene->m_pointLights.push_back(pointLight);

	}
	// Create the VBO layout for the skybox.
	VBOLayout skyboxLayout = {

		{ GL_FLOAT, 3}

	};
	// Create the shader for the skybox, using the skyboxShaderDesc from the SkyboxVert and Frag shaders.
	std::shared_ptr<Shader> skyboxShader;
	skyboxShader = std::make_shared<Shader>(skyboxShaderDesc);
	// Set the indices for the skybox.
	std::vector<uint32_t> skyboxIndices(skyboxVertices.size() / 3);
	std::iota(skyboxIndices.begin(), skyboxIndices.end(), 0);
	//Create the VAO for the skybox, using the indices, vertices and layout. (vertices can be located in the .hpp file)
	std::shared_ptr<VAO> skyboxVAO;
	skyboxVAO = std::make_shared<VAO>(skyboxIndices);
	skyboxVAO->addVertexBuffer(skyboxVertices, skyboxLayout);
	// Create the array for the png files to create the skybox cube.
	std::array<const char*, 6> cubeMapPaths = {

		"./assets/textures/Skybox/right.png",
		"./assets/textures/Skybox/left.png",
		"./assets/textures/Skybox/top.png",
		"./assets/textures/Skybox/bottom.png",
		"./assets/textures/Skybox/back.png",
		"./assets/textures/Skybox/front.png",
	};
	std::shared_ptr<CubeMap> cubeMap;
	cubeMap = std::make_shared<CubeMap>(cubeMapPaths, false, false);
	//Create a cubemap and set the cubemap with the values added from cubemappaths.
	//Create the material and set the value for u_skybox for the skybox fragment shader.
	std::shared_ptr<Material> skyboxMaterial;
	skyboxMaterial = std::make_shared<Material>(skyboxShader, "");
	skyboxMaterial->setValue("u_skybox", cubeMap);

	//Create the actor for the skybox and assign the VAO and material for the actor.
	Actor skybox;
	skybox.geometry = skyboxVAO;
	skybox.material = skyboxMaterial;
	// Add the skybox to the m_actors scene and assign the value of u_skyboxView from the fragment shader to be an inverse
	// of the skybox's transform.
	skyIdx = m_mainScene->m_actors.size();
	skyboxMaterial->setValue("u_skyboxView", glm::inverse(skybox.transform));
	m_mainScene->m_actors.push_back(skybox);


	Actor camera;
	cameraIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(camera);

	std::shared_ptr<Shader> screenShader;
	screenShader = std::make_shared<Shader>(screenShaderDesc);

	VBOLayout screenQuadLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout testLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}

	};

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

	std::shared_ptr<VAO> screenQuadVAO;
	screenQuadVAO = std::make_shared<VAO>(screenIndices);
	screenQuadVAO->addVertexBuffer(screenVertices, screenQuadLayout);

	

	/*************************
	*  Main Render PAss
	**************************/
	RenderPass mainPass;
	mainPass.scene = m_mainScene;
	mainPass.parseScene();
	//mainPass.target = std::make_shared<FBO>();
	mainPass.target = std::make_shared<FBO>(m_winRef.getSize(), testLayout);
	mainPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	mainPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_view", mainPass.camera.view);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_projection", mainPass.camera.projection);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.colour", m_mainScene->m_directionalLights.at(0).colour);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	//Creating the screenpass material, actor and render pass.
	std::shared_ptr<Material> screenQuadMaterial;
	screenQuadMaterial = std::make_shared<Material>(screenShader);
	screenQuadMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));

	Actor screen;
	screen.geometry = screenQuadVAO;
	screen.material = screenQuadMaterial;
	screenIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(screen);

	RenderPass screenPass;
	screenPass.scene = m_screenScene;
	screenPass.parseScene();
	screenPass.target = std::make_shared<FBO>();
	screenPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	screenPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	screenPass.UBOmanager.setCachedValue("b_camera2D", "u_view", screenPass.camera.view);
	screenPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", screenPass.camera.projection);

	for (int i = 0; i < numPointLights; i++)
	{

		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_pointLightConstants);

	}



	m_mainScene->m_actors.at(cameraIdx).attachScript<CameraScript>(mainPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f), 0.5f);
	m_mainScene->m_actors.at(modelIdx).attachScript<RotationScript>(mainPass.scene->m_actors.at(modelIdx), glm::vec3(0.f, 0.6f, 0.f), GLFW_KEY_1);


	m_mainRenderer.addRenderPass(mainPass);
	m_mainRenderer.addRenderPass(screenPass);

}

void Lab2::onRender() const
{
	m_mainRenderer.render();
}
void Lab2::onUpdate(float timestep)
{
	//Update the colour of the floor when it is changed in the GUI.
	auto floorMat = m_mainScene->m_actors.at(floorIdx).material;
	floorMat->setValue("u_albedo", m_colour);

	// Update scripts
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onUpdate(timestep);
	}

	if (m_wireFrame) m_mainRenderer.getRenderPass(0).drawInWireFrame = true;
	else             m_mainRenderer.getRenderPass(0).drawInWireFrame = false;

	for (int i = 0; i < numPointLights; i++)
	{

		m_mainRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_pointLightConstants);

	}



	// Update camera  and its position in UBO
	auto& camera = m_mainScene->m_actors.at(cameraIdx);
	auto& pass = m_mainRenderer.getRenderPass(0);
	pass.camera.updateView(camera.transform);
	pass.UBOmanager.setCachedValue("b_camera", "u_view", pass.camera.view);
	pass.UBOmanager.setCachedValue("b_camera", "u_viewPos", camera.translation);

	auto& skybox = m_mainScene->m_actors.at(skyIdx);
	skybox.material->setValue("u_skyboxView", glm::mat4(glm::mat3(pass.camera.view)));


}


void Lab2::onImGUIRender()
{
	float ms = 1000.0f / ImGui::GetIO().Framerate; ;  //milisecconds
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("GAMR3521");
	ImGui::Text("FPS %.3f ms/frame (%.1f FPS)", ms, ImGui::GetIO().Framerate);  // display FPS and ms
	ImGui::ColorEdit3("Cube Colour", (float*)&m_colour);
	ImGui::InputFloat3("Spotlight Adjuster", (float*)&m_pointLightConstants);
	ImGui::Checkbox("Wireframe", &m_wireFrame);
	GLuint textureID = m_mainRenderer.getRenderPass(0).target->getTarget(0)->getID();
	ImVec2 imageSize = ImVec2(256, 256);
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0 , uv1);
	ImGui::End();
	ImGui::Render();
}

void Lab2::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}