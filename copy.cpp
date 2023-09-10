#include "Starter.hpp"

// The uniform buffer objects data structures
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)

const int numInstances = 30;
//const int numPointLights = 30;
const float camHeight = 1.85f;

struct MeshUniformBlock {
	alignas(4) float visible;
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat[numInstances];
	alignas(16) glm::mat4 mMat[numInstances];
	alignas(16) glm::mat4 nMat[numInstances];
};

struct PlayerUniformBlock {
	alignas(4) float visible;
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct OverlayUniformBlock {
	alignas(4) float visible;
	alignas(4) float alternative;
};

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec3 PlightPos;
	alignas(16) glm::vec3 PlightColor;
};

// The vertices data structures
struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};

struct VertexVColor {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 color;
};

//Support structures
struct PlayerData{
	int scene;
	glm::vec3 maxVector, minVector;
	glm::vec3 maxVectorWorld, minVectorWorld;
	glm::vec2 angles;
	PlayerUniformBlock ubo;
	float scale;
};


class Project;
void GameLogic(Project *A);
void LevelCreation(Project *A);
PlayerData initData(Model<VertexMesh> &Model, int scene, glm::mat4 world, float scale);
glm::mat4 getWorld(glm::vec3 pos, glm::vec3 rot);

// MAIN ! 
class Project : public BaseProject {
	public:
	//Support variables
	int currentScene = 0;
	int currentPlayer = 0;
	bool detect = false;
	float Ar, currW, currH;
	glm::vec4 viewport;
	glm::vec3 camPos = glm::vec3(9.0f, camHeight, 7.0f);	
	glm::mat4 View, Prj;
	std::vector<PlayerData> playables;

	MeshUniformBlock uboWall, uboCorner, uboBrickWall, uboBrickCorner, uboCellBars, uboFloor;
	Model<VertexMesh> MCorner, MWall, MCellBars, MBrickWall, MBrickCorner;
	Model<VertexVColor> MFloor;
	
	int wallCount, cornerCount, brickWallCount, brickCornerCount, cellBarsCount;

	protected:
	// Descriptor Layouts
	DescriptorSetLayout DSLGubo, DSLMesh, DSLOverlay, DSLVColor, DSLPlayer;

	// Vertex formats
	VertexDescriptor VMesh;
	VertexDescriptor VOverlay;
	VertexDescriptor VVColor;

	// Pipelines
	Pipeline PMesh;
	Pipeline POverlay;
	Pipeline PVColor;
	Pipeline PPlayer;

	// Models, textures and Descriptors
	Model<VertexOverlay> MMenu, MCrosshair;
	Model<VertexMesh> MStatue0, MStatue1, MStatue2, MStatue3;

	DescriptorSet DSGubo, DSMenu, DSCrosshair; 
	DescriptorSet DSWall, DSCorner, DSBrickWall, DSBrickCorner, DSCellBars, DSFloor; 
	DescriptorSet DSStatue0, DSStatue1, DSStatue2, DSStatue3;

	Texture TMenu, TCrosshair, TCrosshairAlt, TVarious, TStone;
 	
	// C++ storage for uniform variables
	
	GlobalUniformBlock gubo;
	OverlayUniformBlock uboMenu, uboCrosshair;


	//Main application parameters
	void setWindowParameters() {
		//Window size, title and initial background
		windowWidth = 800;
		currW = windowWidth;
		windowHeight = 600;
		currH = windowHeight;
		windowTitle = "PROJECT";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		//Descriptor pool sizes
		uniformBlocksInPool = 50;
		texturesInPool = 50;
		setsInPool = 50;
		
		Ar = (float)windowWidth / (float)windowHeight;
		viewport = glm::vec4(0.0f,0.0f,currW,currH);
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		currW = w;
		currH = h;
		Ar = (float)w / (float)h;
		viewport = glm::vec4(0.0f,0.0f,currW,currH);
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLMesh.init(this, {
					// this array contains the bindings:
					// first  element : the binding number
					// second element : the type of element (buffer or texture)
					//                  using the corresponding Vulkan constant
					// third  element : the pipeline stage where it will be used
					//                  using the corresponding Vulkan constant
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});
		DSLPlayer.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});				
		DSLOverlay.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
					{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});

		DSLVColor.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
				});
				
		DSLGubo.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
				});
		


		// Vertex descriptors
		VMesh.init(this, {
				  // this array contains the bindings
				  // first  element : the binding number
				  // second element : the stride of this binging
				  // third  element : whether this parameter change per vertex or per instance
				  //                  using the corresponding Vulkan constant
				  {0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  // this array contains the location
				  // first  element : the binding number
				  // second element : the location number
				  // third  element : the offset of this element in the memory record
				  // fourth element : the data type of the element
				  //                  using the corresponding Vulkan constant
				  // fifth  elmenet : the size in byte of the element
				  // sixth  element : a constant defining the element usage
				  //                   POSITION - a vec3 with the position
				  //                   NORMAL   - a vec3 with the normal vector
				  //                   UV       - a vec2 with a UV coordinate
				  //                   COLOR    - a vec4 with a RGBA color
				  //                   TANGENT  - a vec4 with the tangent vector
				  //                   OTHER    - anything else
				  //
				  // ***************** DOUBLE CHECK ********************
				  //    That the Vertex data structure you use in the "offsetoff" and
				  //	in the "sizeof" in the previous array, refers to the correct one,
				  //	if you have more than one vertex format!
				  // ***************************************************
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
				         sizeof(glm::vec2), UV}
				});

		VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
				         sizeof(glm::vec2), OTHER},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
				         sizeof(glm::vec2), UV}
				});

		VVColor.init(this, {
			{0,sizeof(VertexVColor), VK_VERTEX_INPUT_RATE_VERTEX}
		},{
			{0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(VertexVColor,pos),
				sizeof(glm::vec3), POSITION},
			{0,1,VK_FORMAT_R32G32B32_SFLOAT,offsetof(VertexVColor,norm),
				sizeof(glm::vec3), NORMAL},
			{0,2,VK_FORMAT_R32G32B32_SFLOAT,offsetof(VertexVColor,color),
				sizeof(glm::vec3), COLOR}
		});
		

		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", {&DSLGubo, &DSLMesh});
		PPlayer.init(this, &VMesh, "shaders/PlayerVert.spv", "shaders/PlayerFrag.spv", {&DSLGubo, &DSLPlayer});
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", {&DSLOverlay});
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_ALWAYS, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, true);
		PVColor.init(this, &VVColor, "shaders/VColorVert.spv","shaders/VColorFrag.spv", {&DSLGubo, &DSLVColor});
		PVColor.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, true);
		
		// Create the textures
		TMenu.init(this, "textures/Menu.png");
		TCrosshair.init(this, "textures/Crosshair.png");
		TCrosshairAlt.init(this, "textures/Crosshair1.png");
		TVarious.init(this, "textures/Textures_Food.png");
		TStone.init(this, "textures/stone.jpg");

		// Create the models
		// Menu
		MMenu.vertices.push_back({{-1.0f,-1.0f},{0.0f,0.0f}});
		MMenu.vertices.push_back({{-1.0f,1.0f},{0.0f,1.0f}});	
		MMenu.vertices.push_back({{1.0f,-1.0f},{1.0f,0.0f}});	
		MMenu.vertices.push_back({{1.0f,1.0f},{1.0f,1.0f}});	
		MMenu.indices.push_back(0); MMenu.indices.push_back(1); MMenu.indices.push_back(2); 
		MMenu.indices.push_back(1); MMenu.indices.push_back(2); MMenu.indices.push_back(3);
		MMenu.initMesh(this, &VOverlay);
		//Crosshair
		MCrosshair.vertices.push_back({{-0.075f,-0.1f},{0.0f,0.0f}});
		MCrosshair.vertices.push_back({{-0.075f,0.1f},{0.0f,1.0f}});	
		MCrosshair.vertices.push_back({{0.075f,-0.1f},{1.0f,0.0f}});	
		MCrosshair.vertices.push_back({{0.075f,0.1f},{1.0f,1.0f}});
		MCrosshair.indices.push_back(0); MCrosshair.indices.push_back(1); MCrosshair.indices.push_back(2);
		MCrosshair.indices.push_back(1); MCrosshair.indices.push_back(2); MCrosshair.indices.push_back(3);
		MCrosshair.initMesh(this, &VOverlay);

		//Create Playable models
		//Player 0 - Starting player
		MStatue0.init(this, &VMesh, "models/statue.obj", OBJ);
		playables.push_back(initData(MStatue0,1,getWorld(camPos,glm::vec3(0)),0.1));
		playables[0].ubo.visible = 0.0f;

		//Player 1
		MStatue1.init(this, &VMesh, "models/statue.obj", OBJ);
		glm::mat4 W = getWorld(glm::vec3(-1.3f,0,5.f), glm::vec3(0,glm::radians(90.0f),0));		 
		playables.push_back(initData(MStatue1,1,W,0.1));
		playables[1].angles=glm::vec2(glm::radians(90.f),0);

		//Player 2
		MStatue2.init(this, &VMesh, "models/statue.obj", OBJ);
		W = getWorld(glm::vec3(-2.4f,0,-8.3f), glm::vec3(0,glm::radians(180.0f),0));
		playables.push_back(initData(MStatue2,1,W,0.1));
		playables[2].angles=glm::vec2(glm::radians(180.f),0);

		//Player 3
		MStatue3.init(this, &VMesh, "models/statue.obj", OBJ);
		W = getWorld(glm::vec3(7.6f,2.5f,-8.6f), glm::vec3(0,glm::radians(90.0f),0));
		playables.push_back(initData(MStatue3,1,W,0.1));
		playables[3].angles=glm::vec2(glm::radians(90.f),0);
		
		//Level scenery objects
		LevelCreation(this);
		MFloor.initMesh(this, &VVColor);
		MCorner.init(this, &VMesh, "models/tunnel/tunnel.029_Mesh.6128.mgcg", MGCG);
		MWall.init(this, &VMesh, "models/tunnel/tunnel.005_Mesh.7961.mgcg", MGCG);
		MBrickWall.init(this, &VMesh, "models/tunnel/tunnel.031_Mesh.7927.mgcg", MGCG);
		MBrickCorner.init(this, &VMesh, "models/tunnel/tunnel.030_Mesh.7968.mgcg", MGCG);
		MCellBars.init(this, &VMesh, "models/tunnel/tunnel.033_Mesh.6508.mgcg", MGCG);
	}
	

	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		std::cout<<"Initializing pipelines and sets...";
		PMesh.create();
		POverlay.create();
		PVColor.create();
		PPlayer.create();
		
		//Overlay
		DSMenu.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TMenu},
					{2, TEXTURE, 0, &TMenu}
				});
		DSCrosshair.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TCrosshair},
					{2, TEXTURE, 0, &TCrosshairAlt}
				});

		
		//Playables
		DSStatue0.init(this, &DSLPlayer, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TStone}
				});
		DSStatue1.init(this, &DSLPlayer, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TStone}
		});
		DSStatue2.init(this, &DSLPlayer, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TStone}
				});
		DSStatue3.init(this, &DSLPlayer, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TStone}
		});

		//Scenery - Mesh
		DSCorner.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TVarious}
		});	
		DSWall.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TVarious}
		});	
		DSCellBars.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TVarious}
		});	
		DSBrickWall.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TVarious}
		});
		DSBrickCorner.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TVarious}
		});										
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
				});

		//Scenery - VColor
		DSFloor.init(this, &DSLVColor, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr}
		});


		std::cout<<"done!"<<std::endl;		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PMesh.cleanup();
		POverlay.cleanup();
		PVColor.cleanup();
		PPlayer.cleanup();

		// Cleanup datasets
		DSMenu.cleanup();
		DSCrosshair.cleanup();
		DSStatue0.cleanup();
		DSStatue1.cleanup();
		DSStatue2.cleanup();
		DSStatue3.cleanup();
		DSCorner.cleanup();
		DSWall.cleanup();
		DSFloor.cleanup();
		DSCellBars.cleanup();
		DSBrickWall.cleanup();
		DSBrickCorner.cleanup();
		DSGubo.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		TMenu.cleanup();
		TCrosshair.cleanup();
		TCrosshairAlt.cleanup();
		TVarious.cleanup();
		TStone.cleanup();

		// Cleanup models
		MMenu.cleanup();
		MCrosshair.cleanup();
		MStatue0.cleanup();
		MStatue1.cleanup();
		MStatue2.cleanup();
		MStatue3.cleanup();
		MCorner.cleanup();
		MWall.cleanup();
		MCellBars.cleanup();
		MFloor.cleanup();
		MBrickWall.cleanup();
		MBrickCorner.cleanup();

		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
		DSLOverlay.cleanup();
		DSLVColor.cleanup();
		DSLPlayer.cleanup();
		DSLGubo.cleanup();
		
		// Destroies the pipelines
		PMesh.destroy();		
		POverlay.destroy();
		PVColor.destroy();
		PPlayer.destroy();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		switch(currentScene){
			case 0:
				POverlay.bind(commandBuffer);
				MMenu.bind(commandBuffer);
				DSMenu.bind(commandBuffer,POverlay,0,currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MMenu.indices.size()),1,0,0,0);
				break;
			case 1:
				//Player
				DSGubo.bind(commandBuffer, PPlayer, 0, currentImage);
				PPlayer.bind(commandBuffer);
				MStatue0.bind(commandBuffer);
				DSStatue0.bind(commandBuffer, PPlayer, 1, currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MStatue0.indices.size()), 1, 0, 0, 0);
				MStatue1.bind(commandBuffer);
				DSStatue1.bind(commandBuffer, PPlayer, 1, currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MStatue1.indices.size()), 1, 0, 0, 0);
				MStatue2.bind(commandBuffer);
				DSStatue2.bind(commandBuffer, PPlayer, 1, currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MStatue2.indices.size()), 1, 0, 0, 0);
				MStatue3.bind(commandBuffer);
				DSStatue3.bind(commandBuffer, PPlayer, 1, currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MStatue3.indices.size()), 1, 0, 0, 0);


				//Mesh
				DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
				PMesh.bind(commandBuffer);
				MCorner.bind(commandBuffer);
				DSCorner.bind(commandBuffer, PMesh, 1, currentImage);	
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MCorner.indices.size()), cornerCount, 0, 0, 0);
				MBrickCorner.bind(commandBuffer);
				DSBrickCorner.bind(commandBuffer, PMesh, 1, currentImage);	
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MBrickCorner.indices.size()), brickCornerCount, 0, 0, 0);
				MWall.bind(commandBuffer);
				DSWall.bind(commandBuffer, PMesh, 1, currentImage);	
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MWall.indices.size()), wallCount, 0, 0, 0);
				MCellBars.bind(commandBuffer);
				DSCellBars.bind(commandBuffer, PMesh, 1, currentImage);	
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MCellBars.indices.size()), cellBarsCount, 0, 0, 0);
				MBrickWall.bind(commandBuffer);
				DSBrickWall.bind(commandBuffer, PMesh, 1, currentImage);	
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MBrickWall.indices.size()), brickWallCount, 0, 0, 0);	
				
				//VColor
				DSGubo.bind(commandBuffer,PVColor,0,currentImage);
				PVColor.bind(commandBuffer);
				MFloor.bind(commandBuffer);
				DSFloor.bind(commandBuffer,PVColor,1,currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MFloor.indices.size()),1,0,0,0);			

				//Overlay
				POverlay.bind(commandBuffer);
				MCrosshair.bind(commandBuffer);
				DSCrosshair.bind(commandBuffer,POverlay,0,currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MCrosshair.indices.size()),1,0,0,0);
				break;
		}
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		GameLogic(this);

		//Update Gubo
		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;
		gubo.PlightColor = glm::vec3(1.0, 0.5, 0.0);
		gubo.PlightPos = glm::vec3(-3.6,1.75,-0.5); 
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		//Update uniforms for all object, based on the current scene
		switch(currentScene){
			case 0: //Start Menu
				uboMenu.visible = 1.0f;
				DSMenu.map(currentImage, &uboMenu, sizeof(uboMenu), 0);
				break;
			case 1: //Level 1

				//Scenery
				uboFloor.mvpMat[0] = Prj * View * uboFloor.mMat[0];	
				DSFloor.map(currentImage, &uboFloor, sizeof(uboFloor), 0);

				for(int i = 0; i<cornerCount; i++){
					uboCorner.mvpMat[i] = Prj * View * uboCorner.mMat[i];
				}
				DSCorner.map(currentImage, &uboCorner, sizeof(uboCorner), 0);

				for(int i = 0; i<brickCornerCount; i++){
					uboBrickCorner.mvpMat[i] = Prj * View * uboBrickCorner.mMat[i];
				}
				DSBrickCorner.map(currentImage, &uboBrickCorner, sizeof(uboBrickCorner), 0);

				for(int i = 0; i<wallCount; i++){
					uboWall.mvpMat[i] = Prj * View * uboWall.mMat[i];
				}
				DSWall.map(currentImage, &uboWall, sizeof(uboWall), 0);

				for(int i = 0; i<brickWallCount; i++){
					uboBrickWall.mvpMat[i] = Prj * View * uboBrickWall.mMat[i];
				}
				DSBrickWall.map(currentImage, &uboBrickWall, sizeof(uboBrickWall), 0);


				uboCellBars.mvpMat[0] = Prj * View *  uboCellBars.mMat[0];
				uboCellBars.mvpMat[1] = Prj * View *  uboCellBars.mMat[1];
				DSCellBars.map(currentImage, &uboCellBars, sizeof(uboCellBars), 0);

				//Playables				
				playables[0].ubo.mvpMat = Prj * View * playables[0].ubo.mMat;
				DSStatue0.map(currentImage, &playables[0].ubo, sizeof(playables[0].ubo), 0);

				playables[1].ubo.mvpMat = Prj * View * playables[1].ubo.mMat;
				DSStatue1.map(currentImage, &playables[1].ubo, sizeof(playables[1].ubo), 0);

				playables[2].ubo.mvpMat = Prj * View * playables[2].ubo.mMat;
				DSStatue2.map(currentImage, &playables[2].ubo, sizeof(playables[2].ubo), 0);

				playables[3].ubo.mvpMat = Prj * View * playables[3].ubo.mMat;
				DSStatue3.map(currentImage, &playables[3].ubo, sizeof(playables[3].ubo), 0);

				//Crosshair
				uboCrosshair.visible = 1.0f;
				uboCrosshair.alternative = (float)detect;
				DSCrosshair.map(currentImage, &uboCrosshair, sizeof(uboCrosshair), 0);

				break;
		}	
	}	
};

#include "GameLogic.hpp"
#include "LevelCreation.hpp"

// This is the main: probably you do not need to touch this!
int main() {
    Project app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//Set initial parameters for playable model
PlayerData initData(Model<VertexMesh> &Model, int scene, glm::mat4 world, float scale){
	PlayerData data;
	world *= glm::scale(glm::mat4(1),glm::vec3(scale));
	data.scene = scene;
	data.scale = scale;
	data.angles = glm::vec2(0.0,0.0);

	//Find minimum and maximum vertices for AABB computation
	data.minVector = glm::vec3(std::numeric_limits<float>::max());
    data.maxVector = glm::vec3(-std::numeric_limits<float>::max());
	for(int i=0; i<Model.vertices.size(); i++){   
        	glm::vec3 vertexPosition = Model.vertices[i].pos;			
        	data.minVector = glm::min(data.minVector, vertexPosition);
        	data.maxVector = glm::max(data.maxVector, vertexPosition);        
    }
	//Transform from local model coords to world coords
	data.minVectorWorld=glm::vec3(world * glm::vec4(data.minVector,1.0));
	data.maxVectorWorld=glm::vec3(world * glm::vec4(data.maxVector,1.0));

	//Set initial uniform buffer values
	data.ubo.amb = 1.0f; data.ubo.gamma = 180.0f; data.ubo.sColor = glm::vec3(1.0f);
	data.ubo.mMat = world;
	data.ubo.nMat = glm::inverse(glm::transpose(world));
	data.ubo.visible = 1.0f;

	return data;
}

//Create World Matrix given position and rotation
glm::mat4 getWorld(glm::vec3 pos, glm::vec3 rot){
	return 	glm::translate(glm::mat4(1.0f), pos) *
			glm::rotate(glm::mat4(1.0f), rot.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
        	glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0f, 0.0f, 0.0f)) *           
            glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
}
