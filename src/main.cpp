#include <GLFW/glfw3.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#define UNUSED(expr) (void)expr

static void error_callback(int, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // TODO: Remove unused macro when these are used
    UNUSED(scancode);
    UNUSED(mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
int main()
{
    std::cout << "Init GLFW begin" << std::endl;

    GLFWwindow * window = nullptr;
    glfwSetErrorCallback(error_callback);

    if (glfwInit() == false)
    {
    std::cout << "Failed to init GLFW" << std::endl;
        exit(EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    std::cout << "Init GLFW finished" << std::endl;
    std::cout << "Creating GLFW window" << std::endl;

    window = glfwCreateWindow(640, 480, "OGAT - GOAT", NULL, NULL);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    std::cout << "Created GLFW window" << std::endl;

    glfwSetKeyCallback(window, key_callback);

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
    }

    std::cout << "Quitting" << std::endl;
    glfwDestroyWindow(window);
    window = nullptr;

    glfwTerminate();

    return EXIT_SUCCESS;
}
