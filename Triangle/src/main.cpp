#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int CompileShader(const unsigned int& type, const std::string& source) 
{
    unsigned int id = glCreateShader(type); // id for a shader, as mentioned before EVERYTHING in OpenGL has an id!
    const char* src = source.c_str(); // equal to doing &source[0], why we need this will be explained in a little bit
    
    glShaderSource(id, 1, &src, 0); 
    glCompileShader(id); 

// The code below is error checking! We want to make sure that the shader properly compiled since openGL SUCKS at error handling...
    
    int result; 
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // how this works is that we are checking the status of the shader
    // Note: essentially glGetShader is the root function and everything after it (iv in our case) is for a specific thing
    // In this case iv means: i = integer, v = vector (aka a pointer, hence why we need to send a GLint*)

    if (result == GL_FALSE) {
        int length; 
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); 

        char* error = (char*)alloca(length * sizeof(char)); // this is how to dynamically allocate a char* on the stack ;-;
    
        glGetShaderInfoLog(id, length, &length, error); // don't ask why we need a &length, idk either
    
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << '\n';
        std::cout << error << std::endl; 

        glDeleteShader(id); 
        return 0; 
    }

    return id; // returns the shaders id!
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
    // GLuint == unsigned int
    unsigned int program = glCreateProgram();  
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); 
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // the naming convention for these functions are actually self explanitory, FINALLY
    glAttachShader(program, vs); 
    glAttachShader(program, fs);

    glLinkProgram(program); 
    glValidateProgram(program);

    // we can delete the shaders since they were sucessfully copied onto program!
    glDeleteShader(vs); 
    glDeleteShader(fs);

    return program; 
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error\n";
    }

    // Vertex Buffer

    // Note: wording matter since vertexes aren't exclusive to positions and aren't exclusive to only one thing (attribute) per initilization 
    // Also Note: These are seperate vertexes, more details later
    float positions[6] = {
       -0.5f, -0.5f, // Vertex 1
        0.0f,  0.5f, // Vertex 2
        0.5f, -0.5f // Vertex 3
    };
    unsigned int buffer; 

    glGenBuffers(1, &buffer); // this is a Vertex Buffer, this is just to store data in vram 
    glBindBuffer(GL_ARRAY_BUFFER, buffer); 
    /*
    * THIS IS THE MOST IMPORTANT LINE, how opengl works is very similar to photoshop!
    * You have to select which layer you want to edit, OpenGL works the same way!
    * We need to Bind the Buffer we want to edit and manipulate to OpenGL, so it knows
    which buffer we are refering too (Note: everything has a special Address in OpenGL for OpenGL)
    * 
    */

                // type of data                      // data      // usage
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW); 
                                 // data size in bytes                            
    /*
    * This is the data that we are copying onto the buffer.
    * Check documentation for more details, but I'll go over the last paramater: GLenum usage.
    * This hints (it can be ignored) to the compiler on the use case for this buffer data.
    */
    
    glEnableVertexAttribArray(0); // we need to run this in order to ensure that the Attrib is enabled, and it takes in the GLuint index, and because it is OpenGL we can place it anywhere after binding

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); 
    /*
    * glVertexAttribPointer is the layout odf our data, it tell openGL how to proper read the data from the buffer
    * 
    ** Parameter 1: GLuint index - This is the attribute number of the attribute within the buffer. It is the position within the buffer, essentially an index. 
    * 
    ** Parameter 2: GLint size - this tells OpenGL how many components is there for each vertex (ranges from 1-4)
    * 
    ** Parameter 3: GLboolean normalized - Since OpenGL has a system where things can only raneg from 0-1, if you need to reprsent data (like colors which uses 0-255) 
       that doesnt fit in the 0-1 range. Then this parameter will tell OpenGL to automatically convert the data! (For oru case we don't need it since we are usng floats)
    *
    ** Parameter 4: GLsizei stride - This is simply the byte amount of the components amounts for the attribute we are looking for (In this case, all the attributes are 8 bytes/2 floats)
        This tells OpenGL whether or not it is actually accessing the correct data we called for!
    * 
    ** Parameter 5: const GLvoid * pointer - This is essentially the parameter for where we start in byte amount. n this case we want to start at vertex 1, so we start at byte 0 of the Buffer!
       But it is a pointer how can I pass 0? Don't forget 0 == nullptr, all other values however WILL need you to explicitly cast it as a const void*!
    */


    // These are shaders, now you know why they are defined as programs run on the gpu 
    // WE are Literally making a mini program for each shader

    std::string vertexShader = 
        "#version 330 core\n" // represents the processing version we want openGL to use, we don't need something fancy so 330 is just fine
        "\n" 
        "layout(location = 0) in vec4 position;" // we are
        "\n"
        "void main()" 
        "{\n" 
            "gl_Position = position;\n" 
        "}\n"; 

    std::string fragmentShader = 
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;"  
        "\n"
        "void main()"
        "{\n"
        "   color = vec4(1.0, 0.0, 1.0, 0.5);\n" // uses a RGBA coloring format (from ranges 0.0 - 1.0 instead of 0-255)
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader); // this binds the shader

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3); // Now this will draw the Triangle onto the screen

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader); // removed the program from memory

    glfwTerminate();
    return 0;
}