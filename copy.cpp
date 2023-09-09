#include "Starter.hpp"

// The uniform buffer objects data structures
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)

const int numInstances = 30;

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
	glm::vec3 angles;
	PlayerUniformBlock ubo;
};


class Project;
void GameLogic(Project *A);
PlayerData initData(Model<VertexMesh> &Model, int scene, glm::mat4 world);
glm::mat4 getWorld(glm::vec3 pos, glm::vec3 rot);

// MAIN ! 
class Project : public BaseProject {
	public:
	//Support variables
	int currentScene = 0;
	int currentPlayer = 1;
	bool detect = false;
	float Ar, currW, currH;
	glm::vec4 viewport;
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f);	
	glm::mat4 View, Prj;
	std::vector<PlayerData> playables;



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
	Model<VertexMesh> MBarrel, MBarrel1, MCorner, MWall, MCellBars;

	DescriptorSet DSGubo, DSMenu, DSCrosshair, DSBarrel, DSBarrel1, DSCorner, DSWall, DSCellBars;

	Texture TMenu, TCrosshair, TCrosshairAlt, TVarious;
 	
	// C++ storage for uniform variables
	MeshUniformBlock uboCorner, uboWall, uboCellBars;
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
		uniformBlocksInPool = 20;
		texturesInPool = 10;
		setsInPool = 20;
		
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
		
		
		// Models, textures and Descriptors (values assigned to the uniforms)
		// Create models
		MMenu.vertices.push_back({{-1.0f,-1.0f},{0.0f,0.0f}});
		MMenu.vertices.push_back({{-1.0f,1.0f},{0.0f,1.0f}});	
		MMenu.vertices.push_back({{1.0f,-1.0f},{1.0f,0.0f}});	
		MMenu.vertices.push_back({{1.0f,1.0f},{1.0f,1.0f}});	
		MMenu.indices.push_back(0); MMenu.indices.push_back(1); MMenu.indices.push_back(2); 
		MMenu.indices.push_back(1); MMenu.indices.push_back(2); MMenu.indices.push_back(3);
		MMenu.initMesh(this, &VOverlay);

		MCrosshair.vertices.push_back({{-0.075f,-0.1f},{0.0f,0.0f}});
		MCrosshair.vertices.push_back({{-0.075f,0.1f},{0.0f,1.0f}});	
		MCrosshair.vertices.push_back({{0.075f,-0.1f},{1.0f,0.0f}});	
		MCrosshair.vertices.push_back({{0.075f,0.1f},{1.0f,1.0f}});
		MCrosshair.indices.push_back(0); MCrosshair.indices.push_back(1); MCrosshair.indices.push_back(2);
		MCrosshair.indices.push_back(1); MCrosshair.indices.push_back(2); MCrosshair.indices.push_back(3);
		MCrosshair.initMesh(this, &VOverlay);

		//Create Playable models
		//Player 0
		MBarrel.init(this, &VMesh, "models/barrel.001.mgcg", MGCG);
		playables.push_back(initData(MBarrel,1,getWorld(glm::vec3(5.0f,1.0f,5.0f), glm::vec3(0))));

		//Player 1
		MBarrel1.init(this, &VMesh, "models/barrel.001.mgcg", MGCG);
		playables.push_back(initData(MBarrel1,1,glm::mat4(1)));
		playables[1].ubo.visible = 0.0f;
		
		MCorner.init(this, &VMesh, "models/tunnel/tunnel.029_Mesh.6128.mgcg", MGCG);
		glm::mat4 C1 = getWorld(glm::vec3(0,0,10),glm::vec3(0, glm::radians(90.f), 0));
		glm::mat4 C2 = getWorld(glm::vec3(10,0,10),glm::vec3(0, glm::radians(180.f), 0));
		glm::mat4 C3 = getWorld(glm::vec3(0,0,0),glm::vec3(0));
		glm::mat4 C4 = getWorld(glm::vec3(10,0,0),glm::vec3(0, glm::radians(-90.f), 0));

		uboCorner.amb = 1.0f; uboCorner.gamma = 180.0f; uboCorner.sColor = glm::vec3(1.0f);
		uboCorner.visible = 1.0f;
		uboCorner.mMat[0] = C1;
		uboCorner.mMat[1] = C2;
		uboCorner.mMat[2] = C3;
		uboCorner.mMat[3] = C4;
		uboCorner.nMat[0] = glm::inverse(glm::transpose(C1));
		uboCorner.nMat[1] = glm::inverse(glm::transpose(C2));
		uboCorner.nMat[2] = glm::inverse(glm::transpose(C3));
		uboCorner.nMat[3] = glm::inverse(glm::transpose(C4));

		MWall.init(this, &VMesh, "models/tunnel/tunnel.005_Mesh.7961.mgcg", MGCG);
		glm::mat4 W1 = getWorld(glm::vec3(10,0,4),glm::vec3(0, glm::radians(-90.f), 0));
		glm::mat4 W2 = getWorld(glm::vec3(10,0,6),glm::vec3(0, glm::radians(-90.f), 0));
		glm::mat4 W3 = getWorld(glm::vec3(4,0,0),glm::vec3(0));
		glm::mat4 W4 = getWorld(glm::vec3(6,0,0),glm::vec3(0));
		glm::mat4 W5 = getWorld(glm::vec3(4,0,10),glm::vec3(0, glm::radians(180.f), 0));
		glm::mat4 W6 = getWorld(glm::vec3(6,0,10),glm::vec3(0, glm::radians(180.f), 0));
		glm::mat4 W7 = getWorld(glm::vec3(0,0,2),glm::vec3(0, glm::radians(90.f), 0));
		glm::mat4 W8 = getWorld(glm::vec3(0,0,8),glm::vec3(0, glm::radians(90.f), 0));

		uboWall.amb = 1.0f; uboWall.gamma = 180.0f; uboWall.sColor = glm::vec3(1.0f);
		uboWall.visible = 1.0f;
		uboWall.mMat[0] = W1;
		uboWall.mMat[1] = W2;
		uboWall.mMat[2] = W3;
		uboWall.mMat[3] = W4;
		uboWall.mMat[4] = W5;
		uboWall.mMat[5] = W6;
		uboWall.mMat[6] = W7;
		uboWall.mMat[7] = W8;
		uboWall.nMat[0] = glm::inverse(glm::transpose(W1));
		uboWall.nMat[1] = glm::inverse(glm::transpose(W2));
		uboWall.nMat[2] = glm::inverse(glm::transpose(W3));
		uboWall.nMat[3] = glm::inverse(glm::transpose(W4));
		uboWall.nMat[4] = glm::inverse(glm::transpose(W5));
		uboWall.nMat[5] = glm::inverse(glm::transpose(W6));
		uboWall.nMat[6] = glm::inverse(glm::transpose(W7));
		uboWall.nMat[7] = glm::inverse(glm::transpose(W8));

		MCellBars.init(this, &VMesh, "models/tunnel/tunnel.033_Mesh.6508.mgcg", MGCG);
		glm::mat4 CB1 = getWorld(glm::vec3(0.25,0,5),glm::vec3(0, glm::radians(90.f), 0));
		//glm::mat4 W2 = getWorld(glm::vec3(0,0,6),glm::vec3(0, glm::radians(-90.f), 0));
		uboCellBars.amb = 1.0f; uboCellBars.gamma = 180.0f; uboCellBars.sColor = glm::vec3(1.0f);
		uboCellBars.visible = 1.0f;
		uboCellBars.mMat[0] = CB1;
		uboCellBars.nMat[0] = glm::inverse(glm::transpose(CB1));



		// Create the textures
		TMenu.init(this, "textures/Menu.png");
		TCrosshair.init(this, "textures/Crosshair.png");
		TCrosshairAlt.init(this, "textures/Crosshair1.png");
		TVarious.init(this, "textures/Textures_Food.png");
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		std::cout<<"Initializing pipelines and sets...";
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		POverlay.create();
		PVColor.create();
		PPlayer.create();
		
		// Here you define the data set
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
		DSBarrel.init(this, &DSLPlayer, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TVarious}
				});
		DSBarrel1.init(this, &DSLPlayer, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TVarious}
		});
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
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
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
		DSBarrel.cleanup();
		DSBarrel1.cleanup();
		DSCorner.cleanup();
		DSWall.cleanup();
		DSCellBars.cleanup();
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

		// Cleanup models
		MMenu.cleanup();
		MCrosshair.cleanup();
		MBarrel.cleanup();
		MBarrel1.cleanup();
		MCorner.cleanup();
		MWall.cleanup();
		MCellBars.cleanup();
		
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
				DSGubo.bind(commandBuffer, PPlayer, 0, currentImage);
				PPlayer.bind(commandBuffer);
				MBarrel.bind(commandBuffer);
				DSBarrel.bind(commandBuffer, PPlayer, 1, currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MBarrel.indices.size()), 1, 0, 0, 0);
				MBarrel1.bind(commandBuffer);
				DSBarrel1.bind(commandBuffer, PPlayer, 1, currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MBarrel1.indices.size()), 1, 0, 0, 0);

				DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
				PMesh.bind(commandBuffer);
				MCorner.bind(commandBuffer);
				DSCorner.bind(commandBuffer, PMesh, 1, currentImage);	
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MCorner.indices.size()), 4, 0, 0, 0);

				DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
				PMesh.bind(commandBuffer);
				MWall.bind(commandBuffer);
				DSWall.bind(commandBuffer, PMesh, 1, currentImage);	
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MWall.indices.size()), 8, 0, 0, 0);

				DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
				PMesh.bind(commandBuffer);
				MCellBars.bind(commandBuffer);
				DSCellBars.bind(commandBuffer, PMesh, 1, currentImage);	
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MCellBars.indices.size()), 2, 0, 0, 0);

				DSGubo.bind(commandBuffer,PVColor,0,currentImage);
				PVColor.bind(commandBuffer);			

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
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		//Update uniforms for all object, based on the current scene
		switch(currentScene){
			case 0: //Start Menu
				uboMenu.visible = 1.0f;
				DSMenu.map(currentImage, &uboMenu, sizeof(uboMenu), 0);
				break;
			case 1: //Level 1	
				for(int i = 0; i<sizeof(uboCorner.mMat)/sizeof(uboCorner.mMat[0]); i++){
					uboCorner.mvpMat[i] = Prj * View * uboCorner.mMat[i];
				}
				DSCorner.map(currentImage, &uboCorner, sizeof(uboCorner), 0);

				for(int i = 0; i<sizeof(uboWall.mMat)/sizeof(uboWall.mMat[0]); i++){
					uboWall.mvpMat[i] = Prj * View * uboWall.mMat[i];
				}
				DSWall.map(currentImage, &uboWall, sizeof(uboWall), 0);

				uboCellBars.mvpMat[0] = Prj * View *  uboCellBars.mMat[0];
				DSCellBars.map(currentImage, &uboCellBars, sizeof(uboCellBars), 0);

				playables[0].ubo.mvpMat = Prj * View * playables[0].ubo.mMat;
				DSBarrel.map(currentImage, &playables[0].ubo, sizeof(playables[0].ubo), 0);

				playables[1].ubo.mvpMat = Prj * View * playables[1].ubo.mMat;
				DSBarrel1.map(currentImage, &playables[1].ubo, sizeof(playables[1].ubo), 0);

				uboCrosshair.visible = 1.0f;
				uboCrosshair.alternative = (float)detect;
				DSCrosshair.map(currentImage, &uboCrosshair, sizeof(uboCrosshair), 0);

				break;
		}	
	}	
};

#include "GameLogic.hpp"

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
PlayerData initData(Model<VertexMesh> &Model, int scene, glm::mat4 world){
	PlayerData data;

	data.scene = scene;
	data.angles = glm::vec3(0.0,0.0,0.0);

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
        	glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), rot.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
}
