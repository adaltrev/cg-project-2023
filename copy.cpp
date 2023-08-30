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


// MAIN ! 
class Project : public BaseProject {
	protected:

	// Current aspect ratio (used by the callback that resized the window
	float Ar;

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
	Model<VertexVColor> MTest;

	DescriptorSet DSGubo, DSTest;
 	
	// C++ storage for uniform variables
	MeshUniformBlock uboTest;
	GlobalUniformBlock gubo;

	// Other application parameters
	float alpha, beta, rho;
	int gameState;
	glm::vec3 camPos;


	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "PROJECT";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 2;
		texturesInPool = 0;
		setsInPool = 2;
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
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
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
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
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);
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

		
		
		// Create the textures
		
		// Init local variables
		alpha = 0.0f;
		beta = 0.0f;
		rho = 0.0f;
		gameState = 0;
		camPos = glm::vec3(0.0f, -1.0f, 0.0f);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		POverlay.create();
		PVColor.create();
		
		// Here you define the data set
		DSTest.init(this, &DSLVColor, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr}
				});		
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
				});
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
		DSGubo.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		
		// Cleanup models
		MTest.cleanup();
		
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
		// sets global uniforms

		// binds the pipeline

		// binds the model
		
		// binds the data set
					
		// record the drawing command in the command buffer
		DSGubo.bind(commandBuffer,PVColor,0,currentImage);
		PVColor.bind(commandBuffer);			
		MTest.bind(commandBuffer);
		DSTest.bind(commandBuffer,PVColor,1,currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MTest.indices.size()),1,0,0,0);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f);
		glm::vec3 r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);

		//Start event when the key is released
		static bool wasFire = false;
		bool handleFire = (wasFire && (!fire));
		wasFire = fire;
		 
		switch(gameState) {		
		  case 0: //Splash screen
			if(handleFire) {
				gameState = 1;	
			}
			break;
		  case 1: 
			if(handleFire) {
				gameState = 2;	
			}
			break;
		}
		
		// Parameters
		// Camera FOV-y, Near Plane and Far Plane
		const float fov = glm::radians(45.0f);
		const float n = 0.1f;
		const float f = 100.0f;
		const float ROT_SPEED = glm::radians(90.0f);
		const float MOVE_SPEED = 1.0f;
		const float minPitch = glm::radians(-60.0f);
		const float maxPitch = glm::radians(60.0f);

		alpha += ROT_SPEED * (-r.y) * deltaT;
		beta += ROT_SPEED * (-r.x) * deltaT;
		beta = glm::clamp(beta,minPitch,maxPitch);
		rho += ROT_SPEED * r.z * deltaT;

		glm::vec3 ux = glm::vec3(glm::rotate(glm::mat4(1),alpha,glm::vec3(0,1,0)) * glm::vec4(1,0,0,1));
		glm::vec3 uy = glm::vec3(0,1,0);
		glm::vec3 uz = glm::vec3(glm::rotate(glm::mat4(1),alpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1));
		camPos -= ux * MOVE_SPEED * m.x * deltaT;
		camPos -= uz * MOVE_SPEED * m.z * deltaT;
				
		glm::mat4 Prj = glm::perspective(fov, Ar, n, f);
		Prj[1][1] *= -1;
		
		glm::mat4 View = glm::rotate(glm::mat4(1.0), -beta, glm::vec3(1,0,0)) *
						 glm::rotate(glm::mat4(1.0), -alpha, glm::vec3(0,1,0)) *
						 glm::translate(glm::mat4(1.0), camPos);


		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block

		glm::mat4 World = glm::mat4(1);		
		uboTest.amb = 1.0f; uboTest.gamma = 180.0f; uboTest.sColor = glm::vec3(1.0f);
		uboTest.mvpMat = Prj * View * World;
		uboTest.mMat = World;
		uboTest.nMat = glm::inverse(glm::transpose(World)); //DONE
		DSTest.map(currentImage, &uboTest, sizeof(uboTest), 0); //DONE
	}	
};


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