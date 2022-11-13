#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace camera_config {
	enum class move {
		left, right, forward, backword, scrool_forward, scrool_backward
	};
	const GLfloat YAW = -90.0f; //默认角度
	const GLfloat PITCH = 0.0f;
	const GLfloat SPEED = 5000.0f; //默认速度
	const GLfloat SENSITIVTY = 0.05f; //默认鼠标灵敏度
	const GLfloat ZOOM = 45.0f; //默认缩放
};

class Camera
{
public:
	glm::vec3 LookAt; //观察矩阵
	glm::vec3 position; //摄像机位置
	glm::vec3 front; 
	glm::vec3 up; 
	glm::vec3 right; 
	glm::vec3 worldUp;  //世界坐标系中的上
	
	//关于视角移动，总共三个欧拉角，但是摄像机一般不会去考虑滚轮角
	GLfloat yaw;//偏航角
	GLfloat pitch;//俯视角

	//摄像机参数
	GLfloat speed; //移动速度
	GLfloat sense; //灵敏度
	GLfloat FOV; //缩放
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), //原始放在 0,0,0
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = camera_config::YAW,
		GLfloat pitch = camera_config::PITCH);
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ,
		GLfloat yaw, GLfloat pitch);
	glm::mat4 view();
	void scroll_process(double x, double y);//滚轮控制
	void keyboard_process(camera_config::move,GLfloat time);
	void mouse_process(double xpos, double ypos, double& lastX, double& lastY);
private:
	void update();
};


//可以用坐标初始化摄像机
inline Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ,
	GLfloat yaw, GLfloat pitch)
{
	this->front = glm::vec3(0.0f, 0.0f, -1.0f);
	speed = camera_config::SPEED;
	sense = camera_config::SENSITIVTY;
	FOV = camera_config::ZOOM;
	this->position = glm::vec3(posX, posY, posZ);
	this->worldUp = glm::vec3(upX, upY, upZ);
	this->yaw = yaw;
	this->pitch = pitch;
	this->update();
}

glm::mat4 Camera::view()
{
	//GLM已经提供了这些支持。我们要做的只是定义一个摄像机位置，
	//一个目标位置和一个表示世界空间中的上向量的向量
	//（我们计算右向量使用的那个上向量）。
	//接着GLM就会创建一个LookAt矩阵，我们可以把它当作我们的观察矩阵：
	//https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/#_5
	return glm::lookAt(this->position, this->front + this->position, this->up);
}

void Camera::scroll_process(double x, double y)
{
	// 当滚动鼠标滚轮的时候，yoffset值代表我们竖直滚动的大小。
	// 当scroll_callback函数被调用后，我们改变
	// 全局变量fov变量的内容。因为45.0f是默认的视野值，
	// 我们将会把缩放级别(Zoom Level)限制在1.0f到45.0f。
	if (FOV >= 1.0f && FOV <= 45.0f)
		FOV -= (GLfloat)y;
	if (FOV <= 1.0f)
		FOV = 1.0f;
	if (FOV >= 45.0f)
		FOV = 45.0f;
}

void Camera::keyboard_process(camera_config::move d, GLfloat time)
{
	GLfloat camera_speed = this->speed * time;
	switch (d)
	{
		case camera_config::move::left:
			this->position -= this->right * camera_speed;
			break;
		case camera_config::move::right:
			this->position += this->right * camera_speed;
			break;
		case camera_config::move::forward:
			this->position += this->front * camera_speed;
			break;
		case camera_config::move::backword:
			this->position -= this->front * camera_speed;
			break;
		case camera_config::move::scrool_forward:
			if (this->position.y > 70.0f)
				this->position += this->front * (10000.0f * time);
			break;
		case camera_config::move::scrool_backward:
			this->position -= this->front * (15000.0f * time);
			break;
		default:
			break;
	}
}

//鼠标输入处理
void Camera::mouse_process(double xpos, double ypos, double& lastX, double& lastY)
{
	GLfloat xoffset, yoffset;
	xoffset = (GLfloat)xpos - (GLfloat)lastX;
	yoffset = (GLfloat)ypos - (GLfloat)lastY;
	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;
	xoffset *= this->sense;
	yoffset *= this->sense;

	yaw += xoffset;
	pitch += yoffset;
	//我们需要给摄像机添加一些限制，这样摄像机就不会发生奇怪的移动了（这样也会避免一些奇怪的问题）。对于俯仰角，要让用户不能看向高于89度的地方（在90度时视角会发生逆转，所以我们把89度作为极限），同样也不允许小于 - 89度。
	//这样能够保证用户只能看到天空或脚下，但是不能超越这个限制。
	//我们可以在值超过限制的时候将其改为极限值来实现：
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
	//注意我们没有给偏航角设置限制，这是因为我们不希望限制用户的水平旋转。
	update();//更新方向向量
}

//可以用向量初始化摄像机
inline Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)) //注意z轴从屏幕指向外面，前就是 0,0,-1
{
	this->position = position;
	worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	speed = camera_config::SPEED;
	sense = camera_config::SENSITIVTY;
	FOV = camera_config::ZOOM;
	update();
}

/*
* https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/#_5
* direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw)); 
// 译注：direction代表摄像机的前轴(Front)，这个前轴是和
// 本文第一幅图片的第二个摄像机的方向向量是相反的
* direction.y = sin(glm::radians(pitch));
* direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
*/
void Camera::update()
{
	glm::vec3 front = glm::vec3(); //计算前轴
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(this->pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));


	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	//右向量、前向量和上向量可以通过叉乘互相算出，右 X 前 = 上（右手法则），做归一化得到单位向量
	this->up = glm::normalize(glm::cross(this->right, this->front));
}
