#include "Lab1.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/rotation.hpp"
#include <numeric> // For std::iota
#include <glm/gtc/matrix_transform.hpp>
#include <Grid.hpp>


Lab1::Lab1(GLFWWindowImpl& win) : Layer(win)
{

	//This shader description has been changed to Lab1PhongVert and Frag, because the data inside is for the model as opposed to the floor.
	//Certain values, such as the albedo value, are not used for the model.
	m_mainScene.reset(new Scene);
	ShaderDescription phongShaderDesc;
	phongShaderDesc.type = ShaderType::rasterization;
	phongShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/Lab1PhongVert.glsl";
	phongShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/Lab1PhongFrag.glsl";

	//As I am using a separate shader for the floor, I make a new description for the second shader.
	ShaderDescription floorShaderDesc;
	floorShaderDesc.type = ShaderType::rasterization;
	floorShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/PhongVertFloor.glsl";
	floorShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/PhongFragFloor.glsl";

	//Skybox shader
	ShaderDescription skyboxShaderDesc;
	skyboxShaderDesc.type = ShaderType::rasterization;
	skyboxShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/SkyboxVert.glsl";
	skyboxShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/SkyboxFrag.glsl";

	//This is the shader for the 3 cube.
	/*ShaderDescription cubeShaderDesc;
	cubeShaderDesc.type = ShaderType::rasterization;
	cubeShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/PhongVert.glsl";
	cubeShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/PhongFrag.glsl";*/

	//Create a grid for the floor shader and make the shader for the floor below.
	std::shared_ptr<Grid> grid = std::make_shared<Grid>();
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	std::shared_ptr<Shader> floorShader;
	floorShader = std::make_shared<Shader>(floorShaderDesc);
	VBOLayout floorLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	//Model floorModel("./assets/models/whiteCube/letterCube.obj");
	std::shared_ptr<VAO> floorVAO;
	// use indices when assigning the VAO for the shared pointer.
	floorVAO = std::make_shared<VAO>(floor_indices);
	// Uses floor vertices and the layout.
	floorVAO->addVertexBuffer(floor_vertices, floorLayout);

	std::shared_ptr<Texture> floorTexture;
	floorTexture = std::make_shared<Texture>("./assets/models/whiteCube/letterCube.png");

	//Make the material and thw actor for the floor.
	std::shared_ptr<Material> floorMaterial;
	floorMaterial = std::make_shared<Material>(floorShader);
	floorMaterial->setValue("u_albedoMap", floorTexture);
	floorMaterial->setValue("u_albedo", m_colour);
	Actor floor;
	floor.geometry = floorVAO;
	floor.material = floorMaterial;
	// Repositions the location of the floor and then recalculates the position by calling recalc().
	floor.translation = glm::vec3(-50.f, -5.f, -50.0);
	floor.recalc();
	floorIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(floor);

	// Creating the model shader for the witch.
	std::shared_ptr<Shader> modelShader;
	modelShader = std::make_shared<Shader>(phongShaderDesc);
	VBOLayout modelLayout = {

		{ GL_FLOAT, 3},
		{ GL_FLOAT, 3},
		{ GL_FLOAT, 2},
		{ GL_FLOAT, 3}

	};

	uint32_t attributeTypes = Model::VertexFlags::positions |
		Model::VertexFlags::normals |
		Model::VertexFlags::uvs |
		Model::VertexFlags::tangents;


	Model witchModel("./assets/models/witch/witch.obj", attributeTypes);
	std::shared_ptr<VAO> modelVAO;
	modelVAO = std::make_shared<VAO>(witchModel.m_meshes[0].indices);
	modelVAO->addVertexBuffer(witchModel.m_meshes[0].vertices, modelLayout);

	std::shared_ptr<Texture> modelTexture;
	modelTexture = std::make_shared<Texture>("./assets/models/witch/textures/diffuse.png");

	std::shared_ptr<Texture> modelSpecular;
	modelSpecular = std::make_shared<Texture>("./assets/models/witch/textures/specular.png");

	std::shared_ptr<Texture> modelNormal;
	modelNormal = std::make_shared<Texture>("./assets/models/witch/textures/normal.png");

	std::shared_ptr<Material> modelMaterial;
	modelMaterial = std::make_shared<Material>(modelShader);
	modelMaterial->setValue("u_albedoMap", modelTexture);
	modelMaterial->setValue("u_specularMap", modelSpecular);
	modelMaterial->setValue("u_normalMap", modelNormal);

	Actor model;
	model.geometry = modelVAO;
	model.material = modelMaterial;
	model.translation = glm::vec3(0.f, -5.f, -11.f);
	model.scale = glm::vec3(5.f, 5.0f, 5.f);
	model.recalc();
	modelIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(model);

	// 3D Cube data
	//std::shared_ptr<Shader> phongShader;
	//phongShader = std::make_shared<Shader>(phongShaderDesc);
	//VBOLayout cubeLayout = {
	//	{GL_FLOAT, 3},  // position
	//	{GL_FLOAT, 3},  // normal
	//	{GL_FLOAT, 2}   // UV
	//};

	//Model cubeModel("./assets/models/whiteCube/letterCube.obj");
	//std::shared_ptr<VAO> cubeVAO;
	//cubeVAO = std::make_shared<VAO>(cubeModel.m_meshes[0].indices);
	//cubeVAO->addVertexBuffer(cubeModel.m_meshes[0].vertices, cubeLayout);

	/*std::shared_ptr<Texture> cubeTexture;
	cubeTexture = std::make_shared<Texture>("./assets/models/whiteCube/letterCube.png");

	std::shared_ptr<Material> cubeMaterial;
	cubeMaterial = std::make_shared<Material>(phongShader);
	cubeMaterial->setValue("u_albedoMap", cubeTexture);
	cubeMaterial->setValue("u_albedo", m_colour);*/
	/*Actor cube;
	cube.geometry = cubeVAO;
	cube.material = cubeMaterial;
	cube.translation = glm::vec3(0.f, 1.f, -6.f);
	cube.recalc();
	modelIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(cube);*/

	

	DirectionalLight dl;
	dl.direction = glm::normalize(glm::vec3(-0.2, -1.0, -0.5));
	m_mainScene->m_directionalLights.push_back(dl);

	PointLight pointLight;
	for (int i = 0; i < numPointLights; i++)
	{

		pointLight.colour = glm::vec3(Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0));
		pointLight.position = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), -1.f, Randomiser::uniformFloatBetween(-30.0, 30.0));
		pointLight.constants = m_pointLightConstants;
		m_mainScene->m_pointLights.push_back(pointLight);

	}

	//Create the skybox shader, using an array for the cubemap positions from the .png files.
	VBOLayout skyboxLayout = {

		{ GL_FLOAT, 3}

	};

	std::shared_ptr<Shader> skyboxShader;
	skyboxShader = std::make_shared<Shader>(skyboxShaderDesc);

	std::vector<uint32_t> skyboxIndices(skyboxVertices.size() / 3);
	std::iota(skyboxIndices.begin(), skyboxIndices.end(), 0);

	std::shared_ptr<VAO> skyboxVAO;
	skyboxVAO = std::make_shared<VAO>(skyboxIndices);
	skyboxVAO->addVertexBuffer(skyboxVertices, skyboxLayout);

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

	std::shared_ptr<Material> skyboxMaterial;
	skyboxMaterial = std::make_shared<Material>(skyboxShader, "");
	skyboxMaterial->setValue("u_skybox", cubeMap);

	Actor skybox;
	skybox.geometry = skyboxVAO;
	skybox.material = skyboxMaterial;
	skyIdx = m_mainScene->m_actors.size();
	skyboxMaterial->setValue("u_skyboxView", glm::inverse(skybox.transform));
	m_mainScene->m_actors.push_back(skybox);


	Actor camera;
	cameraIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(camera);


	/*************************
	*  Main Render PAss
	**************************/
	RenderPass mainPass;
	mainPass.scene = m_mainScene;
	mainPass.parseScene();
	mainPass.target = std::make_shared<FBO>(); ;
	mainPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	mainPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_view", mainPass.camera.view);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_projection", mainPass.camera.projection);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.colour", m_mainScene->m_directionalLights.at(0).colour);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	for (int i = 0; i < numPointLights; i++)
	{

		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);

	}



	m_mainScene->m_actors.at(cameraIdx).attachScript<CameraScript>(mainPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f), 0.5f);
	m_mainScene->m_actors.at(modelIdx).attachScript<RotationScript>(mainPass.scene->m_actors.at(modelIdx), glm::vec3(0.f, 0.6f, 0.f), GLFW_KEY_1);


	m_mainRenderer.addRenderPass(mainPass);

}

void Lab1::onRender() const
{
	m_mainRenderer.render();
}
void Lab1::onUpdate(float timestep)
{
	// per frame uniforms
	/*auto cubeMat = m_mainScene->m_actors.at(modelIdx).material;
	cubeMat->setValue("u_albedo", m_colour);*/

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


void Lab1::onImGUIRender()
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
	ImGui::End();
	ImGui::Render();
}

void Lab1::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}