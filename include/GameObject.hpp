#include "Component.hpp"
#include <vector>

class GameObject
{
private:
    /* data */
    Component *transform;
    std::vector<Component *> components;
    bool visible;
public:
    GameObject(Component *);
    ~GameObject();

    bool isVisible();
    void setVisible(bool);

    Component * getTransform();
    void update(GLfloat);
};


