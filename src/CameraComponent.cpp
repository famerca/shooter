#include "CameraComponent.hpp"
#include "Scene.hpp"
#include "AudioManager.hpp"
#include "Utils.hpp"

namespace Engine
{

CameraComponent::CameraComponent(Owner owner, std::shared_ptr<Scene> scene): Component(owner, Component::Type::Camera), scene(scene)
{
    position = glm::vec3(0.f, 0.f, 0.f);
    front = glm::vec3(0.f, 0.f, 1.f);
    up = glm::vec3(0.f, 1.f, 0.f);
    aspectRation = 1.f;
    fov = 45.f;
    nearPlane = 0.1f;
    farPlane = 100.f;
    isOrthographic = false;
    x_rotation = 0.f;
    y_rotation = 0.f;
    changed = true;
    perpective_changed = true;
    renderd = false;
    pitch = 0.f;
    yaw = 0.f;
}

std::shared_ptr<CameraComponent> CameraComponent::create(Owner owner, std::shared_ptr<Scene> scene)
{
    return std::make_shared<CameraComponent>(owner, scene);
}

std::shared_ptr<CameraComponent> CameraComponent::self()
{
    return shared_from_this();
}

void CameraComponent::updateRotation(const GLfloat &dt)
{
    if(rotation_changed)
    {
        pitch += y_rotation * dt;
        yaw   += x_rotation * dt;
        
        // Clamp del pitch para que la cámara no pase por debajo del suelo ni de una voltereta completa
        if(clampPitch > 0.0f)
            pitch = std::max(-clampPitch, std::min(clampPitch, pitch));
        if(clampYaw > 0.0f)
            yaw   = std::max(-clampYaw, std::min(clampYaw, yaw));

        rotation_changed = false;
        changed = true; // Marcar para recalcular matriz
        
        // Resetear inputs temporales
        x_rotation = 0.f;
        y_rotation = 0.f;
    }
}

void CameraComponent::updatePerpective()
{
    if(perpective_changed)
    {
        projection = glm::perspective(glm::radians(fov), aspectRation, nearPlane, farPlane);
        perpective_changed = false;
        changed = true;
    }
}

void CameraComponent::setDistance(float dist)
{
    distance = dist;
    changed = true;
}

void CameraComponent::calcFront()
{

    front.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, up));
    std::cout << "[CameraComponent] front: " << Utils::toJoltVec3(front) << std::endl;
    std::cout << "[CameraComponent] right: " << Utils::toJoltVec3(right) << std::endl;

}

void CameraComponent::update(const GLfloat &dt)
{
    // Actualizar valores de pitch y yaw basados en input (tu lógica de x_rotation/y_rotation)
    updateRotation(dt);
    updatePerpective();

    if(changed)
    {
        // 1. OBTENER LA POSICIÓN DEL JUGADOR (TARGET)
        // Le sumamos un offset para mirar al pecho/cabeza, no al pivote que suele estar en los pies
        glm::vec3 targetPos = owner->getTransform()->getPosition() + targetOffset;

        if (isOrbiting) 
        {
            // --- LÓGICA DE TERCERA PERSONA (ORBITAL) ---

            // A. Calcular la dirección desde el centro hacia afuera (esféricas)
            // Nota: Usamos trigonometría para convertir Ángulos -> Vector Dirección
            float hDist = distance * glm::cos(glm::radians(pitch));
            float xOffset = hDist * glm::sin(glm::radians(yaw));
            float zOffset = hDist * glm::cos(glm::radians(yaw));
            float yOffset = distance * glm::sin(glm::radians(pitch));

            // B. Calcular la posición de la cámara
            // Si yaw/pitch son 0, la cámara estará detrás del jugador.
            // Restamos offsets o sumamos dependiendo de tu sistema de coordenadas.
            // Asumiendo OpenGL estándar (Z+ sale de la pantalla):
            glm::vec3 orbitOffset;
            orbitOffset.x = xOffset; 
            orbitOffset.y = yOffset; 
            orbitOffset.z = zOffset;

            // Posición final = Centro del Jugador + Vector Rotado
            // NOTA: Si quieres que la cámara esté "detrás", a menudo se resta el vector dirección.
            // Aquí calculamos la posición en la esfera.
            front = glm::normalize(orbitOffset);
            right = glm::normalize(glm::cross(front, up));

            std::cout << "[CameraComponent] front: " << Utils::toJoltVec3(front) << std::endl;
            std::cout << "[CameraComponent] right: " << Utils::toJoltVec3(right) << std::endl;
            
            glm::vec3 finalCamPos = targetPos - orbitOffset; // El '-' la pone "detrás" de la dirección de vista

            // C. Configurar la vista
            // La cámara está en finalCamPos y mira a targetPos
            view = glm::lookAt(finalCamPos, targetPos, up);
            
        }
        else 
        {
            // --- TU LÓGICA ORIGINAL (PRIMERA PERSONA) ---
            calcFront(); // Calcula hacia dónde miro
            auto absPos = owner->getTransform()->getPosition() + position; // Mi posición es fija relativa al padre
            view = glm::lookAt(absPos, absPos + front, up);
            Engine::AudioManager::Get().setListenerDirection(front);

        }

        changed = false;
        renderd = false;
    }

    
}

const glm::vec3& CameraComponent::getForward() const
{
    return front; 
}

const glm::vec3& CameraComponent::getRight() const
{
    return right; 
}


void CameraComponent::rotate(GLfloat x, GLfloat y)
{
    x_rotation = x;
    y_rotation = y;
    rotation_changed = true;
}

void CameraComponent::setOrbiting(bool orbiting)
{
    isOrbiting = orbiting;
}

void CameraComponent::setClamp(float pitch, float yaw)
{
    clampPitch = pitch;
    clampYaw = yaw;
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::init(glm::vec3 pos, float aspect, float fov, float near, float far, bool orthographic)
{
    position = pos;
    aspectRation = aspect;
    this->fov = fov;
    nearPlane = near;
    farPlane = far;
    isOrthographic = orthographic;

    update(1.f);
}

void CameraComponent::setPosition(glm::vec3 pos)
{
    position = pos;
    changed = true;
}

void CameraComponent::setAspectRation(float aspect)
{
    aspectRation = aspect;
    perpective_changed = true;
}

void CameraComponent::setFov(float fov)
{
    this->fov = fov;
    changed = true;
}

void CameraComponent::setNearPlane(float near)
{
    nearPlane = near;
    perpective_changed = true;
}

void CameraComponent::setFarPlane(float far)
{
    farPlane = far;
    perpective_changed = true;
}

void CameraComponent::setOrthographic(bool orthographic)
{
    isOrthographic = orthographic;
    perpective_changed = true;
}

void CameraComponent::setFront(glm::vec3 front)
{
    this->front = front;
    right = glm::normalize(glm::cross(front, up));
    changed = true;
}

void CameraComponent::setFront(GLfloat pitch, GLfloat yaw)
{
    this->pitch = pitch;
    this->yaw = yaw;
    calcFront();
    changed = true;
}

void CameraComponent::setUp(glm::vec3 up)
{
    this->up = up;
    changed = true;
}

glm::mat4 CameraComponent::getViewMatrix()
{
    renderd = true;
    return view;
}

glm::mat4 CameraComponent::getProjectionMatrix()
{
    return projection;
}

void CameraComponent::activate()
{
    scene->setCamera(self());
}

}