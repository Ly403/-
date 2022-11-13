//计科2班 2052697 刘毅
//x86模式下运行，不然会报错
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
static float camera_pos_z = -40.0f;//相机撤40

static float rotateAngle = 0.0f;
static float increaseRotateAngle = 5.0f;
static int screenWidth = 1000;
static int screenHeight = 1000;
const float sense_scroll = 0.05f;//滚轮的灵敏度
const float sense_board = 1.0f;//键盘的灵敏度
const float sense_mouse = 0.1f;//鼠标的灵敏度

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
			<< " shader!" << endl;//输出错误信息
		cout << message << endl;//输出错误信息
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//ESC退出
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
	if (key == GLFW_KEY_R && (action == GLFW_PRESS||action == GLFW_REPEAT)) {//输入R可以旋转
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

	glfwMakeContextCurrent(window);//上下文

	if (glewInit() != GLEW_OK) {
		cout << "Fail to init GLEW" << endl;
		glfwTerminate();
		return -1;
	}

	//以下处理光标，处理鼠标操作
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //效果不好，撤了
	glfwSetCursorPosCallback(window, mouse); //设置鼠标回调函数

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
		0, 1, 2, // 第1个三角形
		3, 4, 5, // 第2个三角形

		6, 7, 8, // 第3个三角形
		9, 10, 11,// 第4个三角形

		12, 13, 14,// 第5个三角形
		15, 16, 17,// 第6个三角形

		18, 19, 20,// 第7个三角形
		21, 22, 23,// 第8个三角形

		24, 25, 26,// 第9个三角形
		27, 28, 29,// 第10个三角形

		30, 31, 32,// 第11个三角形
		33, 34, 35// 第12个三角形
	};

	//顶点数组
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//数据缓冲区
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//属性设置
	
	//设置属性，注意步长为6 * sizeof(float)
	//首先是位置属性，偏移量为0，3个值
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//然后是颜色属性，偏移量为3*sizeof(float)，3个值
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);//处理输入

		model = glm::mat4(1.0f);//模型
		camera = glm::mat4(1.0f);//相机
		projection = glm::mat4(1.0f);//投影

		model = glm::scale(model, glm::vec3(scale, scale, scale));
		model = glm::translate(model, glm::vec3(translate_x, translate_y, translate_z));
		model = glm::rotate(model, glm::radians(rotateAngle), glm::vec3(1.0f, 0.0f, 1.0f));
		camera = glm::translate(camera, glm::vec3(camera_pos_x, camera_pos_y, camera_pos_z));
		if (screenHeight != 0)
			projection = glm::perspective(glm::radians(45.0f),
				(float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		
		glUseProgram(program); //着色器

		unsigned int modelLoc = glGetUniformLocation(program, "model");
		unsigned int viewLoc = glGetUniformLocation(program, "view");
		unsigned int projectionLoc = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &camera[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

		glfwPollEvents();

		//清除屏幕
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//黑色底色
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
