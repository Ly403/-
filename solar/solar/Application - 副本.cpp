//计科2班 2052697 刘毅
//x86模式下运行，不然会报错
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "config.h"
#include "camera.hpp"
#include "shader.hpp"
#include "sphere.hpp"
#include "stb_image.h"
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::string;

unsigned load(string path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else{
        cout << "Failed to load at path: " << path << endl;
        stbi_image_free(data);
    }

    return textureID;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);//退出窗口的按钮
}


void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);;
}

void scroll_callback(GLFWwindow* window, double x, double y)
{
    if (y == 1)
        app::camera.keyboard_process(camera_config::move::scrool_forward, app::deltaTime);
    else
        app::camera.keyboard_process(camera_config::move::scrool_backward, app::deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key)
        {
            case GLFW_KEY_W:
                app::camera.keyboard_process(camera_config::move::forward, app::deltaTime);
                break;
            case GLFW_KEY_S:
                app::camera.keyboard_process(camera_config::move::backword, app::deltaTime);
                break;
            case GLFW_KEY_A:
                app::camera.keyboard_process(camera_config::move::left, app::deltaTime);
                break;
            case GLFW_KEY_D:
                app::camera.keyboard_process(camera_config::move::right, app::deltaTime);
                break;
            default:
                break;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)
        && action == GLFW_PRESS)
        app::camera_rotate = true;
    else
        app::camera_rotate = false;
} 

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (app::camera_rotate) {
        if (app::mouse_flag) {
            app::mouse_flag = false;
            app::lastX = xpos;
            app::lastY = ypos;
        }
        app::camera.mouse_process(xpos, ypos,app::lastX,app::lastY);
    };
}

void skyData()
{
    GLfloat vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    

    glGenVertexArrays(1, &app::sky_VAO);
    glGenBuffers(1, &app::sky_VBO); //数据缓冲区
    glBindVertexArray(app::sky_VAO); //顶点数组
    glBindBuffer(GL_ARRAY_BUFFER, app::sky_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    
    //偏移量为0，3个值
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); //先撤VBO
    glBindVertexArray(0); //后撤VAO


    glGenTextures(1, &app::sky_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, app::sky_texture);
    // 加载并生成纹理
    int width, height, nrChannels;
    for (int i = 0; i < 6; i++) {
        unsigned char* data = stbi_load(app::skys[i].c_str(), &width, &height, &nrChannels, 0);
        GLenum format;
        if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        if (data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            //glGenerateMipmap(GL_TEXTURE_2D);
        }
        else{
            std::cout << "Failed to load texture" << std::endl;
            stbi_image_free(data);
        }
        stbi_image_free(data);
    }
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void orbitData()
{
    GLfloat x, z;
    GLfloat angle = 0;
    
    for (int i = 0; i < 2 * app::orbitACC; i++) {
        angle = glm::pi<GLfloat>() / 2 - i * (glm::pi<GLfloat>() / app::orbitACC);
        x = 1.f * sin(angle);
        z = 1.f * cos(angle);

        app::orbits.push_back(x);
        app::orbits.push_back(0.0f);
        app::orbits.push_back(z);
    }


    glGenVertexArrays(1, &app::orbit_VAO);
    glBindVertexArray(app::orbit_VAO); //顶点数组

    glGenBuffers(1, &app::orbit_VBO); //数据缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, app::orbit_VBO);
    glBufferData(GL_ARRAY_BUFFER, app::orbits.size() * sizeof(GLfloat), app::orbits.data(), GL_STATIC_DRAW);
    
    //偏移量为0，3个值
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main()
{
    app::camera.LookAt = app::p0;

    //初始化glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    //以下只在MAC系统运作，MAC系统下必须运行下面这行
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "liuyi 2052697", nullptr, nullptr);

    if (window == nullptr) {
        cout << "Fail to create the window" << endl;
        glfwTerminate();
        return -1;
    }

    //创建窗口并做初始化
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    //继续初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    //开启深度检测
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    //如果设置了glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);则表示源颜色乘以自身的alpha 值，目标颜色乘以1.0减去源颜色的alpha值，
    //这样一来，源颜色的alpha值越大，则产生的新颜色中源颜色所占比例就越大，
    //而目标颜色所占比例则减小。
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (window == nullptr)
        return -1;

    //准备数据
    skyData();
    orbitData();
    
    Sphere Sun = Sphere(100.10f, 360, 180, glm::vec3(1.f, 0.f, 0.f));//1,391,000 千米
    Sphere Mercury = Sphere(4.880f, 360, 180, glm::vec3(135.f / 255, 206.f / 255, 235.f / 255)); //4,880 千米 
    Sphere Venus = Sphere(12.1036f, 36, 18, glm::vec3(255.f / 255, 165.f / 255, 0.f / 255)); // 12,103.6 千米 
    Sphere Earth = Sphere(12.7563f, 36, 18, glm::vec3(135.f / 255, 206.f / 255, 250.f / 255));//12,756.3 千米 
    Sphere Mars = Sphere(6.794f, 36, 18, glm::vec3(205.f / 255, 104.f / 255, 57.f / 255)); // 6,794 千米 
    Sphere Jupiter = Sphere(60.984f, 36, 18, glm::vec3(205.f / 255, 170.f / 255, 125.f / 255));//142,984 千米 (赤道) 
    Sphere Saturn = Sphere(50.536f, 36, 18, glm::vec3(255.f / 255, 231.f / 255, 186.f / 255)); //120,536 千米 (赤道) 
    Sphere Uranus = Sphere(30.118f, 36, 18, glm::vec3(0.f / 255, 0.f / 255, 205.f / 255));// 51,118 千米（赤道） 
    Sphere Neptune = Sphere(30.532f, 36, 18, glm::vec3(100.f / 255, 149.f / 255, 237.f / 255));//49,532 千米（赤道） 
    Sphere Moon = Sphere(3.467f, 36, 18, glm::vec3(211.f / 255, 211.f / 255, 211.f / 255));// 3467千米
    //读入着色器
    shader sphere_proc("./sphere_vs.glsl","./sphere_fs.glsl");
    shader sky_proc("./sky_vs.glsl", "./sky_fs.glsl");

    /* 加载纹理 */
    unsigned textures[10] = {};
    for (int i = 0; i < 10; i++) {
        textures[i] = load(app::listTextures[i]);
    }

    (app::camera).position = glm::vec3(0.0f, 250.0f, 1000.0f);
    (app::camera).yaw = 90.0f;
    (app::camera).pitch = -85.964f; //向下俯视
    //(app::camera).mouse_process(app::x_pos, app::y_pos);

    glm::mat4 view = glm::mat4(1.0f);
    glm::vec3 positions[10];
    while (!glfwWindowShouldClose(window)){
        // 输入
        GLdouble x = 0, z = 0;
        app::currentTime = (GLfloat)glfwGetTime();
        app::deltaTime = app::currentTime - app::lastTime;
        app::lastTime = app::currentTime;
        
        processInput(window);

        // 渲染指令
        glm::mat4 model = glm::mat4(1.0f);
        //投影，得到透视矩阵
        view = app::camera.view();
        glm::mat4 projection = glm::perspective(glm::radians(app::camera.FOV),
            (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 15000.0f);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        sphere_proc.use();
        sphere_proc.setMat4("model", model);
        sphere_proc.setMat4("view", view);
        sphere_proc.setMat4("projection", projection);

        /*
            这里列举以下各个星球公转速度和离地球距离
            一、水星
            公转速度：47.87 km/s 英文名：Mercury 离太阳距离：0.387 AU 公转周期：87.70d
            二、金星
            公转速度：35 km/s 英文名：Venus 离太阳距离：0.723AU 公转周期：224.701d
            三、地球
            平均公转速度：29.783km/s 英文： Earth 离太阳距离：1AU 公转周期：365.2422d
            四、火星
            平均公转速度：24.13 km/s 英文名： Mars 离太阳距离：1.524 AU 公转周期：686.98d
            五、木星
            平均公转速度：13.07 km/s 英文名： Jupiter 离太阳距离 5.205 AU 公转周期：11.8618y
            六、土星
            平均公转速度：9.69 km/s 英文名： Saturn 离太阳距离：9.576 AU 公转周期：10832.327d
            七、天王星
            平均公转速度：6.81km/s 英文名： Uranus 离太阳距离：19.18 AU 公转周期：84.323326y
            八、海王星
            公转速度：5.43 km/s 英文名： Neptune 离太阳距离：30.13 AU 公转周期：60327.624d
            九、月球
            公转速度：1.023km/s 英文名：Moon 离地球距离：0.000256 AU 公转周期：27.32166d
       */


        //注意一下统一把公转角速度扩大100倍
        // 没办法不能保证 比例一致了，那样看不到行星了
        //太阳
        sphere_proc.setVec3("pointLight.position", app::p0.x, app::p0.y, app::p0.z);
        sphere_proc.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
        sphere_proc.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        //设定衰减相关系数
        sphere_proc.setFloat("pointLight.k0", 1.f);
        sphere_proc.setFloat("pointLight.k1", 0.09f);
        sphere_proc.setFloat("pointLight.k2", 0.32f);
        sphere_proc.setVec3("viewPos", app::camera.position.x, app::camera.position.y, app::camera.position.z);
        sphere_proc.setVec3("material.specular", 0.f, 0.f, 0.f);
        sphere_proc.setFloat("material.shininess", 32.f);

        glm::mat4 sun = glm::mat4(1.0f);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        sun = glm::rotate(sun, (GLfloat)glfwGetTime() * glm::radians(25.38f), glm::vec3(0.0f, 1.0f, 0.f));
        sphere_proc.setMat4("model", sun);
        Sun.draw();
        positions[0] = app::p0;

        ////水星
        glm::mat4 mercury = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        mercury = glm::translate(mercury, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 87.70))
            * app::distances[1];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 87.70))
            * app::distances[1];
        mercury = glm::translate(mercury, glm::vec3(x, 0.0f, z));
        mercury = glm::rotate(mercury, (GLfloat)glfwGetTime() * glm::radians(58.65f), glm::vec3(0.0f, 1.0f, 0.f));
        positions[1] = glm::vec3(x, 0.0f, z);
        sphere_proc.setMat4("model", mercury);
        Mercury.draw();

        //金星
        glm::mat4 venus = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        venus = glm::translate(venus, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() /  224.701))
            * app::distances[2];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 224.701))
            * app::distances[2];
        venus = glm::translate(venus, glm::vec3(x, 0.0f, z));
        positions[2] = glm::vec3(x, 0.0f, z);
        venus = glm::rotate(venus, (GLfloat)glfwGetTime() * glm::radians(243.f), glm::vec3(0.0f, -1.0f, 0.f));
        sphere_proc.setMat4("model", venus);
        Venus.draw();

        ////地球
        glm::mat4 earth = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[3]);
        earth = glm::translate(earth, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 365.2422))
            * app::distances[3];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 365.2422))
            * app::distances[3];
        earth = glm::translate(earth, glm::vec3(x, 0.0f, z));
        earth = glm::rotate(earth, (GLfloat)glfwGetTime() * glm::radians(1.f), glm::vec3(0.0f, 1.0f, 0.f));
        positions[3] = glm::vec3(x, 0.0f, z);
        sphere_proc.setMat4("model", earth);
        Earth.draw();

        //火星
        glm::mat4 mars = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[4]);
        mars = glm::translate(mars, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 686.98))
            * app::distances[4];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 686.98))
            * app::distances[4];
        mars = glm::translate(mars, glm::vec3(x, 0.0f, z));
        mars = glm::rotate(mars, (GLfloat)glfwGetTime() * glm::radians(1.1f), glm::vec3(0.0f, 1.0f, 0.f));
        positions[4] = glm::vec3(x, 0.0f, z);
        sphere_proc.setMat4("model", mars);
        Mars.draw();

        //木星
        glm::mat4 jupiter = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[5]);
        jupiter = glm::translate(jupiter, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / (11.8618 * 365)))
            * app::distances[5];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / (11.8618 * 365)))
            * app::distances[5];
        jupiter = glm::translate(jupiter, glm::vec3(x, 0.0f, z));
        jupiter = glm::rotate(jupiter, (GLfloat)glfwGetTime() * glm::radians(0.3f), glm::vec3(0.0f, 1.0f, 0.f));
        positions[5] = glm::vec3(x, 0.0f, z);
        sphere_proc.setMat4("model", jupiter);
        Jupiter.draw();

        //土星
        glm::mat4 saturn = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[6]);
        saturn = glm::translate(saturn, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 10832.327))
            * app::distances[6];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 10832.327))
            * app::distances[6];
        saturn = glm::translate(saturn, glm::vec3(x, 0.0f, z));
        saturn = glm::rotate(saturn, (GLfloat)glfwGetTime() * glm::radians(0.4f), glm::vec3(0.0f, 1.0f, 0.f));
        positions[6] = glm::vec3(x, 0.0f, z);
        sphere_proc.setMat4("model", saturn);
        Saturn.draw();

        //海王星
        glm::mat4 uranus = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[7]);
        uranus = glm::translate(uranus, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / (84.323326 * 365)))
            * app::distances[7];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / (84.323326 * 365)))
            * app::distances[7];
        uranus = glm::translate(uranus, glm::vec3(x, 0.0f, z));
        uranus = glm::rotate(uranus, (GLfloat)glfwGetTime() * glm::radians(0.6f), glm::vec3(0.0f, 1.0f, 0.f));
        positions[7] = glm::vec3(x, 0.0f, z);
        sphere_proc.setMat4("model", uranus);
        Uranus.draw();

        //天王星
        glm::mat4 neptune = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[8]);
        neptune = glm::translate(neptune, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 60327.624))
            * app::distances[8];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 60327.624))
            * app::distances[8];
        neptune = glm::translate(neptune, glm::vec3(x, 0.0f, z));
        neptune = glm::rotate(neptune, (GLfloat)glfwGetTime() * glm::radians(0.7f), glm::vec3(0.0f, 1.0f, 0.f));
        positions[8] = glm::vec3(x, 0.0f, z);
        sphere_proc.setMat4("model", neptune);
        Neptune.draw();

        //月球特殊一点，从地球坐标系绘制
        glm::mat4 moon = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[9]);
        moon = glm::translate(moon, app::p0);
        x = sin(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 27.32166))
            * app::distances[9];
        z = cos(glfwGetTime() * app::speed * (2 * glm::pi<GLfloat>() / 27.32166))
            * app::distances[9];
        moon = glm::translate(moon, positions[3]);
        moon = glm::translate(moon, glm::vec3(x, 0.0f, z));
        moon = glm::rotate(moon, (GLfloat)glfwGetTime() * glm::radians(27.32f), glm::vec3(0.0f, 1.0f, 0.f));
        positions[9] = glm::vec3(x, 0.0f, z);
        sphere_proc.setMat4("model", moon);
        Moon.draw();


        glBindVertexArray(app::orbit_VAO);
        //glLineWidth(10.0f);
        glm::mat4 orbit = glm::mat4(1.0f);
        for (int i = 0; i < 8; i++) {
            orbit = glm::mat4(1.0f);
            orbit = glm::translate(orbit, app::p0);
            //通过缩放的方法绘制一系列同心圆
            orbit = glm::scale(orbit, glm::vec3(app::distances[i + 1], app::distances[i + 1], app::distances[i + 1]));
            sphere_proc.setMat4("model", orbit);
            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(app::orbits).size() / 3);
        }
        //特殊的，关于月球绕地球的轨道圆的绘制
        orbit = glm::mat4(1);
        orbit = glm::translate(orbit, positions[3]);
        orbit = glm::scale(orbit, glm::vec3(app::distances[9], app::distances[9], app::distances[9]));
        sphere_proc.setMat4("model", orbit);
        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(app::orbits).size() / 3);

        //绘制星空
        /*
        不再需要模型矩阵
        LookAt矩阵忽略位移，前两条是为了保证天空盒位置及其中心点永不变
        保证进入片段着色器后，任意点的深度值都为最大值1.0（天空盒可以被任意物体挡住）glDepthFunc(GL_LEQUAL)
        */
        glDepthFunc(GL_LEQUAL);
        sky_proc.use();
        view = glm::mat4(glm::mat3(app::camera.view()));
        sky_proc.setMat4("view", view);
        sky_proc.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(app::sky_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, app::sky_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
        sky_proc.use();
        glDepthFunc(GL_LEQUAL);
  
        // 检查并调用事件，交换缓冲
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &app::sky_VAO);
    glDeleteBuffers(1, &app::sky_VBO);
    glDeleteVertexArrays(1, &app::orbit_VAO);
    glDeleteBuffers(1, &app::orbit_VBO);
    glfwTerminate();
    return 0;
}