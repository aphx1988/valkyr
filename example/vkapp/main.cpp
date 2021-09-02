#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <iostream>
#include "vk/vkRenderer.h"

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    valkyr::RenderSetting setting = {};
    setting.width = 1920;
    setting.height = 1080;
    GLFWwindow* window = glfwCreateWindow(setting.width, setting.height, "vkapp", nullptr, nullptr);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
