#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/matrix_decompose.hpp>

#include <shader.h>
#include <camera.h>

#include <iostream>
#include <math.h> 

const char * fsShader = "C:\\Users\\Andre\\source\\repos\\OpenGl\\OpenGLv2\\src\\basic_lighting.fs";
const char * vsShader = "C:\\Users\\Andre\\source\\repos\\OpenGl\\OpenGLv2\\src\\basic_lighting.vs";
const char * fsLightShader = "C:\\Users\\Andre\\source\\repos\\OpenGl\\OpenGLv2\\src\\light_cube.fs";
const char * vsLightShader = "C:\\Users\\Andre\\source\\repos\\OpenGl\\OpenGLv2\\src\\light_cube.vs";


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
//spring
glm::mat4 lookat = glm::mat4(1.0f);
glm::vec3 springPosition = glm::vec3(0.0f, -5.f, 0.0f);
glm::vec3 velocity = glm::vec3(0.0f);
glm::vec3 gravity, springDirection, springRotation;
float mass = 30.0f;
float timeStep = .28f;
float k = 3.0f;
float damping = 10.0f;
float PI = 3.14159265;

void spring(glm::vec3 anchor)
{

    gravity = glm::normalize(anchor-springDirection) + glm::vec3(0, -.2f, 0);

    glm::vec3 massSpringForce = -k * (springPosition - anchor);

    // Mass 1 damping
    glm::vec3 massDampingForce = damping * velocity;

    // Mass 1 net force
    glm::vec3 massForce = massSpringForce + mass * gravity - massDampingForce;

    // Mass 1 acceleration
    glm::vec3 massAcceleration = massForce / mass;

    // Mass 1 velocity
    velocity = velocity + massAcceleration * timeStep;

    // Mass 1 position
    springPosition = springPosition + velocity * timeStep;

    //calculate rotation
    glm::vec3 vector = anchor-springPosition;

    springRotation[0] = atan2(sqrt(pow(vector[1], 2) + pow(vector[2], 2)), vector[0]) * 180.f / PI - 100;
    springRotation[1] = atan2(sqrt(pow(vector[2], 2) + pow(vector[0], 2)), vector[1]) * 180.f / PI - 100;
    springRotation[2] = atan2(sqrt(pow(vector[0], 2) + pow(vector[1], 2)), vector[2]) * 180.f / PI - 100;
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, -8.0f, 30.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

float active = 1;


int main()
{

    std::cout << "use mouse scroll to change auto hand movement on and off" << std::endl;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shader
    // ------------------------------------
    Shader lightingShader(vsShader, fsShader);
    Shader lightCubeShader(vsLightShader, fsLightShader);

    // set up vertex data and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float ani = 0; 
    glm::mat4 cube;
    float angle, spread, rotx, roty, rotz;
    glm::mat4 transform;
    glm::mat4 mElbow;
    glm::mat4 mShoulder;
    glm::mat4 mWrist;
    glm::mat4 mfinger11;
    glm::mat4 mfinger12;
    glm::mat4 mfinger13;

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        //transformation
        mShoulder = glm::mat4(1.0f);
        angle =  20*sin(ani);
        ani = ani + 0.05f;
        mShoulder = glm::translate(mShoulder, glm::vec3(-2.5f, 8.0f, 0));
        mShoulder = glm::rotate(mShoulder, glm::radians(angle), glm::vec3(0, 0, 1));
        mShoulder = glm::translate(mShoulder, glm::vec3(0, -4.f, 0));
        
        lightingShader.setMat4("model", glm::scale(mShoulder, glm::vec3(2.f, 7.5f, 2.f)));

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render elbow
        angle = 20 * sin(ani)+20;
        mElbow = glm::mat4(1.0f);
        mElbow = glm::translate(mElbow, glm::vec3(0, -4.f, 0));
        mElbow = glm::rotate(mElbow, glm::radians(angle), glm::vec3(0, 0, 1));
        mElbow = glm::translate(mElbow, glm::vec3(0, -4.f, 0));
        mElbow = mShoulder * mElbow;
        lightingShader.setMat4("model", glm::scale(mElbow, glm::vec3(2.f, 7.5f, 2.f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render wrist
        mWrist = glm::mat4(1.0f);
        mWrist = glm::translate(mWrist, glm::vec3(0, -4.f, 0));
        //mWrist = glm::rotate(mWrist, 90.f, glm::vec3(0, 1, 0));
        mWrist = mElbow * mWrist;
        glm::decompose(mElbow, scale, rotation, translation, skew, perspective);
        springDirection = translation;
        glm::decompose(mWrist, scale, rotation, translation, skew, perspective);
        spring(translation);
        mWrist = glm::rotate(mWrist, glm::radians(springRotation[0])*active, glm::vec3(0, 0, 1));

        mWrist = glm::translate(mWrist, glm::vec3(0, -1.5f, 0));
        glm::decompose(mWrist, scale, rotation, translation, skew, perspective);

        rotx = 0.f;
        roty = springRotation[1];
        rotz = springRotation[0];
        lightingShader.setMat4("model", glm::scale(mWrist, glm::vec3(2.f, 2.f, 1.f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /*
        //draw pendelum mass
        cube = glm::mat4(1.0f);
        spring(translation);
        cube = glm::translate(cube, springPosition);
        lightingShader.setMat4("model", cube);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        */

        // render finger index
        angle = (((rotx* active +90)/180*90 - (1-(rotx + 90))/180*0)*.5);
        spread = ((rotz*active - 45) / 90 * 40);
        mfinger11 = glm::mat4(1.0f);
        mfinger11 = glm::translate(mfinger11, glm::vec3(.75, -1.75f, 0));
        mfinger11 = glm::rotate(mfinger11, glm::radians(angle), glm::vec3(1, 0, 0));
        //mfinger11 = glm::rotate(mfinger11, glm::radians(spread), glm::vec3(0, 0, 1));
        mfinger11 = glm::translate(mfinger11, glm::vec3(0, -.075f, 0));
        mfinger11 = mWrist * mfinger11;
        lightingShader.setMat4("model", glm::scale(mfinger11, glm::vec3(.4f, 1.f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        mfinger12 = glm::mat4(1.0f);
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -0.5f, 0));
        mfinger12 = glm::rotate(mfinger12, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -.5f, 0));
        mfinger12 = mfinger11* mfinger12;
        lightingShader.setMat4("model", glm::scale(mfinger12, glm::vec3(.4f, .75f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        mfinger13 = glm::mat4(1.0f);
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -0.4f, 0));
        mfinger13 = glm::rotate(mfinger13, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -.3f, 0));
        mfinger13 = mfinger12 * mfinger13;
        lightingShader.setMat4("model", glm::scale(mfinger13, glm::vec3(.4f, .5f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render finger middle
        mfinger11 = glm::mat4(1.0f);
        mfinger11 = glm::translate(mfinger11, glm::vec3(.25f, -1.25f, 0));
        mfinger11 = glm::rotate(mfinger11, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger11 = glm::rotate(mfinger11, glm::radians(spread*.25f), glm::vec3(0, 0, 1));
        mfinger11 = glm::translate(mfinger11, glm::vec3(0, -.75f, 0));
        mfinger11 = mWrist * mfinger11;
        lightingShader.setMat4("model", glm::scale(mfinger11, glm::vec3(.4f, 1.f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        mfinger12 = glm::mat4(1.0f);
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -0.5f, 0));
        mfinger12 = glm::rotate(mfinger12, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -.5f, 0));
        mfinger12 = mfinger11 * mfinger12;
        lightingShader.setMat4("model", glm::scale(mfinger12, glm::vec3(.4f, .75f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        mfinger13 = glm::mat4(1.0f);
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -0.4f, 0));
        mfinger13 = glm::rotate(mfinger13, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -.3f, 0));
        mfinger13 = mfinger12 * mfinger13;
        lightingShader.setMat4("model", glm::scale(mfinger13, glm::vec3(.4f, .5f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render finger ring
        mfinger11 = glm::mat4(1.0f);
        mfinger11 = glm::translate(mfinger11, glm::vec3(-.25f, -1.25f, 0));
        mfinger11 = glm::rotate(mfinger11, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger11 = glm::rotate(mfinger11, glm::radians(spread*.5f), glm::vec3(0, 0, 1));
        mfinger11 = glm::translate(mfinger11, glm::vec3(0, -.75f, 0));
        mfinger11 = mWrist * mfinger11;
        lightingShader.setMat4("model", glm::scale(mfinger11, glm::vec3(.4f, 1.f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        mfinger12 = glm::mat4(1.0f);
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -0.5f, 0));
        mfinger12 = glm::rotate(mfinger12, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -.5f, 0));
        mfinger12 = mfinger11 * mfinger12;
        lightingShader.setMat4("model", glm::scale(mfinger12, glm::vec3(.4f, .75f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        mfinger13 = glm::mat4(1.0f);
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -0.4f, 0));
        mfinger13 = glm::rotate(mfinger13, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -.3f, 0));
        mfinger13 = mfinger12 * mfinger13;
        lightingShader.setMat4("model", glm::scale(mfinger13, glm::vec3(.4f, .5f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render finger pinky
        mfinger11 = glm::mat4(1.0f);
        mfinger11 = glm::translate(mfinger11, glm::vec3(-.75f, -1.25f, 0));
        mfinger11 = glm::rotate(mfinger11, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger11 = glm::rotate(mfinger11, glm::radians(spread*.75f), glm::vec3(0, 0, 1));
        mfinger11 = glm::translate(mfinger11, glm::vec3(0, -.75f, 0));
        mfinger11 = mWrist * mfinger11;
        lightingShader.setMat4("model", glm::scale(mfinger11, glm::vec3(.4f, 1.f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        mfinger12 = glm::mat4(1.0f);
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -0.5f, 0));
        mfinger12 = glm::rotate(mfinger12, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -.5f, 0));
        mfinger12 = mfinger11 * mfinger12;
        lightingShader.setMat4("model", glm::scale(mfinger12, glm::vec3(.4f, .75f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        mfinger13 = glm::mat4(1.0f);
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -0.4f, 0));
        mfinger13 = glm::rotate(mfinger13, glm::radians(angle), glm::vec3(1, 0, 0));
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -.3f, 0));
        mfinger13 = mfinger12 * mfinger13;
        lightingShader.setMat4("model", glm::scale(mfinger13, glm::vec3(.4f, .5f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render finger thum
        mfinger11 = glm::mat4(1.0f);
        mfinger11 = glm::translate(mfinger11, glm::vec3(1.f, 1.f, 0));
        mfinger11 = glm::rotate(mfinger11, glm::radians(80.0f), glm::vec3(.25, 1, .5));
        mfinger11 = glm::rotate(mfinger11, glm::radians(spread), glm::vec3(1, 0, 0));
        mfinger11 = glm::translate(mfinger11, glm::vec3(0, -.75f, 0));
        mfinger11 = mWrist * mfinger11;
        lightingShader.setMat4("model", glm::scale(mfinger11, glm::vec3(.4f, 1.f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        mfinger12 = glm::mat4(1.0f);
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -0.5f, 0));
        mfinger12 = glm::rotate(mfinger12, glm::radians(angle*.5f), glm::vec3(1, 0, 0));
        mfinger12 = glm::translate(mfinger12, glm::vec3(0, -.5f, 0));
        mfinger12 = mfinger11 * mfinger12;
        lightingShader.setMat4("model", glm::scale(mfinger12, glm::vec3(.4f, .75f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        mfinger13 = glm::mat4(1.0f);
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -0.4f, 0));
        mfinger13 = glm::rotate(mfinger13, glm::radians(angle*.5f), glm::vec3(1, 0, 0));
        mfinger13 = glm::translate(mfinger13, glm::vec3(0, -.3f, 0));
        mfinger13 = mfinger12 * mfinger13;
        lightingShader.setMat4("model", glm::scale(mfinger13, glm::vec3(.4f, .5f, .4f)));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset>0)
    {
        if (active < 1) 
        {
            active = active + .2;
        }
    }
    else
    {
        active = active - .2;
        if (active < 0) { active = 0; }
    }
}

