#include <vector>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <memory>

// Define a simple Point structure
struct Point {
    double x, y;

    Point(double x, double y) : x(x), y(y) {}
};

// Define a simple Node structure
struct Node {
    Point point;
    std::shared_ptr<Node> parent;

    Node(Point point) : point(point), parent(nullptr) {}
};

class RRTPlanner {
private:
    std::vector<std::shared_ptr<Node>> nodes;
    Point start, goal;
    double step_size;
    double goal_threshold; // Distance threshold to consider the goal reached
    double max_iterations;

    // Method to find the nearest node to a given point
    std::shared_ptr<Node> nearestNode(Point point) {
        std::shared_ptr<Node> nearest;
        double min_dist = std::numeric_limits<double>::max();

        for (auto &node : nodes) {
            double dist = std::sqrt(std::pow(node->point.x - point.x, 2) + std::pow(node->point.y - point.y, 2));
            if (dist < min_dist) {
                min_dist = dist;
                nearest = node;
            }
        }

        return nearest;
    }

    // Generate a random point in the space
    Point randomPoint() {
        double rand_x = ((double) rand() / RAND_MAX) * 20; // Assuming a 20x20 space
        double rand_y = ((double) rand() / RAND_MAX) * 20;
        return Point(rand_x, rand_y);
    }

    // Method to add a new node
    bool addNode(std::shared_ptr<Node> nearest, Point new_point) {
        double dx = new_point.x - nearest->point.x;
        double dy = new_point.y - nearest->point.y;
        double mag = std::sqrt(dx * dx + dy * dy);
        // Trucate to step_size
        if (mag > step_size) {
            dx = step_size * dx / mag;
            dy = step_size * dy / mag;
        }

        Point new_node_point(nearest->point.x + dx, nearest->point.y + dy);
        nodes.push_back(std::make_shared<Node>(new_node_point));
        nodes.back()->parent = nearest;
        // Check if goal is reached
        if (std::sqrt(std::pow(new_node_point.x - goal.x, 2) + std::pow(new_node_point.y - goal.y, 2)) < goal_threshold)
            return true;

        return false;
    }

public:
    RRTPlanner(Point start, Point goal, double step_size = 1, double max_iterations = 10000, double goal_threshold = 0.5)
        : start(start), goal(goal), step_size(step_size), max_iterations(max_iterations), goal_threshold(goal_threshold) {
        nodes.push_back(std::make_shared<Node>(start));        
    }

    // Method to get the path to the last node
    std::vector<Point> getPathToLastNode() {
        std::vector<Point> path;
        if (!nodes.empty()) {
            std::shared_ptr<Node> current = nodes.back();
            while (current != nullptr) {
                Point point(current->point.x, current->point.y);
                path.push_back(point);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

    // Method to check if the goal is reached
    bool isGoalReached() const {
        if (!nodes.empty()) {
            const std::shared_ptr<Node> last_node = nodes.back();
            double dist_to_goal = std::sqrt(std::pow(last_node->point.x - goal.x, 2) + std::pow(last_node->point.y - goal.y, 2));
            return dist_to_goal <= goal_threshold;
        }
        return false;
    }

    // Main RRT planning method
    std::vector<Point> plan() {
        // TASK 2 TODO: build the algorithm
        // HINT: you can use randomPoint(), nearestNode(), addNode() and getPathToLastNode() for this task

        return std::vector<Point>(); // Return empty path if no path found
    }
};
