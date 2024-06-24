//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "Vector.hpp"

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

class Material{
public:
    MaterialType m_type;//反射属性
    Vector3f m_color;//色彩
    Vector3f m_emission;//放射属性
    float ior;//折射相关的系数
    float Kd, Ks;//反射系数
    float specularExponent;
    //Texture tex;

    inline Material(MaterialType t=DIFFUSE_AND_GLOSSY, Vector3f c=Vector3f(1,1,1), Vector3f e=Vector3f(0,0,0));
    inline MaterialType getType();
    inline Vector3f getColor();
    inline Vector3f getColorAt(double u, double v);
    inline Vector3f getEmission();


};

Material::Material(MaterialType t, Vector3f c, Vector3f e){//初始化反射，色彩，放射
    m_type = t;
    m_color = c;
    m_emission = e;
}

MaterialType Material::getType(){return m_type;}//返回反射属性
Vector3f Material::getColor(){return m_color;}//返回颜色
Vector3f Material::getEmission() {return m_emission;}//返回放射属性

Vector3f Material::getColorAt(double u, double v) {//这个函数怎么没写？？？
    return Vector3f();
}
#endif //RAYTRACING_MATERIAL_H
