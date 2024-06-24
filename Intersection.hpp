//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"
class Object;
class Sphere;

struct Intersection
{
    Intersection(){
        happened=false;
        coords=Vector3f();
        normal=Vector3f();
        distance= std::numeric_limits<double>::max();
        obj =nullptr;
        m=nullptr;
    }
    bool happened;//交了没
    Vector3f coords;//交点
    Vector3f normal;//交点法线
    double distance;//相交的时间;
    Object* obj;//物体
    Material* m;//材质
};
#endif //RAYTRACING_INTERSECTION_H
