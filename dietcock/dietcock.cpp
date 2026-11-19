#include<stdio.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "glm/ext/matrix_transform.hpp"
#include"shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include"math.h"


void framebuffer_call_size(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

bool cursor_locked = true;
bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
double last_toggle_time = 0.0;
const double toggle_cooldown = 1.5;

glm::vec3 camera_pos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

int main(void) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(800, 600, "hi", NULL, NULL);
	if (window == NULL) {
		printf("failed to initialize window\n");
		glfwTerminate();
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_call_size);
	glfwSetScrollCallback(window, scroll_callback);
        glfwSetCursorPosCallback(window, mouse_callback);  

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("failed to initialize glad\n");
		glfwTerminate();
		return -1;
	}	

	Shader shader;
	shader = shader_create("./shader.vs", "./fragment.fs");

	float vertices[] = {
	  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	/*	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
		};*/
	
	glm::vec3 cubePositions[] = {
	  glm::vec3( 0.0f,  0.0f,  0.0f), 
	  glm::vec3( 2.0f,  5.0f, -15.0f), 
	  glm::vec3(-1.5f, -2.2f, -2.5f),  
	  glm::vec3(-3.8f, -2.0f, -12.3f),  
	  glm::vec3( 2.4f, -0.4f, -3.5f),  
	  glm::vec3(-1.7f,  3.0f, -7.5f),  
	  glm::vec3( 1.3f, -2.0f, -2.5f),  
	  glm::vec3( 1.5f,  2.0f, -2.5f), 
	  glm::vec3( 1.5f,  0.2f, -1.5f), 
	  glm::vec3(-1.3f,  1.0f, -1.5f)  
	};

	unsigned int VAO, VBO;//, EBO;
	// generate the Vertex Buffer Object, Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(6 * sizeof(float)));
	//	glEnableVertexAttribArray(2);
	
	//texture implementation
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load and generate the texture
	int width, height, nr_channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nr_channels, 0);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (data) {
		GLenum format = (nr_channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("fucked up the container texture dumb ass\n");
	}
	stbi_image_free(data);
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("awesomeface.png", &width, &height, &nr_channels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
        } else {
	  printf("failed to load awesomeface\n");
        }
	stbi_image_free(data);
	shader_use(&shader);
	shader_set_int(&shader, "texture1", 0);
        shader_set_int(&shader, "texture2", 1);
        printf("tex1 loc=%d  tex2=loc%d\n",
               glGetUniformLocation(shader.ID, "texture1"),
               glGetUniformLocation(shader.ID, "texture2"));
        glfwSwapInterval(1);

        // MATH
	unsigned int model_loc = glGetUniformLocation(shader.ID, "model");
        unsigned int projection_loc = glGetUniformLocation(shader.ID, "projection");
        unsigned int view_loc = glGetUniformLocation(shader.ID, "view");

        if (model_loc == -1) printf("WARNING: uniform 'model' not found\n");
	if (view_loc  == -1) printf("WARNING: uniform 'view' not found\n");
        if (projection_loc == -1)
          printf("WARNING: uniform 'projection' not found\n");

        while (!glfwWindowShouldClose(window)) {

                float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		process_input(window);
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture1);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texture2);
		
		shader_use(&shader);
		// model matrix
		mat4 model = mat4(1.0f);


		//view matrix
	        const float radius = 10.0f;
		float cam_x = sin(glfwGetTime()) * radius;
		float cam_z = cos(glfwGetTime()) * radius;
		mat4 view = mat4(1.0f);
		view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
				
                //view = glm::translate(view,vec3(0.0f,0.0f,-3.0f));              
                              
                // projection matrix
                int fbw, fbh;
                glfwGetFramebufferSize(window, &fbw, &fbh);
                
                mat4 projection = glm::perspective(glm::radians(fov),
                                             (float)fbw / (float)fbh, 0.1f,
                                             100.0f);
	
                // camera
                vec3 camera_target = vec3(0.0f, 0.0f, 0.0f);
                vec3 camera_direction =
                    glm::normalize(camera_pos - camera_target);

                vec3 up = vec3(0.0f, 1.0f, 0.0f);
                vec3 camera_right =
                    glm::normalize(glm::cross(up, camera_direction));
		vec3 camera_up = glm::cross(camera_direction, camera_right);

		// send matrices to shader
	
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
           
                glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
                
                glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

                glBindVertexArray(VAO);
                for(unsigned int i = 0; i < 10; i++)
		  {
		    glm::mat4 model = glm::mat4(1.0f);
		    model = glm::translate(model, cubePositions[i]);
		    float angle = 20.0f * i;
		    
                    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(90.0f), vec3(0.5f, 1.0f, 0.0f));
		    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
		    
		    glDrawArrays(GL_TRIANGLES, 0, 36);
		  }
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
                
		glfwSwapBuffers(window);
		glfwPollEvents();
        }
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shader_destroy(&shader);
	glfwTerminate();
	return 0;
}
void framebuffer_call_size(GLFWwindow* window, int width, int height){
	(void)window;
	glViewport(0, 0, width, height);
}
void process_input(GLFWwindow *window) {
  const float camera_speed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
          glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
          camera_pos += camera_speed * camera_front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
          camera_pos -= camera_speed * camera_front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
          camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) *
                        camera_speed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
          camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) *
                        camera_speed;
		/*if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
			double current_time = glfwGetTime();

			if (current_time - last_toggle_time >= toggle_cooldown) {
				last_toggle_time = current_time;
				cursor_locked = !cursor_locked;
			}
		}
		 if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) 
        cursor_locked = !cursor_locked; // Flip the state

        if (cursor_locked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            //firstMouse = true; // Reset so the camera doesn't jump on re-lock
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    */
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
	/*if (!cursor_locked) {
		return;
	}*/
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float senstivity = 0.1f;
  xoffset *= senstivity;
  yoffset *= senstivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  camera_front = glm::normalize(front);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  fov -= (float)yoffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 45.0f)
    fov = 45.0f;
}
