#ifndef UTILS_HPP
#define UTILS_HPP

#define M_PI 3.14

class Utils
{

    public:

    static float to_radian(float degrees)
    {
        return degrees * M_PI / 180.f;
    }    


};

#endif // UTILS_HPP