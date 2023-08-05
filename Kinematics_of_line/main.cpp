#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>


const float PI = 3.14159265358979323;


class line {

	public:
		
		float length;
		float alpha;
		GLfloat vertices[4];
		GLfloat colors[6];

		float coordinatesX;
		float coordinatesY;

		float velocityX;
		float velocityY;

		float accelerationX;
		float accelerationY;

		float angularVelocityZ;
		float angularAccelerationZ;

		float deltaTime;
		float frameTime;

		GLuint VAO;


		void updateLine() {

			GLfloat totalTime = glfwGetTime();
			deltaTime = totalTime - frameTime;
			frameTime = totalTime;

			coordinatesX += (velocityX * deltaTime) * (0.5 * accelerationX * deltaTime * deltaTime);
			coordinatesY += (velocityY * deltaTime) * (0.5 * accelerationY * deltaTime * deltaTime);

			velocityX += accelerationX * deltaTime;
			velocityY += accelerationY * deltaTime;

			alpha += (angularVelocityZ * deltaTime) + (0.5 * angularAccelerationZ * deltaTime * deltaTime);

			angularVelocityZ += angularAccelerationZ * deltaTime;

		}


		void renderObject() {

			glGenVertexArrays(1, &VAO);
    	    glBindVertexArray(VAO);

	        GLuint vertexVBO;
        	glGenBuffers(1, &vertexVBO);
        	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	        glEnableVertexAttribArray(0);

        	GLuint colorVBO;
        	glGenBuffers(1, &colorVBO);
        	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
       		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
     	   	glEnableVertexAttribArray(1);

        	glBindVertexArray(0);
    	
		}


		void cleanup() {

			glDeleteVertexArrays(1, &VAO);
    
		}

};


const GLchar* vertexShaderSource = R"(

	#version 450 core

	layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 color;

	out vec3 vertexColor;

	void main() {

		vertexColor = color;
		gl_Position = vec4(position.x, position.y, 0.0, 1.0);

	}
	
)";

const GLchar* fragmentShaderSource = R"(

	#version 450 core

	in vec3 vertexColor;
	out vec4 fragColor;

	void main() {
	
		fragColor = vec4(vertexColor, 1.0f);

	}

)";


void init(std::vector<line>& objects);

void display(std::vector<line>& objects, GLuint& shaderProgram, GLFWwindow* window);

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void renderLines(std::vector<line>& objects);

void cleanUp(std::vector<line>& objects); 

void update(std::vector<line>& objects); 

void renderObject1(std::vector<line>& objects, int i);

void renderObject1vertices(std::vector<line>& objects, int i);

void renderObject1colors(std::vector<line>& objects, int i);

void renderObject2(std::vector<line>& objects, int i);

void renderObject2vertices(std::vector<line>& objects, int i);

void renderObject2colors(std::vector<line>& objects, int i);


int main() {

	std::vector<line> objects;

	std::vector<void (*)()> renderObjectVertices;

	GLuint shaderProgram;


    if (!glfwInit()) {

        std::cout << "GLFW initialization failed!" << std::endl;
        return -1;

	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(3840, 2160, "Dynamic Window", nullptr, nullptr);

    if (!window) {

        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;

    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {

        std::cout << "GLEW initialization failed!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;

	}

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    init(objects);

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
		update(objects);
		display(objects, shaderProgram, window);
	
	}

    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;

}


void init(std::vector<line>& objects) {

	int numberofobjects = 1;
		
	int i;

	renderObject1(objects, i = 0);
	renderObject2(objects, i = 1);

}


void display(std::vector<line>& objects, GLuint& shaderProgram, GLFWwindow* window) {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);

	renderLines(objects);

	glfwSwapBuffers(window);

	cleanUp(objects);

}

void renderObject1(std::vector<line>& objects, int i) {

	objects.push_back(line());

	objects[i].length = 0.5;
	objects[i].alpha = 0.0;

	objects[i].coordinatesX = 0.0;
	objects[i].coordinatesY = 0.0;
	objects[i].velocityX = 0.0;
	objects[i].velocityY = 0.0;

	objects[i].accelerationX = 0.0;
	objects[i].accelerationY = 0.0;

	objects[i].angularVelocityZ = 0.0;

	objects[i].angularAccelerationZ = 1.0;

	objects[i].deltaTime = 0.0;
	objects[i].frameTime = 0.0;

	renderObject1vertices(objects, i);
	renderObject1colors(objects, i);

}

void renderObject1vertices(std::vector<line>& objects, int i) {
	
	objects[i].vertices[0] = objects[i].coordinatesX;
	objects[i].vertices[1] = objects[i].coordinatesY;
	objects[i].vertices[2] = objects[i].coordinatesX + (objects[i].length * cos(objects[i].alpha));
	objects[i].vertices[3] = objects[i].coordinatesY + (objects[i].length * sin(objects[i].alpha));

}

void renderObject1colors(std::vector<line>& objects, int i) {

	objects[i].colors[0] = 0.0;
	objects[i].colors[1] = 0.0;
	objects[i].colors[2] = 1.0;
	objects[i].colors[3] = 0.0;
	objects[i].colors[4] = 0.0;
	objects[i].colors[5] = 1.0; 

}


void renderObject2(std::vector<line>& objects, int i) {

	objects.push_back(line());

	objects[i].length = 0.5;
	objects[i].alpha = 0.0;

	objects[i].coordinatesX = objects[i - 1].vertices[0] + (objects[i].length * cos(objects[i].alpha));
	objects[i].coordinatesY = objects[i - 1].vertices[1] + (objects[i].length * sin(objects[i].alpha));
	objects[i].velocityX = 0.0;
	objects[i].velocityY = 0.0;

	objects[i].accelerationX = 0.0;
	objects[i].accelerationY = 0.0;

	objects[i].angularVelocityZ = 0;

	objects[i].angularAccelerationZ = 2.0;

	objects[i].deltaTime = 0.0;
	objects[i].frameTime = 0.0;

	renderObject1vertices(objects, i);
	renderObject1colors(objects, i);

}

void renderObject2vertices(std::vector<line>& objects, int i) {
	
	objects[i].vertices[0] = objects[i - 1].vertices[2];
	objects[i].vertices[1] = objects[i - 1].vertices[3];
	objects[i].vertices[2] = objects[i].vertices[0] + (objects[i].length * cos(objects[i].alpha));
	objects[i].vertices[3] = objects[i].vertices[1] + (objects[i].length * sin(objects[i].alpha));

}

void renderObject2colors(std::vector<line>& objects, int i) {

	objects[i].colors[0] = 1.0;
	objects[i].colors[1] = 0.0;
	objects[i].colors[2] = 0.0;
	objects[i].colors[3] = 1.0;
	objects[i].colors[4] = 0.0;
	objects[i].colors[5] = 0.0; 

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);

}

void renderLines(std::vector<line>& objects) {

	for (int i = 0; i < objects.size(); i++) {

		objects[i].renderObject();
		glBindVertexArray(objects[i].VAO);
		glDrawArrays(GL_LINES, 0, 4);
		glBindVertexArray(0);

	}

}


void update(std::vector<line>& objects) {

	for (int i = 0; i < objects.size(); i++) {

		objects[i].updateLine();

	}

	renderObject1vertices(objects, 0);
	renderObject2vertices(objects, 1);

}


void cleanUp(std::vector<line>& objects) {

	for (int i = 0; i < objects.size(); i++) {

		objects[i].cleanup();

	}

}
