#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "vk/vkRenderer.h"

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    valkyr::render::RenderSetting setting = {};
    std::string szTitle = "vkapp";
    setting.title = std::string_view(szTitle.c_str(), szTitle.size());
    setting.width = 1920;
    setting.height = 1080;
    GLFWwindow* window = glfwCreateWindow(setting.width, setting.height, setting.title.data(), nullptr, nullptr);
    valkyr::vptr<valkyr::render::Renderer> renderer = vmake_ptr<valkyr::render::vkRenderer>();
    renderer->init(setting);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    renderer->destroy();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
