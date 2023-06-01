#include "lab_m1/lab3/lab3.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;

#define BODY_SIZE 100
#define WING_ROTATION 0.25
#define GRASS_SIZE 200
#define MAX_SCORE 40
#define SPEED_INCREASE 10
#define HIT_BOX 70
#define SCORE_X 1000
#define SCORE_Y 650
#define BULLETLIFE_SIZE 70
#define START_SPEED 200
#define DUCK_SCALE 0.5
#define MIDDLE 25
/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab3::Lab3()
{
}


Lab3::~Lab3()
{
}


void Lab3::Init()
{
    window->HidePointer();
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 50;

    windowResolutionX = window->GetResolution().x;
    windowResolutionY = window->GetResolution().y;

    translateX = 0;
    translateY = 0;

    scaleX = 1;
    scaleY = 1;

    angularStep = 0;
    angularStep1 = 0;

    state = 0;

    collisionX = 1;
    collisionY = 1;
    
    clock = 0;

    killed = 0;

    lifes = 3;

    bullets = 3;

    restart = 0;
    
    score = 0;
    
    fly = 1;

    speed = 200;

    speedX = 200;
    speedY = 200;

    Mesh* triangle = object2D::CreateTriangle("triangle", corner, squareSide, glm::vec3(51.0f / 255, 31.0f/255, 31.0f/255));
    AddMeshToList(triangle);

    Mesh* beak = object2D::CreateTriangle("beak", corner, squareSide, glm::vec3(215.0f / 255, 136.0f / 255, 32.0f / 255));
    AddMeshToList(beak);

    Mesh* head = object2D::CreateCircle(100, "head", corner, 1, glm::vec3(16.0f/255, 88.0f/255, 33.0f/255), true);
    AddMeshToList(head);

    Mesh* life = object2D::CreateCircle(100, "life", corner, 1, glm::vec3(1, 0, 0), true);
    AddMeshToList(life);

    Mesh* target = object2D::CreateCircle(100, "target", corner, 1, glm::vec3(1,0,0), true);
    AddMeshToList(target);

    Mesh* grass = object2D::CreateSquare("grass", corner, squareSide, glm::vec3(16.0f / 255, 88.0f / 255, 33.0f / 255), true);
    AddMeshToList(grass);

    Mesh* bullet = object2D::CreateSquare("bullet", corner, squareSide, glm::vec3(16.0f / 255, 88.0f / 255, 33.0f / 255), true);
    AddMeshToList(bullet);

    Mesh* scoreFill = object2D::CreateSquare("scoreFill", corner, squareSide, glm::vec3(0, 0, 1), true);
    AddMeshToList(scoreFill);

    Mesh* scoreBar = object2D::CreateSquare("scoreBar", corner, squareSide, glm::vec3(16.0f / 255, 88.0f / 255, 33.0f / 255), false);
    AddMeshToList(scoreBar);

    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 18);
}


void Lab3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(11.0f/255, 171.0f/255, 250.0f/255, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab3::Update(float deltaTimeSeconds)
{   
    if (bullets == 0) {
        clock = 8;
    }

    if (score == MAX_SCORE){
        restart = 1;
    }

    clock += deltaTimeSeconds;
    
    fly = 1;

    if (restart == 0) {

        if (lifes >= 0) {

            BulletsAndLifesRender();
            ScoreRender();
            TargetRender();
            GrassRender();


            modelMatrix = glm::mat3(1);

            if (killed == 0) {

                if (clock <= 7) {

                    if (translateY <= GRASS_SIZE) {
                        collisionY = 1;
                    }

                    if (translateY >= windowResolutionY) {
                        collisionY = -1;
                    }

                    if (translateX <= 0 - BODY_SIZE) {
                        collisionX = 1;
                    }

                    if (translateX >= windowResolutionX - BODY_SIZE) {
                        collisionX = -1;
                    }
                } else {
                    FlyAway();
                }
            } else {
                Killed();
            }

            FlyDirection(deltaTimeSeconds);

            modelMatrix *= transform2D::Translate(translateX + 120, translateY);

            FlyOrientation(deltaTimeSeconds);
            DuckRender(deltaTimeSeconds);
        } else {
            restart = 1;
        }
    } else {
            RestartGame();
        }

}

void Lab3::WingMove(float deltaTimeSeconds) {

    if (angularStep >= WING_ROTATION) {
        state = 0;
    } else if (angularStep <= -WING_ROTATION) {
        state = 1;
    }

    if (state == 1) {
        angularStep += deltaTimeSeconds * 3;
    } else {
        angularStep += -deltaTimeSeconds * 3;
    }

    modelMatrix *= transform2D::Translate(MIDDLE, 0);
    modelMatrix *= transform2D::Rotate(angularStep + M_PI);
    modelMatrix *= transform2D::Translate(-MIDDLE, 0);
}

void Lab3::FlyDirection(float deltaTimeSeconds) {

    if (fly == 0) {
        translateY += collisionY * deltaTimeSeconds * speed;
    } else if (killed == 1) {
        translateY += collisionY * deltaTimeSeconds * speed;
    } else {
        translateX += collisionX * deltaTimeSeconds * speedX;
        translateY += collisionY * deltaTimeSeconds * speedY;
    }
}

void Lab3::FlyOrientation(float deltaTimeSeconds) {

    if (collisionX == 1 && collisionY == 1 && fly != 0 && killed == 0) {
        modelMatrix *= transform2D::Rotate(0.75);

    }

    if (collisionX == -1 && collisionY == 1 && fly != 0 && killed == 0) {
        modelMatrix *= transform2D::Rotate(2.35);
    }

    if (collisionX == -1 && collisionY == -1 && fly != 0 && killed == 0) {
        modelMatrix *= transform2D::Rotate(3.92);
    }

    if (collisionX == 1 && collisionY == -1 && fly != 0 && killed == 0) {
        modelMatrix *= transform2D::Rotate(5.49);
    }

    if (fly == 0) {
        modelMatrix *= transform2D::Rotate(1.57);
    }

    if (killed == 1) {
        modelMatrix *= transform2D::Rotate(4.71);
    }
}

void Lab3::RestartGame() {

    textRenderer->RenderText("GAME OVER", 500.0f, 300.0f, 2.5f, glm::vec3(0, 0, 0));
    textRenderer->RenderText("PRESS ANY KEY TO RESTART", 300.0f, 400.0f, 2.5f, glm::vec3(0, 0, 0));
    lifes = 3;
    bullets = 3;
    translateX = 0;
    translateY = 0;
    fly = 1;
    clock = 0;
    speed = START_SPEED;
    score = 0;
}

void Lab3::FlyAway() {

    collisionY = 1;
    collisionX = -1;
    fly = 0;

    if (translateY >= windowResolutionY + BODY_SIZE) {
        float rX = 1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 - 1)));
        float rY = 1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 - 1)));
        translateY = 0;
        translateX = rand() % windowResolutionX;
        clock = 0;
        lifes -= 1;
        speed += SPEED_INCREASE;
        speedX = speed * rX;
        speedY = speed * rY;
        bullets = 3;
    }
}
void Lab3::Killed() {

    collisionY = -1;
    collisionX = 1;

    if (translateY <= 0) {
        float rX = 1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 - 1)));
        float rY = 1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 - 1)));
        translateY = 0;
        translateX = rand() % windowResolutionX;
        clock = 0;
        killed = 0;
        score += 1;
        speed += SPEED_INCREASE;
        speedX = speed * rX;
        speedY = speed * rY;
        bullets = 3;
        state = 0;
    }
}

void Lab3::ScoreRender() {

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(SCORE_X, SCORE_Y);
    modelMatrix *= transform2D::Scale(0.1 * score, 1);
    RenderMesh2D(meshes["scoreFill"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(SCORE_X, SCORE_Y);
    modelMatrix *= transform2D::Scale(4, 1);
    RenderMesh2D(meshes["scoreBar"], shaders["VertexColor"], modelMatrix);
}

void Lab3::BulletsAndLifesRender() {

    for (int i = 0; i < lifes; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(i * BULLETLIFE_SIZE + 50, 650);
        modelMatrix *= transform2D::Scale(20, 20);
        RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
    }

    for (int j = 0; j < bullets; j++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(j * BULLETLIFE_SIZE + 35, 550);
        modelMatrix *= transform2D::Scale(0.5, 1);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
    }
}

void Lab3::TargetRender() {

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(targetX, targetY);
    modelMatrix *= transform2D::Scale(5, 5);
    RenderMesh2D(meshes["target"], shaders["VertexColor"], modelMatrix);
}

void Lab3::GrassRender() {

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(0, 0);
    modelMatrix *= transform2D::Scale(30, 3);
    RenderMesh2D(meshes["grass"], shaders["VertexColor"], modelMatrix);
}

void Lab3::DuckRender(float deltaTimeSeconds) {

    modelMatrix *= transform2D::Scale(DUCK_SCALE, DUCK_SCALE);
    modelMatrixAux = modelMatrix;
    modelMatrix *= transform2D::Scale(40, 40);
    RenderMesh2D(meshes["head"], shaders["VertexColor"], modelMatrix);
    modelMatrix = modelMatrixAux;

    modelMatrix *= transform2D::Translate(-50, -BODY_SIZE); // translateX + 70, translateY -100
    modelMatrixAux = modelMatrix;
    modelMatrix *= transform2D::Translate(MIDDLE, BODY_SIZE);
    modelMatrix *= transform2D::Scale(1.5, 1.5);
    modelMatrix *= transform2D::Rotate(-M_PI / 2);
    modelMatrix *= transform2D::Translate(-MIDDLE, -BODY_SIZE);
    RenderMesh2D(meshes["triangle"], shaders["VertexColor"], modelMatrix);
    modelMatrix = modelMatrixAux;

    modelMatrix *= transform2D::Translate(110, -5);//translateX + 180, translateY - 105
    modelMatrixAux = modelMatrix;
    modelMatrix *= transform2D::Translate(MIDDLE, BODY_SIZE);
    modelMatrix *= transform2D::Scale(0.5, 0.5);
    modelMatrix *= transform2D::Rotate(-M_PI / 2);
    modelMatrix *= transform2D::Translate(-MIDDLE, -BODY_SIZE);
    RenderMesh2D(meshes["beak"], shaders["VertexColor"], modelMatrix);
    modelMatrix = modelMatrixAux;

    modelMatrix *= transform2D::Translate(-180, BODY_SIZE);//translateX , translateY
    modelMatrixAux = modelMatrix;
    WingMove(deltaTimeSeconds);
    RenderMesh2D(meshes["triangle"], shaders["VertexColor"], modelMatrix);
    modelMatrix = modelMatrixAux;

    modelMatrix *= transform2D::Translate(MIDDLE, 0);
    modelMatrix *= transform2D::Rotate(-angularStep);
    modelMatrix *= transform2D::Translate(-MIDDLE, 0);
    RenderMesh2D(meshes["triangle"], shaders["VertexColor"], modelMatrix);
}

void Lab3::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab3::OnInputUpdate(float deltaTime, int mods)
{

}


void Lab3::OnKeyPress(int key, int mods)
{
    // Add key press event
    restart = 0;
}


void Lab3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    float sy = window->GetResolution().y;
    targetX = mouseX;
    targetY = sy - mouseY;
}


void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if(bullets <= 0){
        return;
    }
    float sy = window->GetResolution().y;

    float x = translateX + 130;
    float y = sy - translateY;

    float tstX = abs(mouseX - x);
    float tstY = abs(mouseY - y);

    bullets -= 1;


    if (tstX < HIT_BOX && tstY < HIT_BOX) {
        killed = 1;
    }
   
}


void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}


void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab3::OnWindowResize(int width, int height)
{
}
