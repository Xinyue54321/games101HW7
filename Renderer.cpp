//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"


inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;


//Vector3f castRay(
//    const BVHAccel& bvh, const Ray& ray, const Scene& scene,
//    int depth)
//{
//    if (depth > scene.maxDepth) {
//        return Vector3f(0.0, 0.0, 0.0);
//    }
//
//    Vector3f hitColor = scene.backgroundColor;
//
//    Intersection point = bvh.Intersect(ray);
//    Vector2f uv,st;
//    uint32_t index;
//    Vector3f N;
//    point.obj->getSurfaceProperties(point.coords, ray.direction, index, uv, N, st);//获取表面属性法线
//    switch (point.m->m_type)
//    {
//        case REFLECTION_AND_REFRACTION:
//        {
//            Vector3f reflectionDirection;
//            Vector3f refractionDirection;
//            Vector3f reflectionRayOrig;
//            Vector3f refractionRayOrig;
//            float kr;
//            reflectionDirection = normalize(scene.reflect(ray.direction, N));
//            refractionDirection = normalize(scene.refract(ray.direction, N, point.m->ior));
//            reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ?
//                point.coords - N * 0.00001 :
//                point.coords + N * 0.00001;
//            refractionDirection = (dotProduct(refractionDirection, N) < 0) ?
//                point.coords - N * 0.00001 :
//                point.coords + N * 0.00001;
//            scene.fresnel(ray.direction, N, point.m->ior, kr);
//            Ray reflectRay(reflectionRayOrig, reflectionDirection), refractRay(refractionRayOrig, refractionDirection);
//            hitColor = castRay(bvh, reflectRay, scene, depth + 1) * kr + castRay(bvh, refractRay, scene, depth + 1) * (1 - kr);
//            break;
//        }
//        case REFLECTION:
//        {
//            float kr;
//            Vector3f reflectionDirection;
//            Vector3f  reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ?
//                point.coords - N * 0.00001 :
//                point.coords + N * 0.00001;
//            reflectionDirection = normalize(scene.reflect(ray.direction, N));
//            scene.fresnel(ray.direction, N, 0, kr);
//            Ray reflectRay(reflectionRayOrig, reflectionDirection);
//            hitColor = castRay(bvh, reflectRay, scene, depth + 1) * kr;
//            break;
//        }
//        default:
//        {
//            // [comment]
//                // We use the Phong illumation model int the default case. The phong model
//                // is composed of a diffuse and a specular reflection component.
//                // [/comment]
//            Vector3f lightAmt = 0, specularColor = 0;
//            Vector3f shadowPointOrig = (dotProduct(ray.direction, N) < 0) ?
//                point.coords + N * 0.00001 :
//                point.coords - N * 0.00001;
//            // [comment]
//            // Loop over all lights in the scene and sum their contribution up
//            // We also apply the lambert cosine law
//            // [/comment]
//            for (auto& light : scene.get_lights()) {
//                Vector3f lightDir = light->position - point.coords;
//                // square of the distance between hitPoint and the light
//                float lightDistance2 = dotProduct(lightDir, lightDir);
//                lightDir = normalize(lightDir);
//                float LdotN = std::max(0.f, dotProduct(lightDir, N));
//                // is the point in shadow, and is the nearest occluding object closer to the object than the light itself?
//                auto shadow_res = scene.trace(ray, scene.get_objects(),);
//                bool inShadow = shadow_res && (shadow_res->tNear * shadow_res->tNear < lightDistance2);
//
//                lightAmt += inShadow ? 0 : light->intensity * LdotN;
//                Vector3f reflectionDirection = reflect(-lightDir, N);
//
//                specularColor += powf(std::max(0.f, -dotProduct(reflectionDirection, dir)),
//                    payload->hit_obj->specularExponent) * light->intensity;
//            }
//
//            hitColor = lightAmt * payload->hit_obj->evalDiffuseColor(st) * payload->hit_obj->Kd + specularColor * payload->hit_obj->Ks;
//            break;
//        }     
//    }
//
//
//    return hitColor;
//}

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(-1, 5, 10);
    int m = 0;

    //建立bvh树
    //auto bvh = BVHAccel(scene.objects);


    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
            // generate primary ray direction
            float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                      imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;
            // TODO: Find the x and y positions of the current pixel to get the
            // direction
            //  vector that passes through it.
            // Also, don't forget to multiply both of them with the variable
            // *scale*, and x (horizontal) variable with the *imageAspectRatio*

            // Don't forget to normalize this direction!
            Vector3f dir = Vector3f(x, y, -1); // Don't forget to normalize this direction!
            dir = normalize(dir);
            Ray ray(eye_pos, dir);
            //Intersection point = bvh.Intersect(ray);
            auto color = scene.castRay(ray, 0);
            framebuffer[m++] = color;
            //std::cout << color << std::endl;
            //framebuffer[m++] = scene.castRay(ray, 0);
        }
        UpdateProgress(j / (float)scene.height);
    }
    UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp;
    fopen_s(&fp,"binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].x));
        color[1] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].y));
        color[2] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].z));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
