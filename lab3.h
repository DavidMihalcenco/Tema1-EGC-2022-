#pragma once

#include "components/simple_scene.h"
#include <math.h>
#include <components/text_renderer.h>


namespace m1
{
    class Lab3 : public gfxc::SimpleScene
    {
     public:
        Lab3();
        ~Lab3();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        float cx, cy;
        glm::mat3 modelMatrix;
        glm::mat3 modelMatrixAux;
        gfxc::TextRenderer* textRenderer;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
        float angularStep1;
        int state;
        int collisionX;
        int collisionY;
        float clock;
        int killed;
        int lifes;
        int bullets;
        int restart;
        int score;
        int fly;
        float speed;
        float targetX;
        float targetY;
        float speedX;
        float speedY;
        int windowResolutionX;
        int windowResolutionY;
        // TODO(student): If you need any other class variables, define them here.

        void WingMove(float deltaTimeSeconds);
        void FlyDirection(float deltaTimeSeconds); 
        void FlyOrientation(float deltaTimeSeconds);
        void RestartGame();
        void FlyAway();
        void Killed();
        void ScoreRender();
        void BulletsAndLifesRender(); 
        void TargetRender();
        void GrassRender();
        void DuckRender(float deltaTimeSeconds);

    };
}   // namespace m1
