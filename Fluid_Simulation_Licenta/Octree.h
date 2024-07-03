#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <memory>
#include <iostream>

struct Point3D {
    float x, y, z;

    Point3D(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct AABB3D {
    Point3D center;
    float halfDimension;

    AABB3D(Point3D center, float halfDimension) : center(center), halfDimension(halfDimension) {}

    bool contains(Point3D point) const {
        return (point.x >= center.x - halfDimension &&
            point.x <= center.x + halfDimension &&
            point.y >= center.y - halfDimension &&
            point.y <= center.y + halfDimension &&
            point.z >= center.z - halfDimension &&
            point.z <= center.z + halfDimension);
    }

    bool intersects(const AABB3D& other) const {
        return !(other.center.x - other.halfDimension > center.x + halfDimension ||
            other.center.x + other.halfDimension < center.x - halfDimension ||
            other.center.y - other.halfDimension > center.y + halfDimension ||
            other.center.y + other.halfDimension < center.y - halfDimension ||
            other.center.z - other.halfDimension > center.z + halfDimension ||
            other.center.z + other.halfDimension < center.z - halfDimension);
    }
};

class Octree {
public:
    Octree(AABB3D boundary, int capacity);
    ~Octree();

    bool insert(Point3D point);
    void subdivide();
    std::vector<Point3D> queryRange(AABB3D range);
    void print() const;

private:
    AABB3D boundary;
    int capacity;
    std::vector<Point3D> points;
    bool divided;
    std::unique_ptr<Octree> children[8];
};

#endif // OCTREE_H
