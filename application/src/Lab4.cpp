#include "Lab4.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/rotation.hpp"
#include <numeric> // For std::iota
#include <glm/gtc/matrix_transform.hpp>
#include <Grid.hpp>


Lab4::Lab4(GLFWWindowImpl& win) : Layer(win)
{

	m_mainScene.reset(new Scene);
	m_screenScene.reset(new Scene);

	//Shadow Pre-Pass
	//This is a depth buffer which is similar to the lab3 version. It only stores depth information,
	//but I utilise this in the model and floor depth material and geometry, to make use of shadows.
	VBOLayout shadowLayout = {

		{GL_FLOAT, 3}

	};

	FBOLayout shadowFBOLayout = {

		{AttachmentType::Depth, true}

	};

	ShaderDescription shadowShaderDesc;
	shadowShaderDesc.type = ShaderType::rasterization;
	shadowShaderDesc.vertexSrcPath = "./assets/shaders/Lab4/Lab4ShadowVert.glsl";
	shadowShaderDesc.fragmentSrcPath = "./assets/shaders/Lab4/Lab4ShadowFrag.glsl";

	std::shared_ptr<Shader> shadowShader;
	shadowShader = std::make_shared<Shader>(shadowShaderDesc);

	std::shared_ptr<Material> shadowMaterial;
	shadowMaterial = std::make_shared<Material>(shadowShader);
	
	//Making the actors in the scene
	//Setting the grid for the floor.
	std::shared_ptr<Grid> grid = std::make_shared<Grid>();
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	//As I am using a separate shader for the floor, I make a new description for the second shader.
	ShaderDescription floorShaderDesc;
	floorShaderDesc.type = ShaderType::rasterization;
	floorShaderDesc.vertexSrcPath = "./assets/shaders/Lab4/Lab4VertFloor.glsl";
	floorShaderDesc.fragmentSrcPath = "./assets/shaders/Lab4/Lab4FragFloor.glsl";

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

	//Floor depth VAO
	std::shared_ptr<VAO> floorDepthVAO;
	floorDepthVAO = std::make_shared<VAO>(floor_indices);
	std::vector<float> floor_positions = grid->getVertexPositions();
	floorDepthVAO->addVertexBuffer(floor_positions, shadowLayout);

	//Creating the actor for the floor and assigning the VAO and Material to the geometry and material, respectively.
	//I also add depthGeometry and depthMaterial, so that shadows are visible in the scene.
	Actor floor;
	floor.geometry = floorVAO;
	floor.depthGeometry = floorDepthVAO;
	floor.material = floorMaterial;
	floor.depthMaterial = shadowMaterial; //floor shadows if needed.
	// Repositions the location of the floor and then recalculates the position by calling recalc().
	floor.translation = glm::vec3(-50.f, -5.f, -50.0);
	floor.recalc();
	//Adds the floor actor to the m_actors vector from the Scene class.
	floorIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(floor);

	//This shader description has been changed to Lab1PhongVert and Frag, because the data inside is for the model as opposed to the floor.
	//Certain values, such as the albedo value, are not used for the model.
	ShaderDescription phongShaderDesc;
	phongShaderDesc.type = ShaderType::rasterization;
	phongShaderDesc.vertexSrcPath = "./assets/shaders/Lab4/Lab4PhongVert.glsl";
	phongShaderDesc.fragmentSrcPath = "./assets/shaders/Lab4/Lab4PhongFrag.glsl";

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

	//I create a new VAO for the model's shadow.
	std::shared_ptr<VAO> modelShadowVAO;
	modelShadowVAO = std::make_shared<VAO>(witchModel.m_meshes[0].indices);
	modelShadowVAO->addVertexBuffer(witchModel.m_meshes[0].positions, shadowLayout);

	//I use a for loop to iterate over each model rendered to the scene. This also includes the
	//dpeth geometry and material from the shadow pre pass.
	Actor model;
	for (int i = 0; i < m_numModels; i++)
	{
		model.geometry = modelVAO;
		model.depthGeometry = modelShadowVAO;
		model.material = modelMaterial;
		model.depthMaterial = shadowMaterial;
		model.translation = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), -5.0f, Randomiser::uniformFloatBetween(-30.0, 30.0));
		model.scale = glm::vec3(5.f, 5.0f, 5.f);
		model.recalc();
		// Add the model to the m_actors vector from the Scene class by pushing it to the back.
		modelIdx = m_mainScene->m_actors.size();
		m_mainScene->m_actors.push_back(model);
	}

	//Create the directional light and add it to the directional lights vector.
	
	dl.direction = glm::normalize(glm::vec3(dl1, dl2, dl3));
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

	//Skybox shader.
	ShaderDescription skyboxShaderDesc;
	skyboxShaderDesc.type = ShaderType::rasterization;
	skyboxShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/SkyboxVert.glsl";
	skyboxShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/SkyboxFrag.glsl";

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

	//Shadow pass
	//Here I create the shadow depth pass. I increase the size of the viewport to a larger size.
	DepthPass shadowPass;
	shadowPass.scene = m_mainScene;
	shadowPass.parseScene();
	shadowPass.target = std::make_shared<FBO>(glm::ivec2(4096, 4096), shadowFBOLayout);
	shadowPass.viewPort = { 0, 0, 4096, 4096 };
	glm::vec2 mapSize(4096.0f, 4096.0f);

	//Here I calculate the shadow passes camera view, using the m_shadowMapVars struct data.
	lightDir = glm::normalize(m_mainScene->m_directionalLights[0].direction);
	glm::vec3 lightPosition = (m_shadowMapVars.centre - lightDir) * m_shadowMapVars.distanceAlongLightVector;
	glm::mat4 shadowView = glm::lookAt(lightPosition, m_shadowMapVars.centre, m_shadowMapVars.UP);
	shadowPass.camera.view = shadowView;

	//Here I calculate the shadow passes camera projection, using the m_shadowMapVars struct data.
	shadowPass.camera.projection = glm::ortho(-m_shadowMapVars.orthoSize,
		m_shadowMapVars.orthoSize,
		-m_shadowMapVars.orthoSize,
		m_shadowMapVars.orthoSize,
		-m_shadowMapVars.orthoSize / 3,
		m_shadowMapVars.orthoSize * 3);
	

	shadowPass.UBOmanager.setCachedValue("b_cameraLight", "u_view", shadowPass.camera.view);
	shadowPass.UBOmanager.setCachedValue("b_cameraLight", "u_projection", shadowPass.camera.projection);

	m_mainRenderer.addDepthPass(shadowPass);

	//I update the model and floor materials, sampling the shadow passes projection * camera for the lightSpaceTransform.
	modelMaterial->setValue("u_lightSpaceTransform", shadowPass.camera.projection* shadowPass.camera.view);
	floorMaterial->setValue("u_lightSpaceTransform", shadowPass.camera.projection* shadowPass.camera.view);

	//I sample from the shadow pass and pass it to the floor and model materials to update the value of u_shadowMap.
	floorMaterial->setValue("u_shadowMap", shadowPass.target->getTarget(0));
	modelMaterial->setValue("u_shadowMap", shadowPass.target->getTarget(0));

	//I set the variable for gMapSize.
	floorMaterial->setValue("gMapSize", mapSize);
	modelMaterial->setValue("gMapSize", mapSize);

	/*************************
	*  Main Render PAss
	**************************/

	FBOLayout mainPassLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}

	};

	RenderPass mainPass;
	mainPass.scene = m_mainScene;
	mainPass.parseScene();
	mainPass.target = std::make_shared<FBO>(m_winRef.getSize(), mainPassLayout);
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
		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_pointLightConstants);

	}

	m_mainRenderer.addRenderPass(mainPass);

	

	//Lens flare shader
	ShaderDescription lensShaderDesc;
	lensShaderDesc.type = ShaderType::rasterization;
	lensShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/Lab2LFVert.glsl";
	lensShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/Lab2LFFrag.glsl";

	std::shared_ptr<Shader> lensShader;
	lensShader = std::make_shared<Shader>(lensShaderDesc);

	VBOLayout lensLayout = {


		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout lensFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}

	};

	std::shared_ptr<VAO> lensVAO;
	lensVAO = std::make_shared<VAO>(screenIndices);
	lensVAO->addVertexBuffer(screenVertices, lensLayout);

	std::shared_ptr<Material> lensMaterial;
	lensMaterial = std::make_shared<Material>(lensShader);
	lensMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));
	lensMaterial->setValue("u_brightness", 0.8f);
	lensMaterial->setValue("u_screenSize", screenSize);
	lensMaterial->setValue("u_Colour", glm::vec3(1.0f, 0.8f, 0.6f));


	Actor lensScreen;
	lensScreen.geometry = lensVAO;
	lensScreen.material = lensMaterial;
	lensIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(lensScreen);

	RenderPass lensPass;
	lensPass.scene = m_screenScene;
	lensPass.parseScene();
	lensPass.target = std::make_shared<FBO>(m_winRef.getSize(), lensFBOLayout);
	lensPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	lensPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	lensPass.UBOmanager.setCachedValue("b_camera2D", "u_view", lensPass.camera.view);
	lensPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", lensPass.camera.projection);

	m_mainRenderer.addRenderPass(lensPass);
	m_screenScene.reset(new Scene);

	//Shadow renderpass
	ShaderDescription shadowVizShaderDesc;
	shadowVizShaderDesc.type = ShaderType::rasterization;
	shadowVizShaderDesc.vertexSrcPath = "./assets/shaders/Lab3/vizDepthVert.glsl";
	shadowVizShaderDesc.fragmentSrcPath = "./assets/shaders/Lab3/vizDepthFrag.glsl";

	std::shared_ptr<Shader> shadowVizShader;
	shadowVizShader = std::make_shared<Shader>(shadowVizShaderDesc);

	std::shared_ptr<Material> shadowVizMaterial;
	shadowVizMaterial = std::make_shared<Material>(shadowVizShader);
	shadowVizMaterial->setValue("u_originalTexture", shadowPass.target->getTarget(0));
	shadowVizMaterial->setValue("near", near);
	shadowVizMaterial->setValue("far", far);

	std::shared_ptr<VAO> shadowVizVAO;
	shadowVizVAO = std::make_shared<VAO>(screenIndices);
	shadowVizVAO->addVertexBuffer(screenVertices, lensLayout);

	Actor shadowVizScreen;
	shadowVizScreen.geometry = shadowVizVAO;
	shadowVizScreen.material = shadowVizMaterial;
	shadowVizIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(shadowVizScreen);

	RenderPass shadowVizPass;
	shadowVizPass.scene = m_screenScene;
	shadowVizPass.parseScene();
	shadowVizPass.target = std::make_shared<FBO>(glm::ivec2(4096, 4096), lensFBOLayout);
	shadowVizPass.viewPort = { 0, 0, 4096, 4096 };

	shadowVizPass.UBOmanager.setCachedValue("b_camera2D", "u_view", lensPass.camera.view);
	shadowVizPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", lensPass.camera.projection);

	m_mainRenderer.addRenderPass(shadowVizPass);
	m_screenScene.reset(new Scene);

	//Finalpass shader.
	ShaderDescription finalShaderDesc;
	finalShaderDesc.type = ShaderType::rasterization;
	finalShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/FinalPassVert.glsl";
	finalShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/FinalPassFrag.glsl";

	std::shared_ptr<Shader> finalShader;
	finalShader = std::make_shared<Shader>(finalShaderDesc);

	VBOLayout finalLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	std::shared_ptr<VAO> finalVAO;
	finalVAO = std::make_shared<VAO>(screenIndices);
	finalVAO->addVertexBuffer(screenVertices, finalLayout);

	std::shared_ptr<Material> finalMaterial;
	finalMaterial = std::make_shared<Material>(finalShader);
	finalMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));

	Actor finalScreen;
	finalScreen.geometry = finalVAO;
	finalScreen.material = finalMaterial;
	finalIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(finalScreen);

	RenderPass finalPass;
	finalPass.scene = m_screenScene;
	finalPass.parseScene();
	finalPass.target = std::make_shared<FBO>();
	finalPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	finalPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	finalPass.UBOmanager.setCachedValue("b_camera2D", "u_view", finalPass.camera.view);
	finalPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", finalPass.camera.projection);

	m_mainRenderer.addRenderPass(finalPass);

	m_mainScene->m_actors.at(cameraIdx).attachScript<CameraScript>(mainPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f), 0.5f);
	m_mainScene->m_actors.at(modelIdx).attachScript<RotationScript>(mainPass.scene->m_actors.at(modelIdx), glm::vec3(0.f, 0.6f, 0.f), GLFW_KEY_1);



}

void Lab4::onRender() const
{
	m_mainRenderer.render();
}

void Lab4::onUpdate(float timestep)
{


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

	//Light Direction
	glm::vec3 lightPosition = (m_shadowMapVars.centre - lightDir) * m_shadowMapVars.distanceAlongLightVector;
	glm::mat4 shadowView = glm::lookAt(lightPosition, m_shadowMapVars.centre, m_shadowMapVars.UP);

	auto& shadowPass = m_mainRenderer.getDepthPass(0);
	shadowPass.camera.view = shadowView;
	shadowPass.UBOmanager.setCachedValue("b_cameraLight", "u_view", shadowView);
	shadowPass.UBOmanager.setCachedValue("b_cameraLight", "u_projection", shadowPass.camera.projection);
	
	//Update the lightspace transform every frame so that when the light's direction is changed via the GUI, the shadows also move.
	auto& floorMat = m_mainScene->m_actors.at(floorIdx).material;
	floorMat->setValue("u_albedo", m_colour);
	floorMat->setValue("u_lightSpaceTransform", shadowPass.camera.projection * shadowPass.camera.view);

	auto& modelMat = m_mainScene->m_actors.at(modelIdx).material;
	modelMat->setValue("u_lightSpaceTransform", shadowPass.camera.projection * shadowPass.camera.view);
}

void Lab4::onImGUIRender()
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
	GLuint textureID2 = m_mainRenderer.getRenderPass(1).target->getTarget(0)->getID();
	GLuint textureID3 = m_mainRenderer.getRenderPass(2).target->getTarget(0)->getID();
	GLuint textureID4 = m_mainRenderer.getRenderPass(3).target->getTarget(0)->getID();
	
	ImVec2 imageSize = ImVec2(512, 512);
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID2, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID3, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID4, imageSize, uv0, uv1);

	if (ImGui::InputFloat("ShadowMapNear", (float*)&near))
	{

		auto& shadowVizMaterial = m_mainRenderer.getRenderPass(3).scene->m_actors.at(shadowVizIdx).material;
		shadowVizMaterial->setValue("near", near);

	}
	if (ImGui::InputFloat("ShadowMapFar", (float*)&far))
	{

		auto& shadowVizMaterial = m_mainRenderer.getRenderPass(3).scene->m_actors.at(shadowVizIdx).material;
		shadowVizMaterial->setValue("far", far);

	}
	ImGui::DragFloat3("Light Direction", (float*)&lightDir, 0.001f, -1.0f, 1.0f); //Changes the direction of the light.
	ImGui::End();
	ImGui::Render();
}

void Lab4::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}