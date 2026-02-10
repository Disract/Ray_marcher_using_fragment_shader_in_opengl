
#include <glad/glad.h>
#include<GLFW/glfw3.h>
	
#include "./depdencies/include/custom/shader.h"
#include "./depdencies/include/custom/ComputeShader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<math.h>
#include<iostream>


//MAGIC CONSTANTS
const unsigned int magic = 700;
const unsigned int SCR_WIDTH = magic;
const unsigned int SCR_HEIGHT = magic;
const unsigned int TEXTURE_WIDTH = magic;
const unsigned int TEXTURE_HEIGHT = magic;



//MANAGE WINDOW RESIZE
void window_resize_callback(GLFWwindow* window_p, int width, int height)
{
	glViewport(0,0,width,height);
}

//RENDER A PLAN IN WINDOW
unsigned int quadVBO;
unsigned int quadVAO = 0;
void renderQuad()
{
	
	if (quadVAO == 0)
	{
		float vertices[] =
		{					  
			-1.0f,1.0f,0.0f,  
			-1.0f,-1.0f,0.0f,
			 1.0f,1.0f,0.0f,  
			 1.0f,-1.0f,0.0f
		};
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
unsigned int quadVBO1;
unsigned int quadVAO1 = 0;
void renderFinalQuad(unsigned int tex_color_buffer)
{

	if (quadVAO1 == 0)
	{
		float vertices[] =
		{
			-1.0f,1.0f,		0.0f,1.0f,
			-1.0f,-1.0f,	0.0f,0.0f,
			 1.0f,-1.0f,	1.0f,0.0f,
			 
			 -1.0f,1.0f,    0.0f,1.0f,
			 1.0f,-1.0f,    1.0f,0.0f,
			 1.0f,1.0f,		1.0f,1.0f,

		};
		glGenVertexArrays(1, &quadVAO1);
		glGenBuffers(1, &quadVBO1);
		glBindVertexArray(quadVAO1);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	}
	glBindVertexArray(quadVAO1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
int main()
{
	using namespace std;

	//INITIALISING OPENGL WITH 4.3
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//WINDOW CREATION
	GLFWwindow* window_p = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Compute", NULL, NULL);
	if (window_p == NULL)
	{
		cout << "ERROR::WINDOW FAILED" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window_p);
	glfwSetFramebufferSizeCallback(window_p, window_resize_callback);
	//VERTICAL SYNCHRONIZATION TIMEOUT 
	//glfwSwapInterval(0);

	//LOADING ALL OPENGL POINTERS
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "ERROR::FUNCTION POINTERS WEREN'T LOADED" << std::endl;
		return -1;
	}

	//SHADERSS
	Shader FullQuad("./shaders/FullQuad.vs", "./shaders/FullQuad1.ffs");
	Shader FrameBuffer("./shaders/Framebuffer.vs", "./shaders/Framebuffer.ffs");

	
	FrameBuffer.use();
	FrameBuffer.setInt("screenTexture", 0);
	
	//FRAM BUFFER OBJECT 
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int tex_color_buffer;
	glGenTextures(1,&tex_color_buffer);
	glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color_buffer, 0);
	
	
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo);


	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER::" << fboStatus << std::endl;
	

	

	while (!glfwWindowShouldClose(window_p))
	{
		glBindFramebuffer(GL_FRAMEBUFFER,fbo);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		FullQuad.use();
		glm::mat4 rotation = glm::mat4(1.0);
		rotation = glm::rotate(rotation, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0));
		glUniformMatrix4fv(glGetUniformLocation(FullQuad.ID, "rotate"),1,GL_TRUE,glm::value_ptr(rotation));
		FullQuad.setFloat("t", sin(glfwGetTime()) * 2);
		FullQuad.setFloat("elapsed_time", glfwGetTime());
		FullQuad.setInt("tex", 1);
		renderQuad();
		
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		FrameBuffer.use();
		renderFinalQuad(tex_color_buffer);
		glfwSwapBuffers(window_p);
		glfwPollEvents();

	}
}
