#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace camera_config {
	enum class move {
		left, right, forward, backword, scrool_forward, scrool_backward
	};
	const GLfloat YAW = -90.0f; //Ĭ�ϽǶ�
	const GLfloat PITCH = 0.0f;
	const GLfloat SPEED = 5000.0f; //Ĭ���ٶ�
	const GLfloat SENSITIVTY = 0.05f; //Ĭ�����������
	const GLfloat ZOOM = 45.0f; //Ĭ������
};

class Camera
{
public:
	glm::vec3 LookAt; //�۲����
	glm::vec3 position; //�����λ��
	glm::vec3 front; 
	glm::vec3 up; 
	glm::vec3 right; 
	glm::vec3 worldUp;  //��������ϵ�е���
	
	//�����ӽ��ƶ����ܹ�����ŷ���ǣ����������һ�㲻��ȥ���ǹ��ֽ�
	GLfloat yaw;//ƫ����
	GLfloat pitch;//���ӽ�

	//���������
	GLfloat speed; //�ƶ��ٶ�
	GLfloat sense; //������
	GLfloat FOV; //����
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), //ԭʼ���� 0,0,0
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = camera_config::YAW,
		GLfloat pitch = camera_config::PITCH);
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ,
		GLfloat yaw, GLfloat pitch);
	glm::mat4 view();
	void scroll_process(double x, double y);//���ֿ���
	void keyboard_process(camera_config::move,GLfloat time);
	void mouse_process(double xpos, double ypos, double& lastX, double& lastY);
private:
	void update();
};


//�����������ʼ�������
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
	//GLM�Ѿ��ṩ����Щ֧�֡�����Ҫ����ֻ�Ƕ���һ�������λ�ã�
	//һ��Ŀ��λ�ú�һ����ʾ����ռ��е�������������
	//�����Ǽ���������ʹ�õ��Ǹ�����������
	//����GLM�ͻᴴ��һ��LookAt�������ǿ��԰����������ǵĹ۲����
	//https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/#_5
	return glm::lookAt(this->position, this->front + this->position, this->up);
}

void Camera::scroll_process(double x, double y)
{
	// �����������ֵ�ʱ��yoffsetֵ����������ֱ�����Ĵ�С��
	// ��scroll_callback���������ú����Ǹı�
	// ȫ�ֱ���fov���������ݡ���Ϊ45.0f��Ĭ�ϵ���Ұֵ��
	// ���ǽ�������ż���(Zoom Level)������1.0f��45.0f��
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

//������봦��
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
	//������Ҫ����������һЩ���ƣ�����������Ͳ��ᷢ����ֵ��ƶ��ˣ�����Ҳ�����һЩ��ֵ����⣩�����ڸ����ǣ�Ҫ���û����ܿ������89�ȵĵط�����90��ʱ�ӽǻᷢ����ת���������ǰ�89����Ϊ���ޣ���ͬ��Ҳ������С�� - 89�ȡ�
	//�����ܹ���֤�û�ֻ�ܿ�����ջ���£����ǲ��ܳ�Խ������ơ�
	//���ǿ�����ֵ�������Ƶ�ʱ�����Ϊ����ֵ��ʵ�֣�
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
	//ע������û�и�ƫ�����������ƣ�������Ϊ���ǲ�ϣ�������û���ˮƽ��ת��
	update();//���·�������
}

//������������ʼ�������
inline Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)) //ע��z�����Ļָ�����棬ǰ���� 0,0,-1
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
// ��ע��direction�����������ǰ��(Front)�����ǰ���Ǻ�
// ���ĵ�һ��ͼƬ�ĵڶ���������ķ����������෴��
* direction.y = sin(glm::radians(pitch));
* direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
*/
void Camera::update()
{
	glm::vec3 front = glm::vec3(); //����ǰ��
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(this->pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));


	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	//��������ǰ����������������ͨ����˻���������� X ǰ = �ϣ����ַ��򣩣�����һ���õ���λ����
	this->up = glm::normalize(glm::cross(this->right, this->front));
}
