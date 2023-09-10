const float fColor = 34.1f/255.0f;

void LevelCreation(Project *A){
    Model<VertexMesh> &MWall = A->MWall;
    Model<VertexMesh> &MBrickWall = A->MBrickWall;
    Model<VertexMesh> &MCorner = A->MCorner;
    Model<VertexMesh> &MBrickCorner = A->MBrickCorner;
    Model<VertexMesh> &MCellBars = A->MCellBars;
    Model<VertexMesh> &MBarrel = A->MBarrel;
    Model<VertexVColor> &MFloor = A->MFloor;
    MeshUniformBlock &uboWall = A->uboWall;  
    MeshUniformBlock &uboCorner = A->uboCorner; 
    MeshUniformBlock &uboBrickWall = A->uboBrickWall; 
    MeshUniformBlock &uboBrickCorner = A->uboBrickCorner; 
    MeshUniformBlock &uboCellBars = A->uboCellBars; 
    MeshUniformBlock &uboFloor = A->uboFloor;
    MeshUniformBlock &uboBarrel = A->uboBarrel;  

    //Scenery
    //Wall
    uboWall.amb = 1.0f; uboWall.gamma = 180.0f; uboWall.sColor = glm::vec3(1.0f); uboWall.visible = 1.0f;
    uboWall.mMat[0] = getWorld(glm::vec3(10,0,4),glm::vec3(0, glm::radians(-90.f), 0));
    uboWall.mMat[1] = getWorld(glm::vec3(10,0,6),glm::vec3(0, glm::radians(-90.f), 0));
    uboWall.mMat[2] = getWorld(glm::vec3(4,0,0),glm::vec3(0));
    uboWall.mMat[3] = getWorld(glm::vec3(6,0,0),glm::vec3(0));
    uboWall.mMat[4] = getWorld(glm::vec3(4,0,10),glm::vec3(0, glm::radians(180.f), 0));
    uboWall.mMat[5] = getWorld(glm::vec3(6,0,10),glm::vec3(0, glm::radians(180.f), 0));
    uboWall.mMat[6] = getWorld(glm::vec3(0,0,2),glm::vec3(0, glm::radians(90.f), 0));
    uboWall.mMat[7] = getWorld(glm::vec3(0,0,8),glm::vec3(0, glm::radians(90.f), 0));
    uboWall.mMat[8] = getWorld(glm::vec3(-5.75,0,4),glm::vec3(0, glm::radians(90.f), 0));
    uboWall.mMat[9] = getWorld(glm::vec3(-5.75,0,6),glm::vec3(0, glm::radians(90.f), 0));
    uboWall.mMat[10] = getWorld(glm::vec3(-2.25,0,13.5),glm::vec3(0, glm::radians(180.f), 0));
        //Upper level
    uboWall.mMat[11] = getWorld(glm::vec3(1.37,2.5,-7.82),glm::vec3(0, glm::radians(90.f), 0));
    uboWall.mMat[12] = getWorld(glm::vec3(1.37,2.5,-11.32),glm::vec3(0, glm::radians(90.f), 0));
    uboWall.mMat[13] = getWorld(glm::vec3(1.37,2.5,-14.82),glm::vec3(0, glm::radians(90.f), 0));
    uboWall.mMat[14] = getWorld(glm::vec3(3.6,2.5,-7.24),glm::vec3(0, glm::radians(180.f), 0));
    uboWall.mMat[15] = getWorld(glm::vec3(7.1,2.5,-7.24),glm::vec3(0, glm::radians(180.f), 0));
    uboWall.mMat[16] = getWorld(glm::vec3(9.3,2.5,-10.32),glm::vec3(0, glm::radians(-90.f), 0));
    uboWall.mMat[17] = getWorld(glm::vec3(9.3,2.5,-13.82),glm::vec3(0, glm::radians(-90.f), 0));
    uboWall.mMat[18] = getWorld(glm::vec3(5.3,2.5,-16.5),glm::vec3(0));
        
    
    A->wallCount = 19;
    for(int i=0; i<A->wallCount; i++){
        uboWall.nMat[i] = glm::inverse(glm::transpose(uboWall.mMat[i]));
    }


    //Corner
    uboCorner.amb = 1.0f; uboCorner.gamma = 180.0f; uboCorner.sColor = glm::vec3(1.0f); uboCorner.visible = 1.0f;
    uboCorner.mMat[0] = getWorld(glm::vec3(0,0,10),glm::vec3(0, glm::radians(90.f), 0));;
    uboCorner.mMat[1] = getWorld(glm::vec3(10,0,10),glm::vec3(0, glm::radians(180.f), 0));
    uboCorner.mMat[2] = getWorld(glm::vec3(0,0,0),glm::vec3(0));
    uboCorner.mMat[3] = getWorld(glm::vec3(10,0,0),glm::vec3(0, glm::radians(-90.f), 0));
    uboCorner.mMat[4] = getWorld(glm::vec3(-5.75,0,0),glm::vec3(0));
    uboCorner.mMat[5] = getWorld(glm::vec3(-5.75,0,10),glm::vec3(0, glm::radians(90.f), 0));
    uboCorner.mMat[6] = getWorld(glm::vec3(0.35,0,0),glm::vec3(0, glm::radians(-90.f), 0));
    uboCorner.mMat[7] = getWorld(glm::vec3(0.35,0,10),glm::vec3(0, glm::radians(180.f), 0));
    uboCorner.mMat[8] = getWorld(glm::vec3(0.35,0,9.5),glm::vec3(0, glm::radians(-90.f), 0));
    uboCorner.mMat[9] = getWorld(glm::vec3(-5.75,0,9.5),glm::vec3(0));
    uboCorner.mMat[10] = getWorld(glm::vec3(0.35,0,13.5),glm::vec3(0, glm::radians(180.f), 0));
    uboCorner.mMat[11] = getWorld(glm::vec3(-5.75,0,13.5),glm::vec3(0, glm::radians(90.f), 0));
        //Upper level
    uboCorner.mMat[12] = getWorld(glm::vec3(1.41,2.5,-16.5),glm::vec3(0));
    uboCorner.mMat[13] = getWorld(glm::vec3(9.3,2.5,-7),glm::vec3(0,glm::radians(180.f),0));
    uboCorner.mMat[14] = getWorld(glm::vec3(9.3,2.5,-16.5),glm::vec3(0,glm::radians(-90.f),0));

    A->cornerCount = 15; 
    for(int i=0; i<A->cornerCount; i++){
        uboCorner.nMat[i] = glm::inverse(glm::transpose(uboCorner.mMat[i]));
    }


    //BrickWall
    uboBrickWall.amb = 1.0f; uboBrickWall.gamma = 180.0f; uboBrickWall.sColor = glm::vec3(1.0f); uboBrickWall.visible = 1.0f;
    uboBrickWall.mMat[0] = getWorld(glm::vec3(-1.25,0,-1.65),glm::vec3(0, glm::radians(-90.f), 0));
    uboBrickWall.mMat[1] = getWorld(glm::vec3(-4.15,0,-1.675),glm::vec3(0, glm::radians(90.f), 0));
    uboBrickWall.mMat[2] = getWorld(glm::vec3(-1.25,0,-5.65),glm::vec3(0, glm::radians(-90.f), 0));
    uboBrickWall.mMat[3] = getWorld(glm::vec3(-4.15,0,-5.65),glm::vec3(0, glm::radians(90.f), 0));
    uboBrickWall.mMat[4] = getWorld(glm::vec3(0.4,0,-7.28),glm::vec3(0, glm::radians(180.f), 0));
    uboBrickWall.mMat[5] = getWorld(glm::vec3(3.9,0,-7.28),glm::vec3(0, glm::radians(180.f), 0));
    uboBrickWall.mMat[6] = getWorld(glm::vec3(-0.2,0,-9.65),glm::vec3(0));
    uboBrickWall.mMat[7] = getWorld(glm::vec3(1.41,0,-11.32),glm::vec3(0, glm::radians(90.f), 0));
    uboBrickWall.mMat[8] = getWorld(glm::vec3(1.41,0,-14.82),glm::vec3(0, glm::radians(90.f), 0));
    uboBrickWall.mMat[9] = getWorld(glm::vec3(7.4,0,-11.25),glm::vec3(0, glm::radians(-90.f), 0));
    uboBrickWall.mMat[10] = getWorld(glm::vec3(7.4,0,-14.75),glm::vec3(0, glm::radians(-90.f), 0));
            //Upper level floor
    uboBrickWall.mMat[11] = getWorld(glm::vec3(7.1,2.15,-8.25),glm::vec3(glm::radians(-90.f),0,glm::radians(-90.f)));
    uboBrickWall.mMat[12] = getWorld(glm::vec3(7.1,2.15,-11.75),glm::vec3(glm::radians(-90.f),0,glm::radians(-90.f)));
    uboBrickWall.mMat[13] = getWorld(glm::vec3(7.1,2.15,-15.25),glm::vec3(glm::radians(-90.f),0,glm::radians(-90.f)));

    A->brickWallCount = 14;
    for(int i=0; i<A->brickWallCount; i++){
        uboBrickWall.nMat[i] = glm::inverse(glm::transpose(uboBrickWall.mMat[i]));
    }


    //BrickCorner
    uboBrickCorner.amb = 1.0f; uboBrickCorner.gamma = 180.0f; uboBrickCorner.sColor = glm::vec3(1.0f); uboBrickCorner.visible = 1.0f;
    uboBrickCorner.mMat[0] = getWorld(glm::vec3(-4.15,0,-9.65),glm::vec3(0));
    uboBrickCorner.mMat[1] = getWorld(glm::vec3(7.4,0,-7.28),glm::vec3(0,glm::radians(180.0f),0));
    uboBrickCorner.mMat[2] = getWorld(glm::vec3(7.4,0,-16.5),glm::vec3(0,glm::radians(-90.0f),0));
    uboBrickCorner.mMat[3] = getWorld(glm::vec3(1.41,0,-16.5),glm::vec3(0));

    A->brickCornerCount = 4;
    for(int i=0; i<A->brickCornerCount; i++){
        uboBrickCorner.nMat[i] = glm::inverse(glm::transpose(uboBrickCorner.mMat[i]));
    }

    
    //Cell bars
    uboCellBars.amb = 1.0f; uboCellBars.gamma = 180.0f; uboCellBars.sColor = glm::vec3(1.0f); uboCellBars.visible = 1.0f;
    uboCellBars.mMat[0] = getWorld(glm::vec3(0.125,0,5),glm::vec3(0, glm::radians(90.f), 0));
    uboCellBars.mMat[1] = getWorld(glm::vec3(-3,0,9.75),glm::vec3(0, 0, 0));

    A->cellBarsCount = 2;
    for(int i=0; i<A->cellBarsCount; i++){
        uboCellBars.nMat[i] = glm::inverse(glm::transpose(uboCellBars.mMat[i]));
    }


    //Barrels
    uboBarrel.amb = 1.0f; uboBarrel.gamma = 180.0f; uboBarrel.sColor = glm::vec3(1.0f); uboBarrel.visible = 1.0f;
    A->button.startWorld = getWorld(glm::vec3(8.11,3.6,-16.2),glm::vec3(glm::radians(90.f), 0, 0)); //button

    A->barrelCount = 1;
    for(int i=1; i<A->cellBarsCount; i++){
        uboBarrel.nMat[i] = glm::inverse(glm::transpose(uboBarrel.mMat[i]));
    }

        //Initialize AABB for button interaction
    glm::vec3 minVector = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxVector = glm::vec3(-std::numeric_limits<float>::max());
    for(int i=0; i<MBarrel.vertices.size(); i++){   
        glm::vec3 vertexPosition = MBarrel.vertices[i].pos;			
        minVector = glm::min(minVector, vertexPosition);
        maxVector = glm::max(maxVector, vertexPosition);        
    }
    A->button.minVector = glm::vec3(A->button.startWorld * glm::vec4(minVector,1.0));
    A->button.maxVector = glm::vec3(A->button.startWorld * glm::vec4(maxVector,1.0));
    
    
    //Floor
    MFloor.vertices.push_back({{10.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{10.0f,0.0f,13.5f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-5.75f,0.0f,13.5f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-5.75f,0.0f,0.0f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});

    MFloor.indices.push_back(0); MFloor.indices.push_back(1); MFloor.indices.push_back(2); 
    MFloor.indices.push_back(2); MFloor.indices.push_back(3); MFloor.indices.push_back(0);

    MFloor.vertices.push_back({{-1.5f,0.0f,0.0f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-4.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-1.5f,0.0f,-5.f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-4.0f,0.0f,-5.f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});

    MFloor.indices.push_back(4); MFloor.indices.push_back(5); MFloor.indices.push_back(6); 
    MFloor.indices.push_back(5); MFloor.indices.push_back(6); MFloor.indices.push_back(7);

    MFloor.vertices.push_back({{-4.0f,0.0f,-7.f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{10.0f,0.0f,-7.f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{10.0f,0.0f,-17.f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-4.0f,0.0f,-17.f},{0.0f,1.0f,0.0f},{fColor,fColor,fColor}});

    MFloor.indices.push_back(8); MFloor.indices.push_back(9); MFloor.indices.push_back(10); 
    MFloor.indices.push_back(10); MFloor.indices.push_back(11); MFloor.indices.push_back(8);


    //Ceiling
    MFloor.vertices.push_back({{10.0f,2.5f,0.0f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{10.0f,2.5f,13.5f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-5.75f,2.5f,13.5f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-5.75f,2.5f,0.0f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});

    MFloor.indices.push_back(12); MFloor.indices.push_back(13); MFloor.indices.push_back(14); 
    MFloor.indices.push_back(14); MFloor.indices.push_back(15); MFloor.indices.push_back(12);

    MFloor.vertices.push_back({{-4.0f,2.5f,0.0f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{-4.0f,2.5f,-9.45f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{1.8f,2.5f,-9.45f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{1.8f,2.5f,0.0f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});

    MFloor.indices.push_back(16); MFloor.indices.push_back(17); MFloor.indices.push_back(18); 
    MFloor.indices.push_back(18); MFloor.indices.push_back(19); MFloor.indices.push_back(16);

    MFloor.vertices.push_back({{1.56f,5.f,-7.4f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{1.56f,5.f,-16.5f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{9.2f,5.f,-16.5f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});
    MFloor.vertices.push_back({{9.2f,5.f,-7.4f},{0.0f,-1.0f,0.0f},{fColor,fColor,fColor}});

    MFloor.indices.push_back(20); MFloor.indices.push_back(21); MFloor.indices.push_back(22); 
    MFloor.indices.push_back(22); MFloor.indices.push_back(23); MFloor.indices.push_back(20);

    
    uboFloor.amb = 1.0f; uboFloor.gamma = 180.0f; uboFloor.sColor = glm::vec3(1); uboFloor.visible = 1.0f;
    uboFloor.mMat[0]=glm::mat4(1); 
    uboFloor.nMat[0]=glm::inverse(glm::transpose(uboFloor.mMat[0]));
}