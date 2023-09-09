void LevelCreation(Project *A){
    Model<VertexMesh> &MWall = A->MWall;
    Model<VertexMesh> &MBrickWall = A->MBrickWall;
    Model<VertexMesh> &MCorner = A->MCorner;
    Model<VertexMesh> &MBrickCorner = A->MBrickCorner;
    Model<VertexMesh> &MCellBars = A->MCellBars;
    Model<VertexVColor> &MFloor = A->MFloor;
    MeshUniformBlock &uboWall = A->uboWall;  
    MeshUniformBlock &uboCorner = A->uboCorner; 
    MeshUniformBlock &uboBrickWall = A->uboBrickWall; 
    MeshUniformBlock &uboBrickCorner = A->uboBrickCorner; 
    MeshUniformBlock &uboCellBars = A->uboCellBars; 
    MeshUniformBlock &uboFloor = A->uboFloor; 

    //Floor + Ceiling
    float color = 34.1f/255.0f;
    MFloor.vertices.push_back({{0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{10.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{0.0f,0.0f,10.0f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{10.0f,0.0f,10.0f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{-5.75f,0.0f,0.0f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{-5.75f,0.0f,10.0f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{0.35f,0.0f,10.0f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{0.35f,0.0f,13.5f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{-5.75f,0.0f,13.5f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{-1.25f,0.0f,-7.65f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{-4.15f,0.0f,-7.65f},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{-1.25f,0.0f,-1.65},{0.0f,1.0f,0.0f},{color,color,color}});
    MFloor.vertices.push_back({{-4.15f,0.0f,-1.675f},{0.0f,1.0f,0.0f},{color,color,color}});

    MFloor.indices.push_back(0); MFloor.indices.push_back(1); MFloor.indices.push_back(2); 
    MFloor.indices.push_back(1); MFloor.indices.push_back(2); MFloor.indices.push_back(3);
    MFloor.indices.push_back(0); MFloor.indices.push_back(2); MFloor.indices.push_back(4);
    MFloor.indices.push_back(2); MFloor.indices.push_back(4); MFloor.indices.push_back(5);
    MFloor.indices.push_back(5); MFloor.indices.push_back(6); MFloor.indices.push_back(7);
    MFloor.indices.push_back(5); MFloor.indices.push_back(7); MFloor.indices.push_back(8);
    MFloor.indices.push_back(9); MFloor.indices.push_back(10); MFloor.indices.push_back(11);
    MFloor.indices.push_back(10); MFloor.indices.push_back(11); MFloor.indices.push_back(12);

    
    uboFloor.amb = 1.0f; uboFloor.gamma = 180.0f; uboFloor.sColor = glm::vec3(1); uboFloor.visible = 1.0f;
    uboFloor.mMat[0]=glm::mat4(1); 
    uboFloor.mMat[1]=getWorld(glm::vec3(0,2.5,0),glm::vec3(0));
    uboFloor.nMat[0]=glm::inverse(glm::transpose(uboFloor.mMat[0]));
    uboFloor.nMat[1]=glm::inverse(glm::transpose(uboFloor.mMat[1]));

    //Scenery
    //Walls
    
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
    uboCorner.mMat[12] = getWorld(glm::vec3(-4.15,0,-9.65),glm::vec3(0));
    for(int i=0; i<=12; i++){
        uboCorner.nMat[i] = glm::inverse(glm::transpose(uboCorner.mMat[i]));
    }


   
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
    uboWall.mMat[11] = getWorld(glm::vec3(-0.2,0,-9.65),glm::vec3(0));
    uboWall.mMat[12] = getWorld(glm::vec3(0.4,0,-7.25),glm::vec3(0, glm::radians(180.f), 0));
    uboWall.mMat[13] = getWorld(glm::vec3(1.41,0,-11.32),glm::vec3(0, glm::radians(90.f), 0));
    uboWall.mMat[14] = getWorld(glm::vec3(1.9,0,-5.25),glm::vec3(0, glm::radians(90.f), 0));
    for(int i=0; i<=14; i++){
        uboWall.nMat[i] = glm::inverse(glm::transpose(uboWall.mMat[i]));
    }


    
    uboBrickWall.amb = 1.0f; uboBrickWall.gamma = 180.0f; uboBrickWall.sColor = glm::vec3(1.0f); uboBrickWall.visible = 1.0f;
    uboBrickWall.mMat[0] = getWorld(glm::vec3(-1.25,0,-1.65),glm::vec3(0, glm::radians(-90.f), 0));
    uboBrickWall.mMat[1] = getWorld(glm::vec3(-4.15,0,-1.675),glm::vec3(0, glm::radians(90.f), 0));
    uboBrickWall.mMat[2] = getWorld(glm::vec3(-1.25,0,-5.65),glm::vec3(0, glm::radians(-90.f), 0));
    uboBrickWall.mMat[3] = getWorld(glm::vec3(-4.15,0,-5.65),glm::vec3(0, glm::radians(90.f), 0));
    for(int i=0; i<=14; i++){
        uboBrickWall.nMat[i] = glm::inverse(glm::transpose(uboBrickWall.mMat[i]));
    }

    
    //Cell bars
    uboCellBars.amb = 1.0f; uboCellBars.gamma = 180.0f; uboCellBars.sColor = glm::vec3(1.0f); uboCellBars.visible = 1.0f;
    uboCellBars.mMat[0] = getWorld(glm::vec3(0.125,0,5),glm::vec3(0, glm::radians(90.f), 0));
    uboCellBars.mMat[1] = getWorld(glm::vec3(-3,0,9.75),glm::vec3(0, 0, 0));
    for(int i=0; i<=14; i++){
        uboCellBars.nMat[i] = glm::inverse(glm::transpose(uboCellBars.mMat[i]));
    }

    return;
}