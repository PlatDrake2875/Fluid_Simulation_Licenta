#include "Octree.h"

Octree::Octree(AABB3D boundary, int capacity)
    : boundary(boundary), capacity(capacity), divided(false) {}

Octree::~Octree() {}

bool Octree::insert(Point3D point) {
    if (!boundary.contains(point)) {
        return false;
    }

    if (points.size() < capacity) {
        points.push_back(point);
        return true;
    }

    if (!divided) {
        subdivide();
    }

    for (int i = 0; i < 8; ++i) {
        if (children[i]->insert(point)) {
            return true;
        }
    }

    return false;
}

void Octree::subdivide() {
    float x = boundary.center.x;
    float y = boundary.center.y;
    float z = boundary.center.z;
    float half = boundary.halfDimension / 2;

    children[0] = std::make_unique<Octree>(AABB3D(Point3D(x - half, y - half, z - half), half), capacity);
    children[1] = std::make_unique<Octree>(AABB3D(Point3D(x + half, y - half, z - half), half), capacity);
    children[2] = std::make_unique<Octree>(AABB3D(Point3D(x - half, y + half, z - half), half), capacity);
    children[3] = std::make_unique<Octree>(AABB3D(Point3D(x + half, y + half, z - half), half), capacity);
    children[4] = std::make_unique<Octree>(AABB3D(Point3D(x - half, y - half, z + half), half), capacity);
    children[5] = std::make_unique<Octree>(AABB3D(Point3D(x + half, y - half, z + half), half), capacity);
    children[6] = std::make_unique<Octree>(AABB3D(Point3D(x - half, y + half, z + half), half), capacity);
    children[7] = std::make_unique<Octree>(AABB3D(Point3D(x + half, y + half, z + half), half), capacity);

    divided = true;
}

std::vector<Point3D> Octree::queryRange(AABB3D range) {
    std::vector<Point3D> pointsInRange;

    if (!boundary.intersects(range)) {
        return pointsInRange;
    }

    for (const Point3D& point : points) {
        if (range.contains(point)) {
            pointsInRange.push_back(point);
        }
    }

    if (divided) {
        for (int i = 0; i < 8; ++i) {
            std::vector<Point3D> childPoints = children[i]->queryRange(range);
            pointsInRange.insert(pointsInRange.end(), childPoints.begin(), childPoints.end());
        }
    }

    return pointsInRange;
}

void Octree::print() const {
    std::cout << "Boundary: (" << boundary.center.x << ", " << boundary.center.y << ", " << boundary.center.z << ") with half dimension " << boundary.halfDimension << "\n";
    std::cout << "Points: ";
    for (const auto& point : points) {
        std::cout << "(" << point.x << ", " << point.y << ", " << point.z << ") ";
    }
    std::cout << "\n";

    if (divided) {
        for (int i = 0; i < 8; ++i) {
            std::cout << "Child " << i << ":\n";
            children[i]->print();
        }
    }
}
