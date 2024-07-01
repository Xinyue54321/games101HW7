#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;
    if (splitMethod == SplitMethod::NAIVE) root = recursiveBuild(primitives);
    else root = recursiveBuildSAH(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuildSAH(std::vector<Object*> objects)
{//使用SAH算法建立一棵BVH树
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{ objects[0] });
        node->right = recursiveBuild(std::vector{ objects[1] });

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        int objectNum = objects.size();
        int bulletNum = std::min(objectNum, 32);
        std::vector<Bounds3> bullet(bulletNum);
        for (int i = 0; i < objectNum; ++i)
            centroidBounds =
            Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {//将最长边排序
        case 0://按照x轴排序
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                    f2->getBounds().Centroid().x;
                });
            break;
        case 1://按y
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                    f2->getBounds().Centroid().y;
                });
            break;
        case 2://按z
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                    f2->getBounds().Centroid().z;
                });
            break;
        }
        double totalArea = centroidBounds.SurfaceArea();
        double minSAH = std::numeric_limits<double>::max();
        int division = -1;
        for (int i = 0;i < bulletNum;i++) {
            auto beginning = objects.begin();
            auto middling = objects.begin() + objects.size() / bulletNum * i;
            auto ending = objects.end();
            auto leftshapes = std::vector<Object*>(beginning, middling);
            auto rightshapes = std::vector<Object*>(middling, ending);
            Bounds3 left, right;
            for (int k = 0;k < leftshapes.size();k++) {
                left = Union(left, leftshapes[k]->getBounds());
            }
            for (int k = 0;k < rightshapes.size();k++) {
                right = Union(right, rightshapes[k]->getBounds());
            }
            double SL = left.SurfaceArea();
            double SR = right.SurfaceArea();
            double SAH = leftshapes.size() * SL / totalArea + rightshapes.size() * SR / totalArea;
            if (SAH < minSAH) {
                minSAH = SAH;
                division = i;
            }

        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + objects.size() / bulletNum * division;
        auto ending = objects.end();
        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);
        
        assert(objects.size() == (leftshapes.size() + rightshapes.size()));//检查有没有漏掉或者多算object，有错中止

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
    
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{//建立一个bvh节点，递归算法，所以把树直接建好了
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {//将最长边划分
        case 0://按照x轴划分
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1://按y
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2://按z
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));//检查有没有漏掉或者多算object，有错中止

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    //Intersection inter, left, right;
    //if (!node->bounds.IntersectP(ray, ray.direction, std::array<int, 3> {(int)ray.direction.x, (int)ray.direction.y, (int)ray.direction.z})) {
    //    return inter;
    //}
    //if (node->left == nullptr) {
    //    return node->object->getIntersection(ray);
    //}
    //left = getIntersection(node->left, ray);
    //right = getIntersection(node->right, ray);
    //return left.distance < right.distance ? left : right;
    Intersection left, right;
    if (node->left == nullptr) {
        return node->object->getIntersection(ray);
    }
    
    if (node->left->bounds.IntersectP(ray, ray.direction_inv, std::array<int, 3> {(int)ray.direction.x, (int)ray.direction.y, (int)ray.direction.z})) {
        left = BVHAccel::getIntersection(node->left, ray);
    }
    if (node->right->bounds.IntersectP(ray, ray.direction_inv, std::array<int, 3> {(int)ray.direction.x, (int)ray.direction.y, (int)ray.direction.z})) {
        right = BVHAccel::getIntersection(node->right, ray);
    }
    return left.distance < right.distance ? left : right;
}