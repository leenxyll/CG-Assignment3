#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include "Libs/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600; 

float pitch = 0.0f, yaw = -90.0f;

Window mainWindow; 
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList; 

//Vertex Shader
static const char* vShader = "Shaders/shader.vert";
//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);

Mesh* light;
static const char* lightVShader = "Shaders/lightShader.vert";
static const char* lightFShader = "Shaders/lightShader.frag";

static const char* groundVShader = "Shaders/groundShader.vert";
static const char* groundFShader = "Shaders/groundShader.frag";

unsigned int loadTexture(const char *textureFile){
    int width, height, nrChannels;
    unsigned char *data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

void createOBJ(){
    Mesh *obj1 = new Mesh();
    Mesh *obj2 = new Mesh();
    Mesh *obj3 = new Mesh();
    Mesh *obj4 = new Mesh();
    Mesh *obj5 = new Mesh();
    Mesh *obj6 = new Mesh();
    Mesh *obj7 = new Mesh(); 
    bool loaded = obj1->CreateMeshFromOBJ("Models/Body.obj");
    bool loaded2 = obj2->CreateMeshFromOBJ("Models/Leg.obj");
    bool loaded3 = obj3->CreateMeshFromOBJ("Models/Plan.obj");
    bool loaded4 = obj4->CreateMeshFromOBJ("Models/Reactor.obj");
    bool loaded5 = obj5->CreateMeshFromOBJ("Models/Window.obj");
    bool loaded6 = obj6->CreateMeshFromOBJ("Models/Smoke.obj");
    bool loaded7 = obj7->CreateMeshFromOBJ("Models/Floor.obj");

    if(loaded && loaded2 && loaded3 && loaded4 && loaded5 && loaded6 && loaded7){
        meshList.push_back(obj1); //Body    0
        meshList.push_back(obj2); //Leg     1
        meshList.push_back(obj3); //Plan    2
        meshList.push_back(obj4); //Reactor 3
        meshList.push_back(obj5); //Window  4
        meshList.push_back(obj6); //Smoke   5
        meshList.push_back(obj7); //Floor   6
    } else {
        std::cout<< "Failed to load model" << std::endl;
    }

    light = new Mesh();
    loaded = light->CreateMeshFromOBJ("Models/cube.obj");

    if(!loaded){
        std::cout<< "Failed to load model" << std::endl;
    }
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader); 
    shaderList.push_back(shader1);

    Shader* shader3 = new Shader();
    shader3->CreateFromFiles(groundVShader, groundFShader); 
    shaderList.push_back(shader3);

    Shader* shaderLight = new Shader();
    shaderLight->CreateFromFiles(lightVShader, lightFShader); 
    shaderList.push_back(shaderLight);
}

void checkMouse(){
    float xoffset, yoffset;
    double xpos, ypos;
    glfwGetCursorPos(mainWindow.getWindow(), &xpos, &ypos);

    static float lastX = xpos;
    static float lastY = ypos;

    xoffset = xpos - lastX;
    yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89) pitch = 89;
    if(pitch < -89) pitch = -89;
}

int main() //ตัวแรกที่ถูกเรียก
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3); 
    mainWindow.initialise();

    createOBJ();
    CreateShaders();

    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth()/
    (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

    glm::vec3 cameraPos = glm::vec3(2.0f, 5.0f, 10.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f); 
    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
        
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp =  glm::normalize(glm::cross(cameraRight, cameraDirection)); 
    
    float deltaTime, lastFrame;

    unsigned int textureBody = loadTexture("Texture/Grey.jpg");         // 0
    unsigned int textureLeg = loadTexture("Texture/red.jpg");           // 1
    unsigned int texturePlan = loadTexture("Texture/Blue.jpg");         // 2
    unsigned int textureReactor = loadTexture("Texture/Darkgrey.jpg");  // 3
    unsigned int textureWindow = loadTexture("Texture/Orange.jpg");     // 4
    unsigned int textureSmoke = loadTexture("Texture/Yellow.jpg");      // 5
    unsigned int textureFloor = loadTexture("Texture/1_Base_color.jpg"); // 6

    //Loop until window closed
    while (!mainWindow.getShouldClose()) 
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        //Get + Handle user input events 
        glfwPollEvents(); 

        if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraDirection * deltaTime * 8.0f;

        if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraDirection * deltaTime * 8.0f;
        if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= cameraRight * deltaTime * 8.0f;
        if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += cameraRight * deltaTime * 8.0f;

        checkMouse();

        glm::vec3 direction;
        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

        cameraDirection = glm::normalize(direction);

        glm::vec3 lightPos = glm::vec3(10, 10, -5); 
        //เคลื่อนตำแหน่งของไฟ
        // lightPos.x = 1.0f + sin(glfwGetTime()) * 10.0f;
        // lightPos.y = sin(glfwGetTime() / 2.0f) * 10.0f;

        //Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //สีดำ
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw here
        shaderList[0]->UseShader(); 
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");
        uniformView = shaderList[0]->GetUniformLocation("view");

        glm::mat4 view (1.0f);

        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        //Object
        glm::mat4 model (1.0f);
        model = glm::scale(model, glm::vec3(1.8f, 1.8f, 1.8f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); 
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection)); 
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view)); 

        glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, (GLfloat*)&lightColour);
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, (GLfloat*)&lightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureBody);
        meshList[0]->RenderMesh(); //Body
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureLeg);
        meshList[1]->RenderMesh(); //Leg
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturePlan);
        meshList[2]->RenderMesh(); //Plan
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureReactor);
        meshList[3]->RenderMesh(); //Reactor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureWindow);
        meshList[4]->RenderMesh(); //Window
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureSmoke);
        meshList[5]->RenderMesh(); //Smoke

        shaderList[1]->UseShader();
        uniformModel = shaderList[1]->GetUniformLocation("model");
        uniformProjection = shaderList[1]->GetUniformLocation("projection");
        uniformView = shaderList[1]->GetUniformLocation("view");

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); 
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection)); 
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view)); 
        glUniform3fv(shaderList[1]->GetUniformLocation("lightColour"), 1, (GLfloat*)&lightColour);
        glUniform3fv(shaderList[1]->GetUniformLocation("lightPos"), 1, (GLfloat*)&lightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureFloor);
        meshList[6]->RenderMesh(); //Floor

        //light
        shaderList[2]->UseShader();
        uniformModel = shaderList[2]->GetUniformLocation("model");
        uniformProjection = shaderList[2]->GetUniformLocation("projection");
        uniformView = shaderList[2]->GetUniformLocation("view");

        model = glm::translate(model, lightPos);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); 
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection)); 
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view)); 
        glUniform3fv(shaderList[2]->GetUniformLocation("lightColour"), 1, (GLfloat*)&lightColour);

        light->RenderMesh();


        glUseProgram(0);
        //end draw

        mainWindow.swapBuffers();
    }

    return 0;
}

