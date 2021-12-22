#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "head/shader.hpp"
#include "head/texture.hpp"
#include "head/controls.hpp"
#include "head/objloader.hpp"

#define LOG

GLFWwindow* window;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);


void render(GLuint* programID, std::vector<glm::vec3> vertices, GLuint* MatrixID, GLuint* ModelMatrixID, GLuint* ViewMatrixID, GLuint* LightID, GLuint* vertexbuffer, GLuint *normalbuffer, GLuint* texturebuffer, GLuint* Texture, GLuint* TextureID,  glm::mat4 MVP, glm::mat4 ModelMatrix, glm::mat4 ViewMatrix);


    
static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };


int main()
{
    glewExperimental = true;
    if( !glfwInit() )
    {
        fprintf( stderr, "Error: Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x anti aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Make macos happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //NO old opengl


    
    window = glfwCreateWindow(1024, 768, "Test", NULL, NULL);

    if( window == NULL)
    {
        fprintf( stderr, "Error: Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window); //Initialize glew
    glewExperimental = true;
    
    if(glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Error: Failed to initialize GLEW\n");
        return -1;
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    glClearColor(0.0f, 1.0f, 1.0f, 0.9f);
    //enable depth test
    glEnable(GL_DEPTH_TEST);
    //accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);
    //enable backface culing
    glEnable(GL_CULL_FACE);


    //Generate Vao
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders( "Shaders/ShadingVertexShader.vs", "Shaders/ShadingFragmentShader.fs" );


    // Get a handle for  "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");


    GLuint Texture = loadDDS("imgs/uvmap.DDS"); //loadDDS("imgs/uvmap.DDS");

    //read .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool success = loadOBJ("imgs/suzanne.obj", vertices, uvs, normals);

    if(!success)
    {
        fprintf(stderr, "Error: Failed to load mesh.\n");
        return -1;
    }
    // Get a handle for  "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);


    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    GLuint colorbuffer;  
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    
    
    do
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
	
	
	    // Camera matrix
	    glm::mat4 ViewMatrix  = getViewMatrix();
	    glm::mat4 ModelMatrix      = glm::mat4(1.0f);
	    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
   

        render(&programID, vertices, &MatrixID, &ModelMatrixID, &ViewMatrixID, &LightID, &vertexbuffer, &normalbuffer, &uvbuffer, &Texture, &TextureID, MVP, ModelMatrix, ViewMatrix);

        //Update Display
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    //delete and close everything
    glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

    //terminate glfw and close the window
    glfwTerminate();

    return 0;
}


void render(GLuint* programID, std::vector<glm::vec3> vertices, GLuint* MatrixID, GLuint* ModelMatrixID, GLuint* ViewMatrixID, GLuint* LightID, GLuint* vertexbuffer, GLuint *normalbuffer, GLuint* texturebuffer, GLuint *Texture, GLuint* TextureID,  glm::mat4 MVP, glm::mat4 ModelMatrix, glm::mat4 ViewMatrix)
{   
        //Use the shaders
        glUseProgram(*programID);
        //send MVP matrix to currently bound shader
        glUniformMatrix4fv(*MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(*ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(*ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        glm::vec3 lightPos = glm::vec3(4, 4, 4);
        glUniform3f(*LightID, lightPos.x, lightPos.y, lightPos.y);

        // Bind texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *Texture);
		//Set "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(*TextureID, 0);


        //vertexBuffer
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, *vertexbuffer);
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );
       

        //2nd attribute buffer: textures 
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, *texturebuffer);
        glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size only 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
        );


        //3rd attribute buffer: normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, *normalbuffer);
        glVertexAttribPointer(
            2,        //attribute
            3,        //size
            GL_FLOAT, //type
            GL_FALSE, //normalized
            0,        //stride
            (void*) 0 //offset
        );
        


        //draw
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()); 

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
}