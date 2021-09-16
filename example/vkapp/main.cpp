#include <iostream>
#include <string>
#include "vk/vkRenderer.h"

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    valkyr::render::RenderSetting setting = {};
    std::string szTitle = "vkapp";
    setting.title = std::string_view(szTitle.c_str(), szTitle.size());
    setting.width = 1920;
    setting.height = 1080;
    GLFWwindow* window = glfwCreateWindow(setting.width, setting.height, setting.title.data(), nullptr, nullptr);
    valkyr::vptr<valkyr::render::Renderer> renderer = vmake_ptr<valkyr::render::vkRenderer>();
    setting.window = (void*)window;
    renderer->init(setting);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    renderer->destroy();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
