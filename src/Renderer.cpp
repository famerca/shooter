#include "Renderer.hpp"

Renderer::Renderer()
{
    fs::path vertex_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.vert";
    fs::path fragment_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.frag";
}

Renderer::~Renderer()
{   
}

void Renderer::init()
{
    
}

void Renderer::start(std::shared_ptr<Scene> scene)
{
    
}

void Renderer::stop()
{
    running = false;
}       

void Renderer::clear()
{
    
}   