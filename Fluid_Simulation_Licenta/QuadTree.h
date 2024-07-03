#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <memory>
#include <iostream>

struct Point {
    float x, y;

    Point(float x, float y) : x(x), y(y) {}
};

struct AABB {
    Point center;
    float halfDimension;

    AABB(Point center, float halfDimension) : center(center), halfDimension(halfDimension) {}

    bool contains(Point point) const {
        return (point.x >= center.x - halfDimension &&
            point.x <= center.x + halfDimension &&
            point.y >= center.y - halfDimension &&
            point.y <= center.y + halfDimension);
    }

    bool intersects(const AABB& other) const {
        return !(other.center.x - other.halfDimension > center.x + halfDimension ||
            other.center.x + other.halfDimension < center.x - halfDimension ||
            other.center.y - other.halfDimension > center.y + halfDimension ||
            other.center.y + other.halfDimension < center.y - halfDimension);
    }
};

class Quadtree {
public:
    Quadtree(AABB boundary, int capacity);
    ~Quadtree();

    bool insert(Point point);
    void subdivide();
    std::vector<Point> queryRange(AABB range);
    void print() const;

private:
    AABB boundary;
    int capacity;
    std::vector<Point> points;
    bool divided;
    std::unique_ptr<Quadtree> northeast;
    std::unique_ptr<Quadtree> northwest;
    std::unique_ptr<Quadtree> southeast;
    std::unique_ptr<Quadtree> southwest;
};

#endif // QUADTREE_H
