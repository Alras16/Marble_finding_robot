#include "bst_coordinates.h"


BST_coordinates::BST_coordinates() : root{ nullptr }
{
}


BST_coordinates::BST_coordinates(const BST_coordinates & rhs) : root{ nullptr }
{
    root = clone(rhs.root);
}


BST_coordinates::BST_coordinates(BST_coordinates && rhs) : root{ rhs.root }
{
    rhs.root = nullptr;
}

BST_coordinates::~BST_coordinates()
{
    makeEmpty();
}

void BST_coordinates::generateCompleteTree(std::vector<cv::Point> input)
{
    // make pointer to the tree
    std::vector<std::vector<cv::Point>> list;
    list.push_back(input);

    while (list.size() != 0)
    {
        if (list[0].size() == 1)
        {
            insert(list[0][0]);
            list.erase(list.begin());
        }
        else if (list[0].size() == 2) // two elements in the vector
        {
                insert(list[0][1]);
                insert(list[0][0]);

            list.erase(list.begin()); // delete first vector
        }
        else if (abs(std::floor(log2(list[0].size() + 1))) == log2(list[0].size() + 1)) // size equal full tree
        {
            // prepare to split vector in two
            // make two copies and delete original

            int parent_index = 0;
            std::vector<cv::Point> temp_one = list[0];
            std::vector<cv::Point> temp_two = list[0];
            list.erase(list.begin());

            // allways odd
            parent_index = ((temp_one.size() + 1) / 2);

            insert(temp_one[parent_index - 1]);
            temp_one.erase(temp_one.begin() + (parent_index - 1), temp_one.end());
            temp_two.erase(temp_two.begin(), temp_two.begin() + parent_index);

            list.push_back(temp_one);
            list.push_back(temp_two);
        }
        else // four or more elements in the vector
        {
            int height = 0;
            int max_elements = 0;
            int min_dist = 100000;
            int cur_dist = 0;
            while (unsigned(height) < list[0].size())
            {
                height++;
                max_elements = pow(2, height) - 1;
                cur_dist = abs(max_elements - signed(list[0].size()));
                if (cur_dist < min_dist)
                    min_dist = cur_dist;
                else if ((cur_dist >= min_dist) && (min_dist != 100000))
                {
                    height--;
                    break;
                }
            }

            int parent_index;
            if ((list[0].size() > (pow(2, height) - 1) - pow(2, height - 2)) && (list[0].size() < (pow(2, height) - 1)))
                parent_index = pow(2, height) / 2;
            else
                parent_index = pow(2, height) / 2 + min_dist;

            // prepare to split vector in two
            // make two copies and delete original
            std::vector<cv::Point> temp_one = list[0];
            std::vector<cv::Point> temp_two = list[0];
            list.erase(list.begin());

            insert(temp_one[parent_index - 1]);

            temp_one.erase(temp_one.begin() + (parent_index - 1), temp_one.end());
            temp_two.erase(temp_two.begin(), temp_two.begin() + parent_index);

            list.push_back(temp_one);
            list.push_back(temp_two);
        }
    }
}

const cv::Point & BST_coordinates::findMin() const
{
    if (isEmpty())
        return cv::Point (-1,-1);
    return findMin(root)->element;
}


const cv::Point &BST_coordinates::findMax() const
{
    if (isEmpty())
        return cv::Point (-1,-1);
    return findMax(root)->element;
}

bool BST_coordinates::contains(const cv::Point &x)
{
    return contains(x, root);
}

bool BST_coordinates::isEmpty() const
{
    return root == nullptr;
}

void BST_coordinates::makeEmpty()
{
    makeEmpty(root);
}

void BST_coordinates::insert(const cv::Point & x)
{
    insert(x, root);
}

void BST_coordinates::insert(cv::Point &&x)
{
    insert(std::move(x), root);
}

void BST_coordinates::remove(const cv::Point & x)
{
    remove(x, root);
}

std::vector<cv::Point> BST_coordinates::convertToList()
{
    std::vector<cv::Point> temp_points;
    std::vector<BinaryNode *> queue;
    BinaryNode *curr = root;

    while ((curr != NULL) || (queue.size() != 0))
    {
        while (curr !=  NULL)
        {
            queue.push_back(curr);
            curr = curr->left;
        }
        curr = queue[queue.size() - 1];
        queue.erase(queue.begin() + queue.size() - 1);

        temp_points.push_back(curr->element);
        curr = curr->right;

    }
    return temp_points;
}

int BST_coordinates::compare(cv::Point e_one, cv::Point e_two)
{
    if (e_one.x < e_two.x)
        return SMALLER_THAN;
    else if (e_one.x > e_two.x)
        return LARGER_THAN;
    else if (e_one.y < e_two.y)
        return SMALLER_THAN;
    else if (e_one.y > e_two.y)
        return LARGER_THAN;
    else
        return EQUAL_TO;
}

void BST_coordinates::insert(const cv::Point & x, BinaryNode * & t)
{
    if (t == nullptr)
        t = new BinaryNode{ x, nullptr, nullptr };
    else if (compare(x,t->element) == SMALLER_THAN)
        insert(x, t->left);
    else if (compare(x,t->element) == LARGER_THAN)
        insert(x, t->right);
    else
        ;  // Duplicate; do nothing
}


void BST_coordinates::insert(cv::Point &&x, BinaryNode * & t)
{
    if (t == nullptr)
        t = new BinaryNode{ std::move(x), nullptr, nullptr };
    else if (compare(x,t->element) == SMALLER_THAN)
        insert(std::move(x), t->left);
    else if (compare(x,t->element) == LARGER_THAN)
        insert(std::move(x), t->right);
    else
        ;  // Duplicate; do nothing
}

void BST_coordinates::remove(const cv::Point & x, BinaryNode * & t)
{
    if (t == nullptr)
        return;   // Item not found; do nothing
    if (compare(x,t->element) == SMALLER_THAN)
        remove(x, t->left);
    else if (compare(x,t->element) == LARGER_THAN)
        remove(x, t->right);
    else if (t->left != nullptr && t->right != nullptr) // Two children
    {
        t->element = findMin(t->right)->element;
        remove(t->element, t->right);
    }
    else
    {
        BinaryNode *oldNode = t;
        t = (t->left != nullptr) ? t->left : t->right;
        delete oldNode;
    }
}

BinaryNode * BST_coordinates::findMin(BinaryNode *t) const
{
    if (t == nullptr)
        return nullptr;
    if (t->left == nullptr)
        return t;
    return findMin(t->left);
}

BinaryNode * BST_coordinates::findMax(BinaryNode *t) const
{
    if (t != nullptr)
        while (t->right != nullptr)
            t = t->right;
    return t;
}

bool BST_coordinates::contains(const cv::Point &x, BinaryNode *t)
{
    if (t == nullptr)
        return false;
    else if (compare(x,t->element) == SMALLER_THAN)
        return contains(x, t->left);
    else if (compare(x,t->element) == LARGER_THAN)
        return contains(x, t->right);
    else
        return true;    // Match
}

void BST_coordinates::makeEmpty(BinaryNode * & t)
{
    if (t != nullptr)
    {
        makeEmpty(t->left);
        makeEmpty(t->right);
        delete t;
    }
    t = nullptr;
}

BinaryNode * BST_coordinates::clone(BinaryNode *t) const
{
    if (t == nullptr)
        return nullptr;
    else
        return new BinaryNode{ t->element, clone(t->left), clone(t->right) };
}
