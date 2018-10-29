#ifndef BST_COORDINATES_H
#define BST_COORDINATES_H

#define LARGER_THAN 1
#define SMALLER_THAN 2
#define EQUAL_TO 3

#include <iostream>
#include <vector>
#include <math.h>
#include <opencv2/opencv.hpp>

class BinaryNode
{
public:
    cv::Point element;
    BinaryNode *left;
    BinaryNode *right;

    BinaryNode(const cv::Point & theElement, BinaryNode *lt, BinaryNode *rt)
        : element{ theElement }, left{ lt }, right{ rt } { }

    BinaryNode(cv::Point && theElement, BinaryNode *lt, BinaryNode *rt)
        : element{ std::move(theElement) }, left{ lt }, right{ rt } { }
};

class BST_coordinates
{
public:

    BST_coordinates();
    BST_coordinates(const BST_coordinates & rhs);
    BST_coordinates(BST_coordinates && rhs);
    ~BST_coordinates();

    void generateCompleteTree(std::vector<cv::Point> input);

    const cv::Point & findMin() const;
    const cv::Point & findMax() const;
    bool contains(const cv::Point & x);
    bool isEmpty() const;

    void makeEmpty();
    void insert(const cv::Point &x);
    void insert(cv::Point && x);
    void remove(const cv::Point &x);

    std::vector<cv::Point> convertToList();

private:
    BinaryNode *root;

    int compare(cv::Point e_one,cv::Point e_two);

    void insert(const cv::Point &x, BinaryNode * & t);
    void insert(cv::Point && x, BinaryNode * & t);
    void remove(const cv::Point &x, BinaryNode * & t);
    BinaryNode * findMin(BinaryNode *t) const;
    BinaryNode * findMax(BinaryNode *t) const;

    bool contains(const cv::Point & x, BinaryNode *t);

    void makeEmpty(BinaryNode * & t);
    BinaryNode * clone(BinaryNode *t) const;

};

#endif // BST_COORDINATES_H
