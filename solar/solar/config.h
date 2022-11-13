#pragma once

int screenWidth = 1000;
int screenHeight = 800;
#include "camera.hpp"
#include <vector>

#include <string>

enum class STARTYPE{
    Sun,        // ̫��
    Mercury,    // ˮ��
    Venus,      // ����
    Earth,      // ����
    Moon,       // ����
    Mars,       // ����
    Jupiter,    // ľ��
    Saturn,     // ����
    Uranus,     // ������
    Neptune     // ������
};


namespace app {
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec3 p0 = glm::vec3(0.0f, 0.0f, 0.0f);
    const int orbitACC = 3000;
    const float orbitRadius = 100.0f;
    GLfloat x_pos = 0.0f;
    GLfloat y_pos = 0.0f;
    GLfloat lastTime;
    GLfloat deltaTime;
    GLfloat currentTime;
    
    bool mouse_flag = true;
    double lastX, lastY;
    GLuint orbit_VAO, orbit_VBO;
    GLuint sky_VAO, sky_VBO;
    const GLfloat speed = 100.f;
    const GLfloat rotateSpeed = 50.f;
    const GLfloat AU = 149.6f; //һ�����ĵ�λ�ĳ��ȣ���λ��ʮ��ǧ��

    const GLfloat distances[] = {
        0.f * AU, 
        1.f * AU,
        2.f * AU,
        3.f * AU,
        4.f * AU,
        5.f * AU,
        6.f * AU,
        7.f * AU,
        8.f * AU,
        0.3f * AU
    };
    const std::string listTextures[] = {
        "./̫��.bmp",
        "./ˮ��.bmp",
        "./����.bmp",
        "./����.bmp",
        "./����.bmp",
        "./ľ��.bmp",
        "./����.bmp",
        "./������.bmp",
        "./������.bmp",
        "./����.bmp",
    };
    std::vector<GLfloat> orbits;
    std::string skys[6] = {
        "./sky.jpg",
        "./sky.jpg",
        "./sky.jpg",
        "./sky.jpg",
        "./sky.jpg",
        "./sky.jpg"
    };
    unsigned int sky_texture;
    bool camera_rotate = false;
    GLfloat rotate_x = 0.f, rotate_y = 0.f;
}

