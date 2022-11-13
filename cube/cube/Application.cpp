//�ƿ�2�� 2052697 ����
//x86ģʽ�����У���Ȼ�ᱨ��
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

using std::cout;
using std::endl;
using std::ios;
using std::string;
using std::ifstream;
using std::stringstream;
using std::pow;

glm::mat4 model;
glm::mat4 camera;
glm::mat4 projection;

static float scale = 5.0f;
static float translate_x = 0.0f;
static float translate_y = 0.0f;
static float translate_z = 0.0f;

static float camera_pos_x = 0.0f;
static float camera_pos_y = 0.0f;
static float camera_pos_z = -40.0f;//�����40

static float rotateAngle = 0.0f;
static float increaseRotateAngle = 5.0f;
static int screenWidth = 1000;
static int screenHeight = 1000;
const float sense_scroll = 0.05f;//���ֵ�������
const float sense_board = 1.0f;//���̵�������
const float sense_mouse = 0.1f;//����������

static unsigned compileShader(unsigned type, const string& source)
{
	unsigned id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Fail to compile " << 
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader!" << endl;//���������Ϣ
		cout << message << endl;//���������Ϣ
		glDeleteShader(id);
	}
	
	return id;
}

static unsigned createShader(const string& path_vertex_shader, const string& path_fragment_shader) 
{
	unsigned program = glCreateProgram();
	ifstream fout;
	fout.open(path_vertex_shader,ios::in);
	if (!fout.is_open()) {
		cout << "Fail to open " << path_vertex_shader << endl;
		return -1;
	}

	stringstream buffer1;
	buffer1 << fout.rdbuf();
	string vertex(buffer1.str());
	buffer1.clear();
	fout.close();

	fout.open(path_fragment_shader, ios::in);
	if (!fout.is_open()) {
		cout << "Fail to open " << path_fragment_shader << endl;
		return -1;
	}

	stringstream buffer2;
	buffer2 << fout.rdbuf();
	string fragment(buffer2.str());
	buffer2.clear();
	fout.close();

	unsigned vs = compileShader(GL_VERTEX_SHADER, vertex);
	unsigned fs = compileShader(GL_FRAGMENT_SHADER, fragment);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void mouse(GLFWwindow* window, double x, double y)
{
	//cout << "x = " << x << endl;
	//cout << "y = " << y << endl;
	camera_pos_x = sense_mouse * (x - 0.5f * screenWidth);
	camera_pos_y = sense_mouse * (y - 0.5f * screenHeight);
}

void scroll(GLFWwindow* window, double xoffset, double yoffset) 
{
	//cout << yoffset << endl;
	scale *= (float)pow(2.0, (double)(yoffset * sense_scroll));
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//ESC�˳�
		glfwSetWindowShouldClose(window, true);
}

void framebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenHeight = height;
	screenWidth = width;
}

void cb_key(GLFWwindow* win, int key, int code, int action, int mods) 
{
	if (key == GLFW_KEY_R && (action == GLFW_PRESS||action == GLFW_REPEAT)) {//����R������ת
		rotateAngle += increaseRotateAngle;
	}
}

int main()
{
	if (glfwInit() == GL_FALSE) {
		cout << "Fail to init GLFW" << endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//����ֻ��MACϵͳ������MACϵͳ�±���������������
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "liuyi 2052697", nullptr, nullptr);
	
	if (window == nullptr) {
		cout << "Fail to create the window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);//������

	if (glewInit() != GLEW_OK) {
		cout << "Fail to init GLEW" << endl;
		glfwTerminate();
		return -1;
	}

	//���´����꣬����������
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Ч�����ã�����
	glfwSetCursorPosCallback(window, mouse); //�������ص�����

	glViewport(0, 0, screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSize);
	glfwSetScrollCallback(window, scroll);
	glfwSetKeyCallback(window, cb_key);

	unsigned program = createShader("vertex_shader.glsl", "fragment_shader.glsl");
	
	unsigned VAO;
	unsigned VBO;
	unsigned EBO;
	
	float vertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};

	unsigned indices[] = { 
		0, 1, 2, // ��1��������
		3, 4, 5, // ��2��������

		6, 7, 8, // ��3��������
		9, 10, 11,// ��4��������

		12, 13, 14,// ��5��������
		15, 16, 17,// ��6��������

		18, 19, 20,// ��7��������
		21, 22, 23,// ��8��������

		24, 25, 26,// ��9��������
		27, 28, 29,// ��10��������

		30, 31, 32,// ��11��������
		33, 34, 35// ��12��������
	};

	//��������
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//���ݻ�����
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//��������
	
	//�������ԣ�ע�ⲽ��Ϊ6 * sizeof(float)
	//������λ�����ԣ�ƫ����Ϊ0��3��ֵ
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Ȼ������ɫ���ԣ�ƫ����Ϊ3*sizeof(float)��3��ֵ
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);//��������

		model = glm::mat4(1.0f);//ģ��
		camera = glm::mat4(1.0f);//���
		projection = glm::mat4(1.0f);//ͶӰ

		model = glm::scale(model, glm::vec3(scale, scale, scale));
		model = glm::translate(model, glm::vec3(translate_x, translate_y, translate_z));
		model = glm::rotate(model, glm::radians(rotateAngle), glm::vec3(1.0f, 0.0f, 1.0f));
		camera = glm::translate(camera, glm::vec3(camera_pos_x, camera_pos_y, camera_pos_z));
		if (screenHeight != 0)
			projection = glm::perspective(glm::radians(45.0f),
				(float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		
		glUseProgram(program); //��ɫ��

		unsigned int modelLoc = glGetUniformLocation(program, "model");
		unsigned int viewLoc = glGetUniformLocation(program, "view");
		unsigned int projectionLoc = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &camera[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

		glfwPollEvents();

		//�����Ļ
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//��ɫ��ɫ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();

	return 0;
}
