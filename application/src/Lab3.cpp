#include "Lab3.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/rotation.hpp"
#include <numeric> // For std::iota
#include <glm/gtc/matrix_transform.hpp>
#include <Grid.hpp>

//This lab may need to be run a couple of times, because for some reason the first running
// of the lab doesn't allow the camera to move, but successive iterations of it running are fine.


Lab3::Lab3(GLFWWindowImpl& win) : Layer(win)
{

	m_mainScene.reset(new Scene);
	m_screenScene.reset(new Scene);

	//Depth Pre-Pass
	VBOLayout depthLayout = {

		{GL_FLOAT, 3}

	};

	FBOLayout depthFBOLayout = {

		//{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}

	};

	ShaderDescription depthShaderDesc;
	depthShaderDesc.type = ShaderType::rasterization;
	depthShaderDesc.vertexSrcPath = "./assets/shaders/Lab3/zPrePassVert.glsl";
	depthShaderDesc.fragmentSrcPath = "./assets/shaders/Lab3/zPrePassFrag.glsl";

	std::shared_ptr<Shader> depthShader;
	depthShader = std::make_shared<Shader>(depthShaderDesc);

	std::shared_ptr<Material> depthMaterial;
	depthMaterial = std::make_shared<Material>(depthShader);

	//Making the actors in the scene
	//Setting the grid for the floor.
	std::shared_ptr<Grid> grid = std::make_shared<Grid>();
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	//As I am using a separate shader for the floor, I make a new description for the second shader.
	ShaderDescription floorShaderDesc;
	floorShaderDesc.type = ShaderType::rasterization;
	floorShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/PhongVertFloor.glsl";
	floorShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/PhongFragFloor.glsl";

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
	floorDepthVAO->addVertexBuffer(floor_positions, depthLayout);

	//Creating the actor for the floor and assigning the VAO and Material to the geometry and material, respectively.
	Actor floor;
	floor.geometry = floorVAO;
	floor.depthGeometry = floorDepthVAO;
	floor.material = floorMaterial;
	floor.depthMaterial = depthMaterial;
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
	phongShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/Lab1PhongVert.glsl";
	phongShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/Lab1PhongFrag.glsl";

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

	//Witch model depth VAO
	std::shared_ptr<VAO> modelDepthVAO;
	modelDepthVAO = std::make_shared<VAO>(witchModel.m_meshes[0].indices);
	modelDepthVAO->addVertexBuffer(witchModel.m_meshes[0].positions, depthLayout);

	//Create the actor, assign the VAO to geometry and the material.
	// Place the actor infront of the camera and scale it to a reasonable size.
	//Actor model;
	//model.geometry = modelVAO;
	//model.material = modelMaterial;
	//model.translation = glm::vec3(0.f, -5.f, -11.f);
	//model.scale = glm::vec3(5.f, 5.0f, 5.f);
	//model.recalc();
	//// Add the model to the m_actors vector from the Scene class by pushing it to the back.
	//modelIdx = m_mainScene->m_actors.size();
	//m_mainScene->m_actors.push_back(model);

	Actor model;
	for (int i = 0; i < m_numModels; i++)
	{
		model.geometry = modelVAO;
		model.depthGeometry = modelDepthVAO;
		model.material = modelMaterial;
		model.depthMaterial = depthMaterial;
		model.translation = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), -5.0f, Randomiser::uniformFloatBetween(-30.0, 30.0));
		model.scale = glm::vec3(5.f, 5.0f, 5.f);
		model.recalc();
		// Add the model to the m_actors vector from the Scene class by pushing it to the back.
		modelIdx = m_mainScene->m_actors.size();
		m_mainScene->m_actors.push_back(model);
	}

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

	//Depth pass
	DepthPass prePass;
	prePass.scene = m_mainScene;
	prePass.parseScene();
	prePass.target = std::make_shared<FBO>(m_winRef.getSize(), depthFBOLayout);
	prePass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	prePass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	prePass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	prePass.UBOmanager.setCachedValue("b_camera", "u_view", prePass.camera.view);
	prePass.UBOmanager.setCachedValue("b_camera", "u_projection", prePass.camera.projection);
	prePass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	
	
	m_mainRenderer.addDepthPass(prePass);
	
	modelMaterial->setValue("u_depthMap", prePass.target->getTarget(0));
	floorMaterial->setValue("u_depthMap", prePass.target->getTarget(0));
	
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

	//Invertpass shader.
	ShaderDescription invertShaderDesc;
	invertShaderDesc.type = ShaderType::rasterization;
	invertShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/Lab2IVVert.glsl";
	invertShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/Lab2IVFrag.glsl";

	std::shared_ptr<Shader> invertShader;
	invertShader = std::make_shared<Shader>(invertShaderDesc);

	VBOLayout inverseLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout invertFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}

	};

	std::shared_ptr<VAO> inverseVAO;
	inverseVAO = std::make_shared<VAO>(screenIndices);
	inverseVAO->addVertexBuffer(screenVertices, inverseLayout);

	std::shared_ptr<Material> invertMaterial;
	invertMaterial = std::make_shared<Material>(invertShader);
	invertMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));

	Actor invertScreen;
	invertScreen.geometry = inverseVAO;
	invertScreen.material = invertMaterial;
	invertIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(invertScreen);

	RenderPass invertPass;
	invertPass.scene = m_screenScene;
	invertPass.parseScene();
	invertPass.target = std::make_shared<FBO>(m_winRef.getSize(), invertFBOLayout);
	invertPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	invertPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	invertPass.UBOmanager.setCachedValue("b_camera2D", "u_view", invertPass.camera.view);
	invertPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", invertPass.camera.projection);

	
	m_mainRenderer.addRenderPass(invertPass);
	m_screenScene.reset(new Scene);

	//Relative Luminance Shader
	ShaderDescription rlShaderDesc;
	rlShaderDesc.type = ShaderType::rasterization;
	rlShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/Lab2RLVert.glsl";
	rlShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/Lab2RLFrag.glsl";

	std::shared_ptr<Shader> rlShader;
	rlShader = std::make_shared<Shader>(rlShaderDesc);

	VBOLayout rlLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout rlFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}

	};

	std::shared_ptr<VAO> rlVAO;
	rlVAO = std::make_shared<VAO>(screenIndices);
	rlVAO->addVertexBuffer(screenVertices, rlLayout);

	std::shared_ptr<Material> rlMaterial;
	rlMaterial = std::make_shared<Material>(rlShader);
	rlMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));

	Actor rlScreen;
	rlScreen.geometry = rlVAO;
	rlScreen.material = rlMaterial;
	rlIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(rlScreen);

	RenderPass rlPass;
	rlPass.scene = m_screenScene;
	rlPass.parseScene();
	rlPass.target = std::make_shared<FBO>(m_winRef.getSize(), rlFBOLayout);
	rlPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	rlPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	rlPass.UBOmanager.setCachedValue("b_camera2D", "u_view", rlPass.camera.view);
	rlPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", rlPass.camera.projection);

	m_mainRenderer.addRenderPass(rlPass);
	m_screenScene.reset(new Scene);

	//Vignette shader
	glm::vec2 centreScreen = glm::vec2(0.5f, 0.5f);

	ShaderDescription vigShaderDesc;
	vigShaderDesc.type = ShaderType::rasterization;
	vigShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/Lab2VigVert.glsl";
	vigShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/Lab2VigFrag.glsl";

	std::shared_ptr<Shader> vigShader;
	vigShader = std::make_shared<Shader>(vigShaderDesc);

	VBOLayout vigLayout = {


		{GL_FLOAT, 3},
		{GL_FLOAT, 2}
	};

	FBOLayout vigFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}

	};

	std::shared_ptr<VAO> vigVAO;
	vigVAO = std::make_shared<VAO>(screenIndices);
	vigVAO->addVertexBuffer(screenVertices, vigLayout);

	std::shared_ptr<Material> vigMaterial;
	vigMaterial = std::make_shared<Material>(vigShader);
	vigMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));
	vigMaterial->setValue("screenCentre", centreScreen);

	Actor vigScreen;
	vigScreen.geometry = vigVAO;
	vigScreen.material = vigMaterial;
	vigIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(vigScreen);

	RenderPass vigPass;
	vigPass.scene = m_screenScene;
	vigPass.parseScene();
	vigPass.target = std::make_shared<FBO>(m_winRef.getSize(), vigFBOLayout);
	vigPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	vigPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	vigPass.UBOmanager.setCachedValue("b_camera2D", "u_view", vigPass.camera.view);
	vigPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", vigPass.camera.projection);

	m_mainRenderer.addRenderPass(vigPass);
	m_screenScene.reset(new Scene);

	ShaderDescription sepShaderDesc;
	sepShaderDesc.type = ShaderType::rasterization;
	sepShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/Lab2SepVert.glsl";
	sepShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/Lab2SepFrag.glsl";

	std::shared_ptr<Shader> sepShader;
	sepShader = std::make_shared<Shader>(sepShaderDesc);

	VBOLayout sepLayout = {


		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout sepFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}

	};

	std::shared_ptr<VAO> sepVAO;
	sepVAO = std::make_shared<VAO>(screenIndices);
	sepVAO->addVertexBuffer(screenVertices, sepLayout);

	std::shared_ptr<Material> sepMaterial;
	sepMaterial = std::make_shared<Material>(sepShader);
	sepMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));

	Actor sepScreen;
	sepScreen.geometry = sepVAO;
	sepScreen.material = sepMaterial;
	sepIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(sepScreen);

	RenderPass sepPass;
	sepPass.scene = m_screenScene;
	sepPass.parseScene();
	sepPass.target = std::make_shared<FBO>(m_winRef.getSize(), sepFBOLayout);
	sepPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	sepPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	sepPass.UBOmanager.setCachedValue("b_camera2D", "u_view", sepPass.camera.view);
	sepPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", sepPass.camera.projection);

	m_mainRenderer.addRenderPass(sepPass);
	m_screenScene.reset(new Scene);

	//Detect Edges shader
	ShaderDescription edgeShaderDesc;
	edgeShaderDesc.type = ShaderType::rasterization;
	edgeShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/Lab2DEVert.glsl";
	edgeShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/Lab2DEFrag.glsl";

	std::shared_ptr<Shader> edgeShader;
	edgeShader = std::make_shared<Shader>(edgeShaderDesc);

	VBOLayout edgeLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout edgeFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}

	};

	std::shared_ptr<VAO> edgeVAO;
	edgeVAO = std::make_shared<VAO>(screenIndices);
	edgeVAO->addVertexBuffer(screenVertices, edgeLayout);

	std::shared_ptr<Material> edgeMaterial;
	edgeMaterial = std::make_shared<Material>(edgeShader);
	edgeMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));
	edgeMaterial->setValue("screenSize", screenSize);
	edgeMaterial->setValue("threshold", deThreshold);

	Actor edgeScreen;
	edgeScreen.geometry = edgeVAO;
	edgeScreen.material = edgeMaterial;
	edgeIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(edgeScreen);

	RenderPass edgePass;
	edgePass.scene = m_screenScene;
	edgePass.parseScene();
	edgePass.target = std::make_shared<FBO>(m_winRef.getSize(), edgeFBOLayout);
	edgePass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	edgePass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	edgePass.UBOmanager.setCachedValue("b_camera2D", "u_view", edgePass.camera.view);
	edgePass.UBOmanager.setCachedValue("b_camera2D", "u_projection", edgePass.camera.projection);

	m_mainRenderer.addRenderPass(edgePass);
	m_screenScene.reset(new Scene);

	//Visualize Depth Buffer
	//Simply visualizes the depth information.
	//Makes use of the the FBO attachment type "Depth" being true.
	ShaderDescription vizDepthShaderDesc;
	vizDepthShaderDesc.type = ShaderType::rasterization;
	vizDepthShaderDesc.vertexSrcPath = "./assets/shaders/Lab3/vizDepthVert.glsl";
	vizDepthShaderDesc.fragmentSrcPath = "./assets/shaders/Lab3/vizDepthFrag.glsl";

	std::shared_ptr<Shader> vizDepthShader;
	vizDepthShader = std::make_shared<Shader>(vizDepthShaderDesc);

	VBOLayout vizDepthLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout vizDepthFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}

	};

	std::shared_ptr<VAO> vizDepthVAO;
	vizDepthVAO = std::make_shared<VAO>(screenIndices);
	vizDepthVAO->addVertexBuffer(screenVertices, vizDepthLayout);

	std::shared_ptr<Material> vizDepthMaterial;
	vizDepthMaterial = std::make_shared<Material>(vizDepthShader);
	vizDepthMaterial->setValue("u_originalTexture", mainPass.target->getTarget(1));
	vizDepthMaterial->setValue("near", 0.1f);
	vizDepthMaterial->setValue("far", 1000.f);

	Actor vizDepthScreen;
	vizDepthScreen.geometry = vizDepthVAO;
	vizDepthScreen.material = vizDepthMaterial;
	vizDepthIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(vizDepthScreen);

	RenderPass vizDepthPass;
	vizDepthPass.scene = m_screenScene;
	vizDepthPass.parseScene();
	vizDepthPass.target = std::make_shared<FBO>(m_winRef.getSize(), vizDepthFBOLayout);
	vizDepthPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	vizDepthPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	vizDepthPass.UBOmanager.setCachedValue("b_camera2D", "u_view", vizDepthPass.camera.view);
	vizDepthPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", vizDepthPass.camera.projection);

	m_mainRenderer.addRenderPass(vizDepthPass);
	m_screenScene.reset(new Scene);

	//Fog shader
	//Produces artificial fog in the scene based on depth information.
	//Takes in the depth texture from the main passes FBO.
	ShaderDescription fogShaderDesc;
	fogShaderDesc.type = ShaderType::rasterization;
	fogShaderDesc.vertexSrcPath = "./assets/shaders/Lab3/fogVert.glsl";
	fogShaderDesc.fragmentSrcPath = "./assets/shaders/Lab3/fogFrag.glsl";

	std::shared_ptr<Shader> fogShader;
	fogShader = std::make_shared<Shader>(fogShaderDesc);

	VBOLayout fogLayout = {

		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout fogFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}

	};

	std::shared_ptr<VAO> fogVAO;
	fogVAO = std::make_shared<VAO>(screenIndices);
	fogVAO->addVertexBuffer(screenVertices, fogLayout);

	std::shared_ptr<Material> fogMaterial;
	fogMaterial = std::make_shared<Material>(fogShader);
	fogMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));
	fogMaterial->setValue("u_depthTexture", mainPass.target->getTarget(1));
	fogMaterial->setValue("u_fogColour", glm::vec3(1.0f));
	fogMaterial->setValue("u_near", 0.1f);
	fogMaterial->setValue("u_far", 1000.f);

	Actor fogScreen;
	fogScreen.geometry = fogVAO;
	fogScreen.material = fogMaterial;
	fogIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(fogScreen);

	RenderPass fogPass;
	fogPass.scene = m_screenScene;
	fogPass.parseScene();
	fogPass.target = std::make_shared<FBO>(m_winRef.getSize(), fogFBOLayout);
	fogPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	fogPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	fogPass.UBOmanager.setCachedValue("b_camera2D", "u_view", fogPass.camera.view);
	fogPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", fogPass.camera.projection);

	m_mainRenderer.addRenderPass(fogPass);
	m_screenScene.reset(new Scene);

	//Blur shader
	ShaderDescription blurShaderDesc;
	blurShaderDesc.type = ShaderType::rasterization;
	blurShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/Lab2BlurVert.glsl";
	blurShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/Lab2BlurFrag.glsl";

	std::shared_ptr<Shader> blurShader;
	blurShader = std::make_shared<Shader>(blurShaderDesc);

	VBOLayout blurLayout = {


		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout blurFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}

	};

	std::shared_ptr<VAO> blurVAO;
	blurVAO = std::make_shared<VAO>(screenIndices);
	blurVAO->addVertexBuffer(screenVertices, blurLayout);

	std::shared_ptr<Material> blurMaterial;
	blurMaterial = std::make_shared<Material>(blurShader);
	blurMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));
	blurMaterial->setValue("screenSize", screenSize);
	blurMaterial->setValue("blurRadius", blurRadius);

	Actor blurScreen;
	blurScreen.geometry = blurVAO;
	blurScreen.material = blurMaterial;
	blurIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(blurScreen);

	RenderPass blurPass;
	blurPass.scene = m_screenScene;
	blurPass.parseScene();
	blurPass.target = std::make_shared<FBO>(m_winRef.getSize(), blurFBOLayout);
	blurPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	blurPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	blurPass.UBOmanager.setCachedValue("b_camera2D", "u_view", blurPass.camera.view);
	blurPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", blurPass.camera.projection);

	m_mainRenderer.addRenderPass(blurPass);
	m_screenScene.reset(new Scene);

	//Depth of Field shader
	//This is similar to the blur shader, but the closer the viewer gets to a model, the
	//clearer the model gets, rather than constantly staying blurred.
	//It takes depth texture from the main pass but also samples from the texture from the blur pass.
	ShaderDescription dofShaderDesc;
	dofShaderDesc.type = ShaderType::rasterization;
	dofShaderDesc.vertexSrcPath = "./assets/shaders/Lab3/DOFVert.glsl";
	dofShaderDesc.fragmentSrcPath = "./assets/shaders/Lab3/DOFFrag.glsl";

	std::shared_ptr<Shader> dofShader;
	dofShader = std::make_shared<Shader>(dofShaderDesc);

	VBOLayout dofLayout = {


		{GL_FLOAT, 3},
		{GL_FLOAT, 2}

	};

	FBOLayout dofFBOLayout = {

		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}

	};

	std::shared_ptr<VAO> dofVAO;
	dofVAO = std::make_shared<VAO>(screenIndices);
	dofVAO->addVertexBuffer(screenVertices, dofLayout);

	std::shared_ptr<Material> dofMaterial;
	dofMaterial = std::make_shared<Material>(dofShader);
	dofMaterial->setValue("u_originalTexture", mainPass.target->getTarget(0));
	dofMaterial->setValue("u_depthTexture", mainPass.target->getTarget(1));
	dofMaterial->setValue("u_blurTexture", blurPass.target->getTarget(0));
	dofMaterial->setValue("u_near", 0.1f);
	dofMaterial->setValue("u_far", 1000.f);
	dofMaterial->setValue("u_focus", focusValue);
	dofMaterial->setValue("u_depthFallOff", depthFallOff);

	Actor dofScreen;
	dofScreen.geometry = dofVAO;
	dofScreen.material = dofMaterial;
	dofIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(dofScreen);

	RenderPass dofPass;
	dofPass.scene = m_screenScene;
	dofPass.parseScene();
	dofPass.target = std::make_shared<FBO>(m_winRef.getSize(), dofFBOLayout);
	dofPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };
	dofPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	dofPass.UBOmanager.setCachedValue("b_camera2D", "u_view", dofPass.camera.view);
	dofPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", dofPass.camera.projection);

	m_mainRenderer.addRenderPass(dofPass);
	m_screenScene.reset(new Scene);

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
	finalMaterial->setValue("u_originalTexture", lensPass.target->getTarget(0));

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

void Lab3::onRender() const
{
	m_mainRenderer.render();
}

void Lab3::onUpdate(float timestep)
{

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

void Lab3::onImGUIRender()
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
	GLuint textureID5 = m_mainRenderer.getRenderPass(4).target->getTarget(0)->getID();
	GLuint textureID6 = m_mainRenderer.getRenderPass(5).target->getTarget(0)->getID();
	GLuint textureID7 = m_mainRenderer.getRenderPass(6).target->getTarget(0)->getID();
	GLuint textureID8 = m_mainRenderer.getRenderPass(7).target->getTarget(0)->getID();
	GLuint textureID9 = m_mainRenderer.getRenderPass(8).target->getTarget(0)->getID();
	GLuint textureID10 = m_mainRenderer.getRenderPass(9).target->getTarget(0)->getID();
	GLuint textureID11 = m_mainRenderer.getRenderPass(10).target->getTarget(0)->getID();
	GLuint textureID12 = m_mainRenderer.getRenderPass(11).target->getTarget(0)->getID();
	ImVec2 imageSize = ImVec2(512, 512);
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID2, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID3, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID4, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID5, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID6, imageSize, uv0, uv1);
	if (ImGui::InputFloat("Edge Detection Threshold", (float*)&deThreshold))
	{

		auto& edgeMaterial = m_mainRenderer.getRenderPass(6).scene->m_actors.at(edgeIdx).material;
		edgeMaterial->setValue("threshold", deThreshold);

	}
	ImGui::Image((void*)(intptr_t)textureID7, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID8, imageSize, uv0, uv1);	
	ImGui::Image((void*)(intptr_t)textureID9, imageSize, uv0, uv1);
	if (ImGui::InputFloat("Blur Threshold", (float*)&blurRadius))
	{

		auto& blurMaterial = m_mainRenderer.getRenderPass(9).scene->m_actors.at(blurIdx).material;
		blurMaterial->setValue("blurRadius", blurRadius);

	}
	ImGui::Image((void*)(intptr_t)textureID10, imageSize, uv0, uv1);
	if (ImGui::InputFloat("Depth of Field: Focus", (float*)&focusValue))
	{

		auto& dofMaterial = m_mainRenderer.getRenderPass(10).scene->m_actors.at(dofIdx).material;
		dofMaterial->setValue("u_focus", focusValue);

	}
	if (ImGui::InputFloat("Depth of Field: Depth Falloff", (float*)&depthFallOff))
	{

		auto& dofMaterial = m_mainRenderer.getRenderPass(10).scene->m_actors.at(dofIdx).material;
		dofMaterial->setValue("u_depthFallOff", depthFallOff);

	}
	ImGui::Image((void*)(intptr_t)textureID11, imageSize, uv0, uv1);
	ImGui::Image((void*)(intptr_t)textureID12, imageSize, uv0, uv1);
	ImGui::End();
	ImGui::Render();
}

void Lab3::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}