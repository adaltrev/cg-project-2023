#include "Starter.hpp"

// The uniform buffer objects data structures
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)

struct MeshUniformBlock {
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
struct ModelData{
	glm::vec3 maxVector;
	glm::vec3 minVector;
	glm::mat4 world;
	int scene;
};


class Project;
void GameLogic(Project *A);
ModelData initData(Model<VertexMesh> &Model);

// MAIN ! 
class Project : public BaseProject {
	public:
	//Support variables
	int currentScene = 0;
	bool detect = false;
	float Ar, currW, currH;
	glm::vec4 viewport;
	glm::vec3 camPos;
	glm::mat4 View, Prj;
	std::vector<ModelData> playables;



	protected:
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLMesh, DSLOverlay, DSLVColor;

	// Vertex formats
	VertexDescriptor VMesh;
	VertexDescriptor VOverlay;
	VertexDescriptor VVColor;

	// Pipelines [Shader couples]
	Pipeline PMesh;
	Pipeline POverlay;
	Pipeline PVColor;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexVColor> MTest, MDummy;
	Model<VertexOverlay> MMenu, MCrosshair;
	Model<VertexMesh> MBarrel;

	DescriptorSet DSGubo, DSTest, DSMenu, DSCrosshair, DSBarrel;

	Texture TMenu, TCrosshair, TCrosshairAlt, TVarious;
 	
	// C++ storage for uniform variables
	MeshUniformBlock uboTest, uboBarrel;
	GlobalUniformBlock gubo;
	OverlayUniformBlock uboMenu, uboCrosshair;


	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		currW = windowWidth;
		windowHeight = 600;
		currH = windowHeight;
		windowTitle = "PROJECT";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 5;
		texturesInPool = 5;
		setsInPool = 6;
		
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
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", {&DSLOverlay});
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_ALWAYS, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, true);
		PVColor.init(this, &VVColor, "shaders/VColorVert.spv","shaders/VColorFrag.spv", {&DSLGubo, &DSLVColor});
		PVColor.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, true);
		
		// Models, textures and Descriptors (values assigned to the uniforms)
		// Create models
		MTest.vertices.push_back({{0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{255.0f,255.0f,255.0f}});
		MTest.vertices.push_back({{10.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{255.0f,255.0f,255.0f}});
		MTest.vertices.push_back({{0.0f,0.0f,10.0f},{0.0f,1.0f,0.0f},{255.0f,255.0f,255.0f}});
		MTest.vertices.push_back({{10.0f,0.0f,10.0f},{0.0f,1.0f,0.0f},{255.0f,255.0f,255.0f}});
		MTest.indices.push_back(0); MTest.indices.push_back(1); MTest.indices.push_back(2); 
		MTest.indices.push_back(1); MTest.indices.push_back(2); MTest.indices.push_back(3);
		MTest.initMesh(this, &VVColor); 

		MMenu.vertices.push_back({{-1.0f,-1.0f},{0.0f,0.0f}});
		MMenu.vertices.push_back({{-1.0f,1.0f},{0.0f,1.0f}});	
		MMenu.vertices.push_back({{1.0f,-1.0f},{1.0f,0.0f}});	
		MMenu.vertices.push_back({{1.0f,1.0f},{1.0f,1.0f}});	
		MMenu.indices.push_back(0); MMenu.indices.push_back(1); MMenu.indices.push_back(2); 
		MMenu.indices.push_back(1); MMenu.indices.push_back(2); MMenu.indices.push_back(3);
		MMenu.initMesh(this, &VOverlay);

		MCrosshair.vertices.push_back({{-0.1f,-0.1f},{0.0f,0.0f}});
		MCrosshair.vertices.push_back({{-0.1f,0.1f},{0.0f,1.0f}});	
		MCrosshair.vertices.push_back({{0.1f,-0.1f},{1.0f,0.0f}});	
		MCrosshair.vertices.push_back({{0.1f,0.1f},{1.0f,1.0f}});
		MCrosshair.indices.push_back(0); MCrosshair.indices.push_back(1); MCrosshair.indices.push_back(2);
		MCrosshair.indices.push_back(1); MCrosshair.indices.push_back(2); MCrosshair.indices.push_back(3);
		MCrosshair.initMesh(this, &VOverlay);

		MBarrel.init(this, &VMesh, "models/barrel.001.mgcg", MGCG);
		ModelData Barrel = initData(MBarrel);
		Barrel.scene = 1; 
		Barrel.world = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-2.0f)),
													0.0f, glm::vec3(1,0,0));
		playables.push_back(Barrel);

		
		// Create the textures
		TMenu.init(this, "textures/Menu.png");
		TCrosshair.init(this, "textures/Crosshair.png");
		TCrosshairAlt.init(this, "textures/Crosshair1.png");
		TVarious.init(this, "textures/Textures_Food.png");

		
		// Init local variables
		camPos = glm::vec3(0.0f, 0.0f, 0.0f);		
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		std::cout<<"Initializing pipelines and sets...";
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		POverlay.create();
		PVColor.create();
		
		// Here you define the data set
		DSTest.init(this, &DSLVColor, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr}
				});
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
		DSBarrel.init(this, &DSLMesh, {
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

		// Cleanup datasets
		DSTest.cleanup();
		DSMenu.cleanup();
		DSCrosshair.cleanup();
		DSBarrel.cleanup();
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
		MTest.cleanup();
		MMenu.cleanup();
		MCrosshair.cleanup();
		MBarrel.cleanup();
		
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
		DSLOverlay.cleanup();
		DSLVColor.cleanup();
		DSLGubo.cleanup();
		
		// Destroies the pipelines
		PMesh.destroy();		
		POverlay.destroy();
		PVColor.destroy();
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
				DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
				PMesh.bind(commandBuffer);
				MBarrel.bind(commandBuffer);
				DSBarrel.bind(commandBuffer, PMesh, 1, currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MBarrel.indices.size()), 1, 0, 0, 0);

				DSGubo.bind(commandBuffer,PVColor,0,currentImage);
				PVColor.bind(commandBuffer);			
				MTest.bind(commandBuffer);
				DSTest.bind(commandBuffer,PVColor,1,currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MTest.indices.size()),1,0,0,0);

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

		
		

		/*Barrel.world= glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-2.0f)),
									0.0f, glm::vec3(1,0,0));

		//std::cout<<check<<std::endl;*/
    	



		//Update Gubo
		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		
		switch(currentScene){
			case 0: //Start Menu
				uboMenu.visible = 1.0f;
				DSMenu.map(currentImage, &uboMenu, sizeof(uboMenu), 0);
				break;
			case 1: //Level 1
				glm::mat4 World = glm::mat4(1);		
				uboTest.amb = 1.0f; uboTest.gamma = 180.0f; uboTest.sColor = glm::vec3(1.0f);
				uboTest.mvpMat = Prj * View * World;
				uboTest.mMat = World;
				uboTest.nMat = glm::inverse(glm::transpose(World));
				DSTest.map(currentImage, &uboTest, sizeof(uboTest), 0);

				World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-2.0f)),
							0.0f, glm::vec3(1,0,0));
				uboBarrel.amb = 1.0f; uboBarrel.gamma = 180.0f; uboBarrel.sColor = glm::vec3(1.0f);
				uboBarrel.mvpMat = Prj * View * World;
				uboBarrel.mMat = World;
				uboBarrel.nMat = glm::inverse(glm::transpose(World));
				DSBarrel.map(currentImage, &uboBarrel, sizeof(uboBarrel), 0);

				uboCrosshair.visible = 1.0f;
				uboCrosshair.alternative = (float)detect;
				DSCrosshair.map(currentImage, &uboCrosshair, sizeof(uboCrosshair), 0);

				//checkCollision(View, Prj, MBarrel);
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


ModelData initData(Model<VertexMesh> &Model){
	ModelData data;

	data.minVector = glm::vec3(std::numeric_limits<float>::max());
    data.maxVector = glm::vec3(-std::numeric_limits<float>::max());
	for(int i=0; i<Model.vertices.size(); i++){   
        	glm::vec3 vertexPosition = Model.vertices[i].pos;			
        	data.minVector = glm::min(data.minVector, vertexPosition);
        	data.maxVector = glm::max(data.maxVector, vertexPosition);        
    }

	data.world = glm::mat4(1);	

	return data;
}