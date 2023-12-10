//////////////////////////////////////////////////////////////////////////////
//
//  Sphere.cpp
//  1. 球体的绘制（求出球面上所有的点）
//  2. 三角面片的构造
//  3. 利用统一变量进行数据传递
//////////////////////////////////////////////////////////////////////////////

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vmath.h>
#include <vector>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

//窗口大小参数
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float aspact = (float)4.0 / (float)3.0;




// 指定时间参数
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//指定摄像机
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


//句柄参数
GLuint vertex_array_object; // == VAO句柄
GLuint vertex_buffer_object; // == VBO句柄
GLuint element_buffer_object;//==EBO句柄
GLuint texture_buffer_object1; // 纹理对象句柄1
GLuint texture_buffer_object2; // 纹理对象句柄2
GLuint texture_buffer_object3; // 纹理对象句柄3
GLuint texture_buffer_object4;
int shader_program;//着色器程序句柄

//球的数据参数
std::vector<float> sphereVertices;
std::vector<int> sphereIndices;
const int Y_SEGMENTS = 20;
const int X_SEGMENTS = 20;
const float Radio = 1.0;
const GLfloat  PI = 3.14159265358979323846f;

std::vector<float> Vertices;



void initial(void)
{
	//进行球体顶点和三角面片的计算
	// 生成球的顶点
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2 * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2 * PI) * std::sin(ySegment * PI);
			Vertices.push_back(xPos);
			Vertices.push_back(yPos);
			Vertices.push_back(zPos);
			Vertices.push_back(xSegment);
			Vertices.push_back(ySegment);
		}
	}

	// 生成球的顶点
	for (int i = 0; i < Y_SEGMENTS; i++)
	{
		for (int j = 0; j < X_SEGMENTS; j++)
		{

			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);

			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
		}
	}

	// 球
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object);

	glBindVertexArray(vertex_array_object);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float) * 5, &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);




	//地球
	glGenTextures(1, &texture_buffer_object1);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_object1);

	//指定纹理的参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	int width, height, nrchannels;

	unsigned char* data = stbi_load("res/earth.jpg", &width, &height, &nrchannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//生成Mipmap纹理
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);//释放资源

	//太阳
	glGenTextures(1, &texture_buffer_object2);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_object2);

	//指定纹理的参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//加载纹理
	//stbi_set_flip_vertically_on_load(true);
	//加载纹理图片wall.jpg或者htmdb.png
	data = stbi_load("res/sun.jpg", &width, &height, &nrchannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//生成Mipmap纹理
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);//释放资源

	//月亮
	glGenTextures(1, &texture_buffer_object3);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_object3);

	//指定纹理的参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//加载纹理

	data = stbi_load("res/moon.jpg", &width, &height, &nrchannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//生成Mipmap纹理
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);//释放资源

	//设定点线面的属性
	glPointSize(15);//设置点的大小
	glLineWidth(5);//设置线宽

	//启动剔除操作
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//开启深度测试
	glEnable(GL_DEPTH_TEST);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case GLFW_KEY_2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case GLFW_KEY_3:
		glEnable(GL_CULL_FACE);    //打开背面剔除
		glCullFace(GL_BACK);          //剔除多边形的背面
		break;
	case GLFW_KEY_4:
		glDisable(GL_CULL_FACE);     //关闭背面剔除
		break;
	default:
		break;
	}
}

//鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//对摄像机进行操作
	camera.ProcessMouseScroll((float)yoffset);
}

// 鼠标移动回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// 将光标重新移动到窗口中央
	glfwSetCursorPos(window, (double)SCR_WIDTH / 2.0, (double)SCR_HEIGHT / 2.0);
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - (float)SCR_WIDTH / 2.0f;
	float yoffset = (float)SCR_HEIGHT / 2.0f - (float)ypos; // y坐标系进行反转

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset*1, yoffset*1);


}


void reshaper(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	if (height == 0)
	{
		aspact = (float)width;
	}
	else
	{
		aspact = (float)width / (float)height;
	}

}



void processJoystickInput(GLFWwindow* window, int joystick, float deltaTime)
{
	// 获取手柄的轴信息
	int axisCount;
	const float* axes = glfwGetJoystickAxes(joystick, &axisCount);

	// 获取手柄的按钮信息
	int buttonCount;
	const unsigned char* buttons = glfwGetJoystickButtons(joystick, &buttonCount);

	// 处理摄像机移动
	float xMove = axes[0];  // 左摇杆X轴
	float yMove = -axes[1]; // 左摇杆Y轴

	if (abs(xMove) < 0.1) xMove = (float)0.0000f;
	if (abs(yMove) < 0.1) yMove = (float)0.000000000f;

	// 处理摄像机旋转

	float xLook = axes[2];  // 右摇杆X轴
	float yLook = -axes[3]; // 右摇杆Y轴

	if (abs(xLook) < 0.1) xLook = (float)0.0000f;
	if (abs(yLook) < 0.1) yLook = (float)0.000000000f;


	// 处理摄像机的移动
	glm::vec3 cameraMovement(xMove, 0.0f, yMove);
	//cameraMovement = glm::normalize(cameraMovement);
	camera.ProcessKeyboard(Camera_Movement::FORWARD, cameraMovement.z * 0.01f);
	camera.ProcessKeyboard(Camera_Movement::RIGHT, cameraMovement.x * 0.01f);

	// 处理摄像机的旋转
	camera.ProcessMouseMovement(xLook * 500 * deltaTime, yLook * 1000 * deltaTime);


}


int main()
{

	glfwInit(); // 初始化GLFW

	// OpenGL版本为3.3，主次版本号均设为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

	// 使用核心模式(无需向后兼容性)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口(宽、高、窗口名称)
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sphere", NULL, NULL);

	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);

	if (!present) {
		std::cerr << "No joystick connected." << std::endl;
		// 处理没有连接手柄的情况
	}

	if (window == NULL)
	{
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}

	// 将窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);

	// 初始化GLAD，加载OpenGL函数指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	initial();//初始化
	glDisable(GL_CULL_FACE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//窗口大小改变时调用reshaper函数
	glfwSetFramebufferSizeCallback(window, reshaper);

	//窗口中有键盘操作时调用key_callback函数
	glfwSetKeyCallback(window, key_callback);

		//鼠标回调函数
	glfwSetCursorPosCallback(window, mouse_callback);
	//当鼠标滚轮滚动时调用函数scroll_callback
	glfwSetScrollCallback(window, scroll_callback);

	Shader lightingShader("sphere.vs", "sphere.fs");
	lightingShader.use();
	lightingShader.setInt("texture1", 0);
	float a = 6.0f, b = 3.0f;
	float r = 1.0f;

	
	while (!glfwWindowShouldClose(window))
	{
		//处理时间
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if(present) processJoystickInput(window, GLFW_JOYSTICK_1, deltaTime);


		// 输入处理
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);


		// 清空颜色缓冲和深度缓冲区
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 激活光照
		lightingShader.use();
		lightingShader.setVec3("light.position", glm::vec3(0.0f, 0.0f, 0.0f));
		lightingShader.setVec3("light.direction", camera.Front);
		lightingShader.setVec3("viewPos", camera.Position);

		//光源属性
		lightingShader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("light.specular", 0.05f, 0.05f, 0.05f);
		lightingShader.setFloat("light.constant", 1.0f);
		lightingShader.setFloat("light.linear", 0.045f);
		lightingShader.setFloat("light.quadratic", 0.0075f);

		//反光度
		lightingShader.setFloat("shininess", 2.0f);



		//地球
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object1);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		float currentTime = (float)glfwGetTime() / 30;

		glBindVertexArray(vertex_array_object);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 scale_earth = glm::mat4(1.0f);
		glm::mat4 trans_revolution_earth = glm::mat4(1.0f);
		glm::mat4 rot_tilt = glm::mat4(1.0f);
		glm::mat4 rot_self = glm::mat4(1.0f);
		scale_earth = glm::scale(scale_earth, glm::vec3(0.4, 0.4, 0.4));
		rot_tilt = glm::rotate(rot_tilt, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		trans_revolution_earth = glm::translate(trans_revolution_earth, glm::vec3(-a * std::cos(PI / 4 * currentTime), 0.0f, b * std::sin(PI / 4 * currentTime)));

		rot_self = glm::rotate(rot_self, -365 * currentTime * glm::radians(45.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
		model = trans_revolution_earth * rot_self * rot_tilt * scale_earth;

		lightingShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);

		//太阳
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object2);

		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::rotate(model, currentTime * glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		lightingShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);

		//月亮
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object3);

		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		glBindVertexArray(vertex_array_object);
		glm::mat4 scale_moon = glm::mat4(1.0f);
		glm::mat4 trans3 = glm::mat4(1.0f);
		glm::mat4 trans_revolution_moon = glm::mat4(1.0f);
		scale_moon = glm::scale(scale_moon, glm::vec3(0.1, 0.1, 0.1));
		trans3 = glm::translate(trans3, glm::vec3(a, 0.0f, 0.0f));
		trans_revolution_moon = glm::translate(trans_revolution_moon, glm::vec3(r * std::cos(PI * 7 * currentTime), 0.0f, r * std::sin(PI * 7 * currentTime)));
		rot_tilt = glm::mat4(1.0f);
		rot_tilt = glm::rotate(rot_tilt, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = trans_revolution_earth * rot_tilt * trans_revolution_moon * rot_self * scale_moon;

		lightingShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);





		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 解绑和删除VAO和VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
