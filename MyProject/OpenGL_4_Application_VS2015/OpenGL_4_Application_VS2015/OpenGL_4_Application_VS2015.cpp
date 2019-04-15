//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "SkyBox.hpp"
#include "Model3D.hpp"
#include "Mesh.hpp"
#include<time.h>
int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(glm::vec3(-5.0f, 0.0f, 0.5f), glm::vec3(10.0f, -5.0f, 0.0f));
GLfloat cameraSpeed = 0.05f;

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;
float directionx=0.0f, directiony=0.0f;

float manta_superioara;
float manta_inferioara=-0.22;
float manta_laterala_drepta;
float manta_laterala_stanga;
gps::Model3D myModel;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D pool_table;
gps::Model3D pool_stick;
gps::Model3D white_ball;
gps::Model3D ball_one;
gps::Model3D ball_two;
gps::Model3D ball_three;
gps::Model3D ball_four;
gps::Model3D ball_five;
gps::Model3D ball_six;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;
gps::Shader skyBoxShader;
gps::SkyBox skybox;
GLuint shadowMapFBO;
GLuint depthMapTexture;
//Camera
bool firstMouse;
float xLast= glWindowWidth/2.0f;
float yLast=glWindowHeight/2.0f;
float pitch = 0.0f;
float yaw = -90.0f;
std::vector<const GLchar*>faces;
GLfloat force;

glm::vec3 lightPositionPoint[] = {
	 glm::vec3(0.5f, 1.0f, -1.0f),
	 glm::vec3(1.5f,1.0f,1.0f)
};
glm::vec3 lightColorPoint[] = {
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f,0.0f,1.0f)
};
bool startGame = false;
GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	lightShader.useShaderProgram();
	
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state != GLFW_PRESS)
	{
		firstMouse = true;
		return;
	}
	if (firstMouse)
	{
		xLast = xpos;
		yLast = ypos;
		firstMouse = false;
	}
	float x_Offset = (xpos-xLast)*0.1;
	float y_Offset = (yLast-ypos)*0.1;
	xLast = xpos;
	yLast = ypos;
	yaw += x_Offset;
	pitch += y_Offset;
	myCamera.rotate(pitch, yaw);

}
void processMove();
void processMovement()
{
	if (pressedKeys[GLFW_KEY_G])
	{
		force += 0.01f;
	}
	if (pressedKeys[GLFW_KEY_F])
	{
		startGame = true;
		
	}
	

	if (pressedKeys[GLFW_KEY_N])
	{
		myCamera.recalculate_camera(glm::vec3(0.0f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
	}
	if (pressedKeys[GLFW_KEY_B])
	{
		myCamera.recalculate_camera(glm::vec3(-5.0f, 0.0f, 0.5f), glm::vec3(10.0f, -5.0f, 0.0f));
	}
	if (pressedKeys[GLFW_KEY_O])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (pressedKeys[GLFW_KEY_I])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	if (pressedKeys[GLFW_KEY_P])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 0.1f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 0.1f;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f; 
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}	
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
    //glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	myModel = gps::Model3D("objects/nanosuit/nanosuit.obj", "objects/nanosuit/");
	ground = gps::Model3D("objects/ground/ground.obj", "objects/ground/");
	lightCube = gps::Model3D("objects/cube/cube.obj", "objects/cube/");
	pool_table = gps::Model3D("objects/Pool Table/pool_table.obj", "objects/Pool Table/");
	pool_stick = gps::Model3D("objects/Pool Cue/10522_Pool_Cue_v1_L3_U.obj", "objects/Pool Cue/");
	white_ball = gps::Model3D("objects/Ball/ball_white.obj", "objects/Ball/");
	ball_one = gps::Model3D("objects/Ball/ball_number_1.obj", "objects/Ball/");
	ball_two = gps::Model3D("objects/Ball/ball_number_2.obj", "objects/Ball/");
	ball_three = gps::Model3D("objects/Ball/ball_number_3.obj", "objects/Ball/");
	ball_four = gps::Model3D("objects/Ball/ball_number_4.obj", "objects/Ball/");
	ball_five = gps::Model3D("objects/Ball/ball_number_5.obj", "objects/Ball/");
	ball_six = gps::Model3D("objects/Ball/ball_number_6.obj", "objects/Ball/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
	skyBoxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	
	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 2.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
}
void init_skybox() {
	faces.push_back("skybox/devils-tooth_rt.tga");
	faces.push_back("skybox/devils-tooth_lf.tga");
	faces.push_back("skybox/devils-tooth_up.tga");
	faces.push_back("skybox/devils-tooth_dn.tga");
	faces.push_back("skybox/devils-tooth_bk.tga");
	faces.push_back("skybox/devils-tooth_ft.tga");
	skybox.Load(faces);
}
void processMove(){
	if (startGame)
	{
		glm::vec3 direction = myCamera.getCameraDirection();
		if (directiony < manta_inferioara)
			direction = -direction;
		directionx += direction.x/10.0f;
		directiony += direction.z/10.0f;
		
		std::cout << direction.x << " " << directiony << std::endl;
		force -= 0.01f;
	}
	
	
	if (force <= 0.0f)
	{
		force = 0.0f;
		startGame = false;
	}
}
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processMovement();	
	processMove();
	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
		
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.08, 0.08, 0.08));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	pool_table.Draw(depthMapShader);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.7f, -1.4f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	pool_stick.Draw(depthMapShader);

	//Ball Shadows

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f+directionx, -1.83f, 0.0f+directiony));
	model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	white_ball.Draw(depthMapShader);


	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -1.83f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_one.Draw(depthMapShader);
	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(11.2f, -9.1f, -0.6f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_two.Draw(depthMapShader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(11.2f, -9.1f, 0.6f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_three.Draw(depthMapShader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(12.4f, -9.1f, -1.2f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_four.Draw(depthMapShader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(12.4f, -9.1f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_five.Draw(depthMapShader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(12.4f, -9.1f, 1.2f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_six.Draw(depthMapShader);
	//create model matrix for ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 
						1, 
						GL_FALSE, 
						glm::value_ptr(model));

	//ground.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the scene (second pass)

	myCustomShader.useShaderProgram();
	//Send point light to shader
	for (int i = 0; i < 2; i++)
	{
		std::string number = std::to_string(i);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, (("lumina["+number+"].position").c_str())), lightPositionPoint[i].x, lightPositionPoint[i].y, lightPositionPoint[i].z);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, (("lumina["+number+"].ambient").c_str())), lightColorPoint[i].x*0.1, lightColorPoint[i].y*0.1, lightColorPoint[i].z*0.1);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, (("lumina["+number+"].diffuse").c_str())), lightColorPoint[i].x, lightColorPoint[i].y, lightColorPoint[i].z);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, (("lumina["+number+"].specular").c_str())), lightColorPoint[i].x, lightColorPoint[i].y, lightColorPoint[i].z);
		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, (("lumina["+number+"].constant").c_str())), 1.0f);
		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, (("lumina["+number+"].linear").c_str())), 0.09f);
		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, (("lumina["+number+"].quadratic").c_str())), 0.032f);
	}
	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));	

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	
	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.08, 0.08, 0.08));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	pool_table.Draw(myCustomShader);
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.7f, -1.4f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	pool_stick.Draw(myCustomShader);

	//create model matrix for ground
	
	//Ball drawing
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f+directionx, -1.83f, 0.0f+directiony));
	model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	white_ball.Draw(myCustomShader);


	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -1.83f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_one.Draw(myCustomShader);
	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(11.2f, -9.1f, -0.6f));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_two.Draw(myCustomShader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(11.2f, -9.1f, 0.6f));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_three.Draw(myCustomShader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(12.4f, -9.1f, -1.2f));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_four.Draw(myCustomShader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(12.4f, -9.1f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_five.Draw(myCustomShader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	model = glm::translate(model, glm::vec3(12.4f, -9.1f, 1.2f));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ball_six.Draw(myCustomShader);
	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	//ground.Draw(myCustomShader);

	//draw a white cube around the light

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, lightDir);
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	lightCube.Draw(lightShader);

	skyBoxShader.useShaderProgram();
	
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	skybox.Draw(skyBoxShader, view, projection);


}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();	
	init_skybox();
	glCheckError();
	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
