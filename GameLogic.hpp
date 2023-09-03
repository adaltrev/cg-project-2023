const float fov = glm::radians(45.0f);
const float n = 0.1f;
const float f = 100.0f;
const float ROT_SPEED = glm::radians(90.0f);
const float MOVE_SPEED = 1.0f;
const float minPitch = glm::radians(-60.0f);
const float maxPitch = glm::radians(60.0f);
const float EPSILON = 1e-6f;


void GameLogic(Project *A, float Ar, glm::mat4 &View, glm::mat4 &Prj, glm::vec3 &camPos, int &currentScene){
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

    //State machine    
    switch(currentScene) {		
        case 0: //Start Menu
            if(handleFire) {
                currentScene = 1;
                A->RebuildPipeline();	
            }
        break;
        
        case 1: //Level 1
            if(handleFire) {
                currentScene = 0;
                A->RebuildPipeline();		
            } 
            else{
            //Update camera orientation
            alpha += ROT_SPEED * (-r.y) * deltaT;
            beta += ROT_SPEED * (-r.x) * deltaT;
            beta = glm::clamp(beta,minPitch,maxPitch);
            rho += ROT_SPEED * r.z * deltaT;

            //Update camera position
            glm::vec3 ux = glm::vec3(glm::rotate(glm::mat4(1),alpha,glm::vec3(0,1,0)) * glm::vec4(1,0,0,1));
            glm::vec3 uy = glm::vec3(0,1,0);
            glm::vec3 uz = glm::vec3(glm::rotate(glm::mat4(1),alpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1));
            camPos += ux * MOVE_SPEED * m.x * deltaT;
            camPos += uy * MOVE_SPEED * m.y * deltaT;
            camPos += uz * MOVE_SPEED * m.z * deltaT;

            //Compute projection + view		
            Prj = glm::perspective(fov, Ar, n, f);
            Prj[1][1] *= -1;
            View =  glm::rotate(glm::mat4(1.0),-beta,glm::vec3(1,0,0)) *
                    glm::rotate(glm::mat4(1.0),-alpha,glm::vec3(0,1,0)) *
                    glm::translate(glm::mat4(1.0), -camPos);
            }
        break;
    }    
}

void checkCollision(glm::mat4 &View, glm::mat4 &Prj, Model<VertexMesh> &Obj){
}



