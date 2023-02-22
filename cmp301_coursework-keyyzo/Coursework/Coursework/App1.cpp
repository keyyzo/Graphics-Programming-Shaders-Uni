// Main project file
// where everything runs from

#include "App1.h"

App1::App1()
{
	// objects
	heightMap = nullptr;
	waterPlane = nullptr;
	orb = nullptr;
	lavaOrb = nullptr;
	orbParticles = nullptr;
	rightPlane = nullptr;
	leftPlane = nullptr;
	backPlane = nullptr;
	frontPlane = nullptr;


	// shaders
	heightMapShader = nullptr;
	waterShader = nullptr;
	orbShader = nullptr;
	waterDepthShader = nullptr;
	depthShader = nullptr;
	textureShader = nullptr;
	horizontalBlurShader = nullptr;
	verticalBlurShader = nullptr;
	depthOfFieldShader = nullptr;
	genericDepthShader = nullptr;
	geometryOrbShader = nullptr;
	shadowShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.
	
	// Load all textures
	textureMgr->loadTexture(L"forestBedHeight", L"res/forestBedHeightMapV3.png");
	textureMgr->loadTexture(L"forestBed", L"res/forestBedTexV3.png");
	textureMgr->loadTexture(L"waterTex", L"res/water-texV3.jpg");
	textureMgr->loadTexture(L"orbTex", L"res/orbTex.png");
	textureMgr->loadTexture(L"lavaOrbTex", L"res/redOrbTex.jpg");
	textureMgr->loadTexture(L"waterBumpMap", L"res/water-bump-map.png");
	textureMgr->loadTexture(L"waterNormalMap", L"res/water-normal-map.png");
	textureMgr->loadTexture(L"grassPlaneTex", L"res/grassTexV2.jpg");
	

	// initialise objects
	heightMap = new TessellatedHeightMap(renderer->getDevice(), renderer->getDeviceContext(),75);
	waterPlane = new TessellatedHeightMap(renderer->getDevice(), renderer->getDeviceContext(), 75);
	orb = new TessellatedOrb(renderer->getDevice(), renderer->getDeviceContext(),10);
	lavaOrb = new TessellatedOrb(renderer->getDevice(), renderer->getDeviceContext(), 10);
	orbParticles = new OrbParticles(renderer->getDevice(), renderer->getDeviceContext(),10);
	rightPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 75);
	leftPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 75);
	backPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 75);
	frontPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 75);


	// initialise shaders
	heightMapShader = new HeightMapShader(renderer->getDevice(), hwnd);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	orbShader = new OrbShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	waterDepthShader = new WaterDepthShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	depthOfFieldShader = new DepthOfFieldShader(renderer->getDevice(), hwnd);
	genericDepthShader = new GenericDepthShader(renderer->getDevice(), hwnd);
	geometryOrbShader = new GeometryOrbShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// initialise ortho meshes
	directionalMapOrtho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, -screenWidth / 2.7, screenHeight / 2.7);

	pointMapOrthoXPositive = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 7, screenHeight / 7, screenWidth / 2.7, 250);
	pointMapOrthoXNegative = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 7, screenHeight / 7, screenWidth / 2.7, 150);
	pointMapOrthoYPositive = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 7, screenHeight / 7, screenWidth / 2.7, 50);
	pointMapOrthoYNegative = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 7, screenHeight / 7, screenWidth / 2.7, -50);
	pointMapOrthoZPositive = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 7, screenHeight / 7, screenWidth / 2.7, -150);
	pointMapOrthoZNegative = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 7, screenHeight / 7, screenWidth / 2.7, -250);

	spotlightOrtho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 7, screenHeight / 7, -100, -250);

	cameraDepthOrtho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 7, screenHeight / 7, 100, -250);

	finalSceneOrtho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);

	// initialise render textures
	nonBlurredTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth / 6, screenHeight / 6, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	upSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	depthOfFieldTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Tessellation variable
	tessellationValue = 1;

	// initialise height map variables
	heightMapDisplacementVal = 20.0f;
	heightMapControls = false;

	// initialise water variables
	waterAmplitude = 1.5f;
	waterFrequency = 0.6f;
	waterSpeed = 0.025f;
	waterPosY = 2.2f;
	waterControls = false;
	
	// initialise blur depth variables
	nearPlane = 1.0f;
	farPlane = 200.0f;
	aperture = 0.9f;
	focalLength = 3.65f;
	planeInFocus = 5.75f;
	blurEnabled = 0.0f;

	// defining shadow map variables
	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;
	int sceneWidth = 150;
	int sceneHeight = 150;

	// initialise shadow maps
	directionalMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	for (int i = 0; i < 6; i++)
	{
		pointMap[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	}
	
	spotlightMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	cameraDepth = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// initialise light variables

	// directional
	directionalLightDirVals = XMFLOAT3(-0.7f, -0.7f, -0.7f);
	directionalLightPosVals = XMFLOAT3(84.5f, 60.0f, 82.5f);

	// orb point light
	orbLightPosition = XMFLOAT3(19.0f, 12.0f, 16.0f);
	orbDirections[0] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	orbDirections[1] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	orbDirections[2] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	orbDirections[3] = XMFLOAT3(0.0f, -1.0f, 0.0f);
	orbDirections[4] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	orbDirections[5] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	orbAttenVals = XMFLOAT3(0.5f, 0.125f, 0.0f);

	// spotlight
	spotlightPosition = XMFLOAT3(65.0f, 12.0f, 55.0f);
	spotlightDirection = XMFLOAT3(0.0f, -1.0f, 0.0f);
	spotlightAttenVals = XMFLOAT3(0.5f, 0.125f, 0.0f);
	spotlightCone = 10.0f;
	spotlightRange = 1000.0f;


	// geometry orb position
	renderOrbVertexNormals = false;
	renderOrbFaceNormals = false;
	vertexNormalSize = 0.5f, faceNormalSize = 2.5f;
	vertexNormalActive = 0.0f, faceNormalActive = 0.0f;

	// initialise lights

	// directional
	directionalLight = new Light();
	directionalLight->setDiffuseColour(1.0f,1.0f,1.0f,1.0f);
	directionalLight->setAmbientColour(0.2f,0.2f,0.2f,1.0f);
	directionalLight->setPosition(directionalLightPosVals.x, directionalLightPosVals.y, directionalLightPosVals.z);
	directionalLight->setDirection(directionalLightDirVals.x, directionalLightDirVals.y, directionalLightDirVals.z);
	directionalLight->generateOrthoMatrix((float)sceneWidth,(float)sceneHeight,0.1f,200.0f);
	
	// orb point light
	orbLight = new Light();
	orbLight->setDiffuseColour(0.45f,0.0f,0.65f,1.0f);
	orbLight->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	orbLight->setPosition(orbLightPosition.x, orbLightPosition.y, orbLightPosition.z);
	orbLight->generateProjectionMatrix(5.0f,200.0f);
	
	// spotlight
	spotLight = new Light();
	spotLight->setDiffuseColour(1.0f, 0.2f, 0.0f, 1.0f);
	spotLight->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	spotLight->setPosition(spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
	spotLight->setDirection(spotlightDirection.x, spotlightDirection.y, spotlightDirection.z);
	spotLight->generateProjectionMatrix(2.0f, 200.0f);
	
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release objects
	if (heightMap)
	{
		delete heightMap;
		heightMap = 0;
	}

	if (waterPlane)
	{
		delete waterPlane;
		waterPlane = 0;
	}

	if (orb)
	{
		delete orb;
		orb = 0;
	}

	if (lavaOrb)
	{
		delete lavaOrb;
		lavaOrb = 0;
	}

	if (orbParticles)
	{
		delete orbParticles;
		orbParticles = 0;
	}

	if (rightPlane)
	{
		delete rightPlane;
		rightPlane = 0;
	}

	if (leftPlane)
	{
		delete leftPlane;
		leftPlane = 0;
	}

	if (backPlane)
	{
		delete backPlane;
		backPlane = 0;
	}

	if (frontPlane)
	{
		delete frontPlane;
		frontPlane = 0;
	}


	// Release shaders
	if (heightMapShader)
	{
		delete heightMapShader;
		heightMapShader = 0;
	}

	if (waterShader)
	{
		delete waterShader;
		waterShader = 0;
	}

	if (orbShader)
	{
		delete orbShader;
		orbShader = 0;
	}

	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}

	if (waterDepthShader)
	{
		delete waterDepthShader;
		waterDepthShader = 0;
	}

	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

	if (horizontalBlurShader)
	{
		delete horizontalBlurShader;
		horizontalBlurShader = 0;
	}

	if (verticalBlurShader)
	{
		delete verticalBlurShader;
		verticalBlurShader = 0;
	}

	if (depthOfFieldShader)
	{
		delete depthOfFieldShader;
		depthOfFieldShader = 0;
	}

	if (genericDepthShader)
	{
		delete genericDepthShader;
		genericDepthShader = 0;
	}

	if (geometryOrbShader)
	{
		delete geometryOrbShader;
		geometryOrbShader = 0;
	}

	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
	}
	
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();

	// update time variables
	waterTime += timer->getTime();

	// update position / direction variables

	// directional light
	directionalLight->setPosition(directionalLightPosVals.x, directionalLightPosVals.y, directionalLightPosVals.z);
	directionalLight->setDirection(directionalLightDirVals.x, directionalLightDirVals.y, directionalLightDirVals.z);

	// orb point light
	orbLight->setPosition(orbLightPosition.x, orbLightPosition.y, orbLightPosition.z);
	

	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}



bool App1::render()
{

	// render full scene
	directionalDepthPass();
	pointDepthPass();
	spotlightDepthPass();
	cameraDepthPass();
	geometryPass();
	upSamplePass();
	horizontalblurPass();
	verticalblurPass();
	downSamplePass();
	depthOfFieldPass();
	finalPassTest();

	return true;
}

// Creates the depth map for the directional light
// by sending the light matrices through to each object's depth shader
// and positioning each object the same way its positioned in the geometry pass

void App1::directionalDepthPass()
{
	directionalMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	renderer->setWireframeMode(false);

	// generates the matrixes used to send through to each object's depth shader

	directionalLight->generateViewMatrix();
	XMMATRIX directionalViewMatrix = directionalLight->getViewMatrix();
	XMMATRIX directionalProjectionMatrix = directionalLight->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// height map creation
	heightMap->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(),worldMatrix, directionalViewMatrix, directionalProjectionMatrix,
		tessellationValue,directionalLight, heightMapDisplacementVal);
	depthShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());

	// water plane creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, waterPosY, 0.0f);
	waterPlane->sendData(renderer->getDeviceContext());
	waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, directionalViewMatrix, directionalProjectionMatrix,
		tessellationValue, waterTime, waterAmplitude, waterFrequency, waterSpeed);
	waterDepthShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());

	// orb creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(orbLightPosition.x, orbLightPosition.y, orbLightPosition.z);
	orb->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, directionalViewMatrix, directionalProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), orb->getIndexCount());

	// lava orb creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
	lavaOrb->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, directionalViewMatrix, directionalProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), lavaOrb->getIndexCount());

	// outskirts of scene planes

	// left plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(-37.0f, 4.17f, -37.0f);
	leftPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, directionalViewMatrix, directionalProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), leftPlane->getIndexCount());

	// right plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(74.0, 4.17f, -37.0f);
	rightPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, directionalViewMatrix, directionalProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), rightPlane->getIndexCount());

	// back plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, -37.0f);
	backPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, directionalViewMatrix, directionalProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), backPlane->getIndexCount());

	// front plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, 74.0f);
	frontPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, directionalViewMatrix, directionalProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), frontPlane->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

// Creates the depth map for the point light
// by sending the light matrices through to each object's depth shader
// and positioning each object the same way its positioned in the geometry pass

void App1::pointDepthPass()
{

	// as the point light travels in each direction
	// six shadow maps need to be calculated in order 
	// to correctly shadow in each direction
	// this requires six of each type of matrix and to run all matrices 
	// into their respective shaders six times

	XMMATRIX pointViewMatrix[6];
	XMMATRIX pointProjectionMatrix[6];
	XMMATRIX worldMatrix[6];

	for (int i = 0; i < 6; i++)
	{
		pointMap[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

		renderer->setWireframeMode(false);

		orbLight->setDirection(orbDirections[i].x,orbDirections[i].y,orbDirections[i].z);

		// generates the matrixes used to send through to each object's depth shader

		orbLight->generateViewMatrix();
		pointViewMatrix[i] = orbLight->getViewMatrix();
		pointProjectionMatrix[i] = orbLight->getProjectionMatrix();
		worldMatrix[i] = renderer->getWorldMatrix();

		pointViewMatrices[i] = pointViewMatrix[i];
		pointProjectionMatrices[i] = pointProjectionMatrix[i];

		// height map creation
		heightMap->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix[i], pointViewMatrix[i], pointProjectionMatrix[i],
			tessellationValue, directionalLight, heightMapDisplacementVal);
		depthShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());

		// water plane creation
		worldMatrix[i] = renderer->getWorldMatrix();
		worldMatrix[i] = XMMatrixTranslation(0.0f, waterPosY, 0.0f);
		waterPlane->sendData(renderer->getDeviceContext());
		waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix[i], pointViewMatrix[i], pointProjectionMatrix[i],
			tessellationValue, waterTime, waterAmplitude, waterFrequency, waterSpeed);
		waterDepthShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());

		// outskirts of scene planes

		// left plane
		worldMatrix[i] = renderer->getWorldMatrix();
		worldMatrix[i] = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(-37.0f, 4.17f, -37.0f);
		leftPlane->sendData(renderer->getDeviceContext());
		genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix[i], pointViewMatrix[i], pointProjectionMatrix[i]);
		genericDepthShader->render(renderer->getDeviceContext(), leftPlane->getIndexCount());

		// right plane
		worldMatrix[i] = renderer->getWorldMatrix();
		worldMatrix[i] = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(74.0, 4.17f, -37.0f);
		rightPlane->sendData(renderer->getDeviceContext());
		genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix[i], pointViewMatrix[i], pointProjectionMatrix[i]);
		genericDepthShader->render(renderer->getDeviceContext(), rightPlane->getIndexCount());

		// back plane
		worldMatrix[i] = renderer->getWorldMatrix();
		worldMatrix[i] = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, -37.0f);
		backPlane->sendData(renderer->getDeviceContext());
		genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix[i], pointViewMatrix[i], pointProjectionMatrix[i]);
		genericDepthShader->render(renderer->getDeviceContext(), backPlane->getIndexCount());

		// front plane
		worldMatrix[i] = renderer->getWorldMatrix();
		worldMatrix[i] = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, 74.0f);
		frontPlane->sendData(renderer->getDeviceContext());
		genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix[i], pointViewMatrix[i], pointProjectionMatrix[i]);
		genericDepthShader->render(renderer->getDeviceContext(), frontPlane->getIndexCount());

		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}

}

// Creates the depth map for the spotlight
// by sending the light matrices through to each object's depth shader
// and positioning each object the same way its positioned in the geometry pass

void App1::spotlightDepthPass()
{
	spotlightMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	renderer->setWireframeMode(false);

	// setting spotlight position and direction
	spotLight->setPosition(spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
	spotLight->setDirection(spotlightDirection.x, spotlightDirection.y, spotlightDirection.z);

	// generates the matrixes used to send through to each object's depth shader

	spotLight->generateViewMatrix();
	XMMATRIX spotlightViewMatrix = spotLight->getViewMatrix();
	XMMATRIX spotlightProjectionMatrix = spotLight->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// height map creation
	
	heightMap->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, spotlightViewMatrix, spotlightProjectionMatrix,
		tessellationValue, directionalLight, heightMapDisplacementVal);
	depthShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());

	// water plane creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, waterPosY, 0.0f);
	waterPlane->sendData(renderer->getDeviceContext());
	waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, spotlightViewMatrix, spotlightProjectionMatrix,
		tessellationValue, waterTime, waterAmplitude, waterFrequency, waterSpeed);
	waterDepthShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());

	// orb creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(orbLightPosition.x, orbLightPosition.y, orbLightPosition.z);
	orb->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), orb->getIndexCount());

	// lava orb creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
	lavaOrb->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), lavaOrb->getIndexCount());

	// outskirts of scene planes

	// left plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(-37.0f, 4.17f, -37.0f);
	leftPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), leftPlane->getIndexCount());

	// right plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(74.0, 4.17f, -37.0f);
	rightPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), rightPlane->getIndexCount());

	// back plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, -37.0f);
	backPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), backPlane->getIndexCount());

	// front plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, 74.0f);
	frontPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, spotlightViewMatrix, spotlightProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), frontPlane->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


// Creates the depth map from the camera
// by sending the light matrices through to each object's depth shader
// and positioning each object the same way its positioned in the geometry pass

void App1::cameraDepthPass()
{
	cameraDepth->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	renderer->setWireframeMode(false);

	// generates the matrixes used to send through to each object's depth shader

	XMMATRIX cameraViewMatrix = camera->getViewMatrix();
	XMMATRIX cameraProjectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	
	// height map creation
	heightMap->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix,
		tessellationValue, directionalLight, heightMapDisplacementVal);
	depthShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());

	// water plane creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, waterPosY, 0.0f);
	waterPlane->sendData(renderer->getDeviceContext());
	waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix,
		tessellationValue, waterTime, waterAmplitude, waterFrequency, waterSpeed);
	waterDepthShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());

	// orb creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(orbLightPosition.x, orbLightPosition.y, orbLightPosition.z);
	orb->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), orb->getIndexCount());
	
	// lava orb creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
	lavaOrb->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), lavaOrb->getIndexCount());

	// outskirts of scene planes

	// left plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(-37.0f, 4.17f, -37.0f);
	leftPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), leftPlane->getIndexCount());

	// right plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(74.0, 4.17f, -37.0f);
	rightPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), rightPlane->getIndexCount());

	// back plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, -37.0f);
	backPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), backPlane->getIndexCount());

	// front plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, 74.0f);
	frontPlane->sendData(renderer->getDeviceContext());
	genericDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	genericDepthShader->render(renderer->getDeviceContext(), frontPlane->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();

}

// creates all the geometry that will make up the scene

void App1::geometryPass()
{

	// save the scene pass to a render texture to be used for post processing
	nonBlurredTexture->setRenderTarget(renderer->getDeviceContext());
	nonBlurredTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	renderer->setWireframeMode(wireframeToggle);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader

	// save point light shadow maps to new texture array
	for (int i = 0; i < 6; i++)
	{
		orbShadowTextures[i] = pointMap[i]->getDepthMapSRV();
	}

	// height map creation
	heightMap->sendData(renderer->getDeviceContext());
	heightMapShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, pointViewMatrices, pointProjectionMatrices, textureMgr->getTexture(L"forestBed"), textureMgr->getTexture(L"forestBedHeight"),
		directionalMap->getDepthMapSRV(), orbShadowTextures, spotlightMap->getDepthMapSRV(), tessellationValue, 
		orbAttenVals, spotlightAttenVals , directionalLight, orbLight, spotLight, spotlightCone, spotlightRange, heightMapDisplacementVal);
	heightMapShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());

	

	// water plane creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, waterPosY, 0.0f);

	renderer->setAlphaBlending(true);

	waterPlane->sendData(renderer->getDeviceContext());
	waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, pointViewMatrices, pointProjectionMatrices, textureMgr->getTexture(L"waterTex"),
		textureMgr->getTexture(L"waterBumpMap"), textureMgr->getTexture(L"waterNormalMap"), directionalMap->getDepthMapSRV(), orbShadowTextures, spotlightMap->getDepthMapSRV(), 
		tessellationValue,
		waterTime, waterAmplitude, waterFrequency, waterSpeed, orbAttenVals, spotlightAttenVals, directionalLight, orbLight, spotLight, spotlightCone, spotlightRange);
	waterShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());

	renderer->setAlphaBlending(false);

	// orb creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(orbLightPosition.x, orbLightPosition.y, orbLightPosition.z);
	orb->sendData(renderer->getDeviceContext());
	orbShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"orbTex"),
		XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 0.0f));
	orbShader->render(renderer->getDeviceContext(), orb->getIndexCount());

	// lava orb creation
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
	lavaOrb->sendData(renderer->getDeviceContext());
	orbShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"lavaOrbTex"),
		XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 0.0f));
	orbShader->render(renderer->getDeviceContext(), lavaOrb->getIndexCount());

	if (renderOrbVertexNormals || renderOrbFaceNormals)
	{
		// geometry orb creation
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(orbLightPosition.x, orbLightPosition.y, orbLightPosition.z);
		orbParticles->sendData(renderer->getDeviceContext());
		geometryOrbShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"orbTex"),
			vertexNormalSize,faceNormalSize,vertexNormalActive,faceNormalActive);
		geometryOrbShader->render(renderer->getDeviceContext(), orbParticles->getIndexCount());
	}
	
	// extra simple planes for showing off shadow better
	
	// left plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f,1.0f,2.0f) *  XMMatrixTranslation(-37.0f, 4.17f, -37.0f);
	leftPlane->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, pointViewMatrices,pointProjectionMatrices,textureMgr->getTexture(L"grassPlaneTex"),
		directionalMap->getDepthMapSRV(), orbShadowTextures, spotlightMap->getDepthMapSRV(), orbAttenVals, spotlightAttenVals, directionalLight, orbLight, spotLight, spotlightCone, spotlightRange);
	shadowShader->render(renderer->getDeviceContext(), leftPlane->getIndexCount());

	// right plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(0.5f, 1.0f, 2.0f) * XMMatrixTranslation(74.0, 4.17f, -37.0f);
	rightPlane->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, pointViewMatrices, pointProjectionMatrices, textureMgr->getTexture(L"grassPlaneTex"),
		directionalMap->getDepthMapSRV(), orbShadowTextures, spotlightMap->getDepthMapSRV(), orbAttenVals, spotlightAttenVals, directionalLight, orbLight, spotLight, spotlightCone, spotlightRange);
	shadowShader->render(renderer->getDeviceContext(), rightPlane->getIndexCount());

	// back plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, -37.0f);
	backPlane->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, pointViewMatrices, pointProjectionMatrices, textureMgr->getTexture(L"grassPlaneTex"),
		directionalMap->getDepthMapSRV(), orbShadowTextures, spotlightMap->getDepthMapSRV(), orbAttenVals, spotlightAttenVals, directionalLight, orbLight, spotLight, spotlightCone, spotlightRange);
	shadowShader->render(renderer->getDeviceContext(), backPlane->getIndexCount());

	// front plane
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 0.5f) * XMMatrixTranslation(0.0f, 4.17f, 74.0f);
	frontPlane->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, pointViewMatrices, pointProjectionMatrices, textureMgr->getTexture(L"grassPlaneTex"),
		directionalMap->getDepthMapSRV(), orbShadowTextures, spotlightMap->getDepthMapSRV(), orbAttenVals, spotlightAttenVals, directionalLight, orbLight, spotLight, spotlightCone, spotlightRange);
	shadowShader->render(renderer->getDeviceContext(), frontPlane->getIndexCount());


	renderer->setBackBufferRenderTarget();
}

// downsamples the render texture from the geometry pass
// to bring down the resolution for the blurring to have more effect

void App1::downSamplePass()
{
	// save the downsample to a render texture to be used for post processing

	downSampleTexture->setRenderTarget(renderer->getDeviceContext());
	downSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);
	
	renderer->setWireframeMode(false);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering
	
	// Render for down sample
	renderer->setZBuffer(false);
	finalSceneOrtho->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, nonBlurredTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), finalSceneOrtho->getIndexCount());
	renderer->setZBuffer(true);
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// applies a horizontal blur to the render texture from the downsample pass
// setting up the vertical blur afterwards to completely blur the scene

void App1::horizontalblurPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// save the horizontal blur to a render texture to be used for post processing 

	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	renderer->setWireframeMode(false);

	// Get matrices
	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();
	
	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	finalSceneOrtho->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, downSampleTexture->getShaderResourceView(), screenSizeX);
	horizontalBlurShader->render(renderer->getDeviceContext(), finalSceneOrtho->getIndexCount());
	renderer->setZBuffer(true);
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// applies a vertical blur to the render texture from the horizontal blur pass
// completely blurs the scene and gets it ready to be upsampled back to full resolution

void App1::verticalblurPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	
	// save the vertical blur to a render texture to be used for post processing 

	float screenSizeY = (float)verticalBlurTexture->getTextureHeight();
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);
	
	renderer->setWireframeMode(false);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();
	
	// Render for Vertical Blur
	renderer->setZBuffer(false);
	finalSceneOrtho->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSizeY);
	verticalBlurShader->render(renderer->getDeviceContext(), finalSceneOrtho->getIndexCount());
	renderer->setZBuffer(true);
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// upsamples the render texture from the vertical blur pass
// to bring up the resolution to complete the full blurring effect

void App1::upSamplePass()
{
	// save the up sample to a render texture to be used for post processing

	upSampleTexture->setRenderTarget(renderer->getDeviceContext());
	upSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);
	
	renderer->setWireframeMode(false);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering
	
	// Render for up sample
	renderer->setZBuffer(false);
	finalSceneOrtho->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, verticalBlurTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), finalSceneOrtho->getIndexCount());
	renderer->setZBuffer(true);
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// applies the depth of field technique to the upsampled blurred scene render texture
// creates an illusion of focus on objects within a set range and blurs the rest of the scene

void App1::depthOfFieldPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// save the depth of field pass to a render texture to be used for post processing

	depthOfFieldTexture->setRenderTarget(renderer->getDeviceContext());
	depthOfFieldTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	renderer->setWireframeMode(false);

	// Get matrices
	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = depthOfFieldTexture->getOrthoMatrix();
	
	// render for depth of field
	renderer->setZBuffer(false);
	finalSceneOrtho->sendData(renderer->getDeviceContext());
	depthOfFieldShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, nonBlurredTexture->getShaderResourceView(),upSampleTexture->getShaderResourceView(),cameraDepth->getDepthMapSRV(),
		nearPlane, farPlane, aperture,focalLength,planeInFocus, blurEnabled);
	depthOfFieldShader->render(renderer->getDeviceContext(),finalSceneOrtho->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore
	renderer->setBackBufferRenderTarget();
}

void App1::finalPassTest()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	renderer->setWireframeMode(false);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// render the final look of the scene

	XMMATRIX OrthoViewMatrixFinal = camera->getOrthoViewMatrix();
	XMMATRIX OrthoMatrixFinal = renderer->getOrthoMatrix();
	renderer->setZBuffer(false);
	finalSceneOrtho->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, OrthoViewMatrixFinal, OrthoMatrixFinal, depthOfFieldTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), finalSceneOrtho->getIndexCount());

	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::TextColored(ImVec4(0.75f, 0.0f, 1.0f, 1.0f), "Main Camera Position");
	ImGui::Text("X: %.2f Y: %.2f Z: %.2f", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::NewLine();
	ImGui::Checkbox("Tessellation Controls", &tessellationControls);
	if (tessellationControls)
	{
		ImGui::TextColored(ImVec4(0.7f, 0.3f, 0.3f, 1.0f), "Tessellation Value");
		ImGui::DragInt("Tessellation Value", &tessellationValue, 1.0f, 1, 10);
	}
	ImGui::NewLine();
	ImGui::Checkbox("Height Map Controls", &heightMapControls);
	if (heightMapControls)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Height Map Displacement Value");
		ImGui::DragFloat("Displacement Value", &heightMapDisplacementVal, 0.5f, 1.0f, 40.0f);
	}
	
	ImGui::NewLine();
	ImGui::Checkbox("Water Controls", &waterControls);
	if (waterControls)
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 1.0f, 1.0f), "Water Values");
		ImGui::DragFloat("Water Amplitude: ", &waterAmplitude, 0.01f, 0.0f, 5.0f);
		ImGui::DragFloat("Water Speed: ", &waterSpeed, 0.001, 0.0f, 0.5f);
		ImGui::SliderFloat("Water Y position: ", &waterPosY, 0.0f, 10.0f);
	}
	ImGui::NewLine();
	ImGui::Checkbox("Directional Light Controls", &directionalLightControls);
	if (directionalLightControls)
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Directional Light");
		ImGui::DragFloat3("Position XYZ", (float*)&directionalLightPosVals, 0.5f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Direction XYZ", (float*)&directionalLightDirVals, 0.1f, -1.0f, 1.0f);
	}
	ImGui::NewLine();
	ImGui::Checkbox("Point Light Controls", &pointLightControls);
	if (pointLightControls)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Orb Light - Point");
		ImGui::DragFloat3("Orb Light Position", (float*)&orbLightPosition, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Orb Light Attenuation Values", (float*)&orbAttenVals, 0.005f, 0.0f, 1.0f);
		ImGui::Checkbox("Render Orb Vertex Normals", &renderOrbVertexNormals);
		if (renderOrbVertexNormals)
		{
			vertexNormalActive = 1.0f;
			ImGui::DragFloat("Vertex Normal Size", &vertexNormalSize, 0.1f, 0.0f, 10.0f);
		}
		else
		{
			vertexNormalActive = 0.0f;
		}
		ImGui::Checkbox("Render Orb Face Normals", &renderOrbFaceNormals);
		if (renderOrbFaceNormals)
		{
			faceNormalActive = 1.0f;
			ImGui::DragFloat("Face Normal Size", &faceNormalSize, 0.1f, 0.0f, 10.0f);
		}
		else
		{
			faceNormalActive = 0.0f;
		}
	}
	ImGui::NewLine();
	ImGui::Checkbox("Spot Light Controls", &spotLightControls);
	if(spotLightControls)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Spotlight");
		ImGui::DragFloat3("Spotight Position", (float*)&spotlightPosition, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Spotlight Direction", (float*)&spotlightDirection, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat3("Spotlight Attenuation Values", (float*)&spotlightAttenVals, 0.005f, 0.0f, 1.0f);
		ImGui::DragFloat("Spotlight Cone", &spotlightCone, 0.1f, 1.0f, 200.0f);
		ImGui::DragFloat("Spotlight Range", &spotlightRange, 1.0f, 1.0f, 2000.0f);
	}
	ImGui::NewLine();
	ImGui::Checkbox("Depth of Field",&blurEnabledBool);
	if (blurEnabledBool)
	{
		blurEnabled = 1.0f;
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Circle of Confusion Values");
		ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, 0.1f, 300.0f);
		ImGui::DragFloat("Far Plane", &farPlane, 0.1f, 1.0f, 300.0f);
		ImGui::DragFloat("Aperture", &aperture, 0.1f, 0.1f, 30.0f);
		ImGui::DragFloat("Focal Length", &focalLength, 0.05f, 0.1f, 30.0f);
		ImGui::DragFloat("Plane in Focus", &planeInFocus, 0.05f, 0.1f, 30.0f);
	}

	else
	{
		blurEnabled = 0.0f;
	}
	ImGui::NewLine();
	ImGui::Checkbox("Directional Light Shadow Map", &directionalMapOrthoActive);
	if (directionalMapOrthoActive)
	{
		ImGui::Begin("Directional Light Shadow Map");
		{
			ImGui::Image((void*)directionalMap->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	ImGui::Checkbox("Point Light Pos X Shadow Map", &pointXPosMapOrthoActive);
	if (pointXPosMapOrthoActive)
	{
		ImGui::Begin("Point Light Pos X Shadow Map");
		{
			ImGui::Image((void*)pointMap[0]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	ImGui::Checkbox("Point Light Neg X Shadow Map", &pointXNegMapOrthoActive);
	if (pointXNegMapOrthoActive)
	{
		ImGui::Begin("Point Light Neg X Shadow Map");
		{
			ImGui::Image((void*)pointMap[1]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	ImGui::Checkbox("Point Light Pos Y Shadow Map", &pointYPosMapOrthoActive);
	if (pointYPosMapOrthoActive)
	{
		ImGui::Begin("Point Light Pos Y Shadow Map");
		{
			ImGui::Image((void*)pointMap[2]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	ImGui::Checkbox("Point Light Neg Y Shadow Map", &pointYNegMapOrthoActive);
	if (pointYNegMapOrthoActive)
	{
		ImGui::Begin("Point Light Neg Y Shadow Map");
		{
			ImGui::Image((void*)pointMap[3]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	ImGui::Checkbox("Point Light Pos Z Shadow Map", &pointZPosMapOrthoActive);
	if (pointZPosMapOrthoActive)
	{
		ImGui::Begin("Point Light Pos Z Shadow Map");
		{
			ImGui::Image((void*)pointMap[4]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	ImGui::Checkbox("Point Light Neg Z Shadow Map", &pointZNegMapOrthoActive);
	if (pointZNegMapOrthoActive)
	{
		ImGui::Begin("Point Light Neg Z Shadow Map");
		{
			ImGui::Image((void*)pointMap[5]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	ImGui::Checkbox("Spotight Shadow Map", &spotlightMapOrthoActive);
	if (spotlightMapOrthoActive)
	{
		ImGui::Begin("Spotight Shadow Map");
		{
			ImGui::Image((void*)spotlightMap->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	ImGui::Checkbox("Camera Depth Shadow Map", &cameraMapOrthoActive);
	if (cameraMapOrthoActive)
	{
		ImGui::Begin("Camera Depth Shadow Map");
		{
			ImGui::Image((void*)cameraDepth->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

