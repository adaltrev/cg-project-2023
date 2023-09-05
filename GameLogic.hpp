const float fov = glm::radians(45.0f);
const float n = 0.1f;
const float f = 100.0f;
const float ROT_SPEED = glm::radians(90.0f);
const float MOVE_SPEED = 1.0f;
const float minPitch = glm::radians(-60.0f);
const float maxPitch = glm::radians(60.0f);
const float EPSILON = 1e-6f;


void GameLogic(Project *A){
    static float alpha = 0.0f;
    static float beta = 0.0f;
    static float rho = 0.0f;

    //Get input
    float deltaT;
    glm::vec3 m = glm::vec3(0.0f);
    glm::vec3 r = glm::vec3(0.0f);
    bool fire = false;
    A->getSixAxis(deltaT, m, r, fire);

    //Debounce
    static bool wasFire = false;
    bool handleFire = (wasFire && (!fire));
    wasFire = fire;

    int currentScene = A->currentScene;

    if(currentScene>0){
        //Update camera orientation
        alpha += ROT_SPEED * (-r.y) * deltaT;
        beta += ROT_SPEED * (-r.x) * deltaT;
        beta = glm::clamp(beta,minPitch,maxPitch);
        rho += ROT_SPEED * r.z * deltaT;

        //Update camera position
        glm::vec3 ux = glm::vec3(glm::rotate(glm::mat4(1),alpha,glm::vec3(0,1,0)) * glm::vec4(1,0,0,1));
        glm::vec3 uy = glm::vec3(0,1,0);
        glm::vec3 uz = glm::vec3(glm::rotate(glm::mat4(1),alpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1));
        A->camPos += ux * MOVE_SPEED * m.x * deltaT;
        A->camPos += uy * MOVE_SPEED * m.y * deltaT;
        A->camPos += uz * MOVE_SPEED * m.z * deltaT;

        //Compute projection + view		
        A->Prj = glm::perspective(fov, A->Ar, n, f);
        A->Prj[1][1] *= -1;
        A->View =  glm::rotate(glm::mat4(1.0),-beta,glm::vec3(1,0,0)) *
                glm::rotate(glm::mat4(1.0),-alpha,glm::vec3(0,1,0)) *
                glm::translate(glm::mat4(1.0), -(A->camPos));

        //Compute raycast for crosshair detection
        glm::vec3 rayStart = glm::unProject(glm::vec3(A->currW/2, A->currH/2, 0.0f), A->View, A->Prj, A->viewport);
		glm::vec3 rayEnd = glm::unProject(glm::vec3(A->currW/2, A->currH/2, 1.0f), A->View, A->Prj, A->viewport);
		glm::vec3 rayDir = glm::normalize(rayEnd-rayStart);
        glm::vec3 invDir = 1.0f/rayDir;

        //Check crosshair detection for all valid objects in the scene, with AABB
        A->detect = false;
        for(int i=0;i<A->playables.size();i++){
            ModelData model = A->playables[i];
            if(model.scene==currentScene){
                glm::vec3 minVector=glm::vec3(model.world * glm::vec4(model.minVector,1.0));
		        glm::vec3 maxVector=glm::vec3(model.world * glm::vec4(model.maxVector,1.0));

                glm::vec3 tMin = (minVector - rayStart) * invDir;
                glm::vec3 tMax = (maxVector - rayStart) * invDir;
                glm::vec3 t1 = glm::min(tMin, tMax);
                glm::vec3 t2 = glm::max(tMin, tMax);
                float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
                float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
                
                if (tFar>=0 && tNear<=tFar){
                    A->detect = true;
                    break;
                }
            }
        }
    }

    //State machine    
    switch(A->currentScene) {		
        case 0: //Start Menu
            if(handleFire) {
                A->currentScene = 1;
                A->RebuildPipeline();	
            }
        break;        
        case 1: //Level 1
            if(handleFire) {
                A->currentScene = 0;
                A->RebuildPipeline();		
            } 
        break;
    }    
}




