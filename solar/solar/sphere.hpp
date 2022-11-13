#pragma once
//关于球体如何渲染
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
using std::vector;

class Sphere
{
public:
	vector<GLfloat> vertices;
	vector<GLfloat> texcoord;
	vector<GLuint> indices;
	Sphere();
	Sphere(GLfloat radius, GLuint sectors, GLuint stacks, glm::vec3 color);
	~Sphere();
	void draw();
private:
	GLfloat radius;
	GLuint sectors;
	GLuint stacks;
	GLuint VBO, EBO, VAO;
	glm::vec3 color;
};

inline Sphere::Sphere()
{
	this->radius = 0;
	this->sectors = this->stacks = 1;
	VBO = EBO = VAO = -1;
	this->color = glm::vec3(0.0f, 0.0f, 0.0f);
}

inline Sphere::Sphere(GLfloat radius, GLuint sectors, GLuint stacks, glm::vec3 color)
	:radius(radius), sectors(sectors), stacks(stacks), color(color)
{
	//算出步长，一个圆划分成多少份
	GLfloat sector_step = 2.0f * glm::pi<GLfloat>() / sectors;
	GLfloat stack_step = glm::pi<GLfloat>() / stacks;
	GLfloat ag_sector = 0.0f, ag_stack = 0.0f;

	//顶点数组初始化
	for (unsigned int i = 0; i <= stacks; i++) {
		ag_stack = glm::pi<GLfloat>() / 2 - i * stack_step; //[-pi/2,pi/2]

		GLfloat xy = cos(ag_stack) * radius;//注意第一步先投影到xy平面上面去
		GLfloat z = sin(ag_stack) * radius;//然后投影得到z值
		//接下来再一次拆解xy
		for (unsigned int j = 0; j <= sectors; j += 3) {
			ag_sector = j * sector_step;//从0到2pi
			GLfloat x1 = xy * cos(ag_sector);
			GLfloat y1 = xy * sin(ag_sector);
			GLfloat s1 = (GLfloat)j / sectors;
			GLfloat t1 = (GLfloat)i / stacks;

			ag_sector = (j + 1) * sector_step;//从0到2pi
			GLfloat x2 = xy * cos(ag_sector);
			GLfloat y2 = xy * sin(ag_sector);
			GLfloat s2 = (GLfloat)(j + 1) / sectors;
			GLfloat t2 = (GLfloat)i / stacks;

			ag_sector = (j + 2) * sector_step;//从0到2pi
			GLfloat x3 = xy * cos(ag_sector);
			GLfloat y3 = xy * sin(ag_sector);
			GLfloat s3 = (GLfloat)(j + 2) / sectors;
			GLfloat t3 = (GLfloat)i / stacks;

			//计算法向量，三角形法向量算法
			glm::vec3 a = glm::vec3(x1 - x2, y1 - y2, z - z);
			glm::vec3 b = glm::vec3(x1 - x3, y1 - y3, z - z);
			glm::vec3 normal = glm::normalize(glm::cross(a, b));
			//扔进顶点数组
			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z);

			vertices.push_back(color.x);
			vertices.push_back(color.y);
			vertices.push_back(color.z);

			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);

			vertices.push_back(s1);
			vertices.push_back(t1);

			vertices.push_back(x2);
			vertices.push_back(y2);
			vertices.push_back(z);

			vertices.push_back(color.x);
			vertices.push_back(color.y);
			vertices.push_back(color.z);

			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);

			vertices.push_back(s2);
			vertices.push_back(t2);

			vertices.push_back(x3);
			vertices.push_back(y3);
			vertices.push_back(z);

			vertices.push_back(color.x);
			vertices.push_back(color.y);
			vertices.push_back(color.z);

			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);

			vertices.push_back(s3);
			vertices.push_back(t3);
		}
	}

	//序号数组（按这个序列渲染）初始化
	GLint a = 0, b = 0;
	for (unsigned int i = 0; i < stacks; i++) {
		a = i * (sectors + 1);
		b = a + sectors + 1;

		for (unsigned int j = 0; j < sectors; j++, a++, b++) {
			if (i != 0) {
				//三个点组成一个小三角形，按三角形的方法渲染球面
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(a + 1);
			}

			if (i != sectors - 1) {
				//第二个小三角形，组成一个平行四边形
				indices.push_back(a + 1);
				indices.push_back(b);
				indices.push_back(b + 1);
			}
		}
	}

	//需要准备数据
	glGenVertexArrays(1, &VAO); //取一个顶点数组
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO); //数据缓冲区
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);


	glGenBuffers(1, &EBO); //数据缓冲区
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

	//设置属性，注意步长为11 * sizeof(float)
	//偏移量为0，3个值
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//设置属性，注意步长为11 * sizeof(float)
	//偏移量为3，3个值
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//设置属性，注意步长为11 * sizeof(float)
	//偏移量为6，3个值
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//设置属性，注意步长为11 * sizeof(float)
	//偏移量为9，3个值
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

inline Sphere::~Sphere()
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Sphere::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}
