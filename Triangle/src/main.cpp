#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

static std::string parseFile(const std::string& filepath)
{
    std::ifstream stream = std::ifstream(filepath); 

    if (!stream) {
        std::cerr << "File didn't open.\n"; 
        std::cout << filepath << std::endl; 
        return std::string();
    }

    enum class ShaderType
    {
        NONE = -1, 
        VERTEX = 0, 
        FRAGMENT = 1
    };

    std::string line; 
    std::stringstream ss; 
    
    ShaderType type = ShaderType::NONE; 

    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex")) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment")) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss << line << '\n'; 
        }
    }

    return ss.str();
}

struct ShaderSource {
    std::string vertexShader;
    std::string fragmentShader;
};

static ShaderSource getShaders(
    std::string&& vertexPath, 
    std::string&& fragmentPath
) {
    return
    {
        parseFile(vertexPath),
        parseFile(fragmentPath)
    };
}

static unsigned int CompileShader(const unsigned int& type, const std::string& source) 
{
    unsigned int id = glCreateShader(type); 
    const char* src = source.c_str(); 
    
    glShaderSource(id, 1, &src, 0); 
    glCompileShader(id); 
   
    int result; 
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); 

    if (result == GL_FALSE) {
        int length; 
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); 

        char* error = (char*)_alloca(length * sizeof(char)); 
    
        glGetShaderInfoLog(id, length, &length, error); 
    
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << '\n';
        std::cout << error << std::endl; 

        glDeleteShader(id); 
        return 0; 
    }

    return id; 
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
    unsigned int program = glCreateProgram();  
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); 
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs); 
    glAttachShader(program, fs);

    glLinkProgram(program); 
    glValidateProgram(program);

    glDeleteShader(vs); 
    glDeleteShader(fs);

    return program; 
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error\n";
    }
    
    float positions[6] = {
       -0.5f, -0.5f, 
        0.0f,  0.5f, 
        0.5f, -0.5f 
    };
    unsigned int buffer; 

    glGenBuffers(1, &buffer); 
    glBindBuffer(GL_ARRAY_BUFFER, buffer); 
   
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW); 
                                                             
   
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); 
  

    ShaderSource Shaders = getShaders(
        "res/shaders/vertex.shader", 
        "res/shaders/fragment.shader"
    );

    std::cout << "Vertex" << std::endl;
    std::cout << Shaders.vertexShader << std::endl;
    std::cout << "Fragment" << std::endl; 
    std::cout << Shaders.fragmentShader << std::endl; 

    unsigned int shader = CreateShader(
        Shaders.vertexShader, 
        Shaders.fragmentShader
    );

    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3); 

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteProgram(shader); 

    glfwTerminate();
    
    return 0; 
}