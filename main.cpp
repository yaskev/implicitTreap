#include <iostream>
#include <stack>
#include <random>

using namespace std;

template <typename T>
class Treap {
public:
    void insertAt(T data, int position);
    T getAt(int position) { return getAt(root, position); }
    T deleteAt(int position);

    Treap() = default;
    ~Treap();
    Treap(const Treap&) = delete;
    Treap(Treap&&) = delete;
    Treap& operator=(const Treap&) = delete;
    Treap& operator=(Treap&&) = delete;
private:
    struct Node;
    bool split(Node *node, int size, Node*& left, Node*& right);
    Node *merge(Node *left, Node *right);
    Node *root = nullptr;

    T getAt(Node *node, int position);

    default_random_engine& getEngine();
    uniform_int_distribution<int>& getInt();
    int randomInt() { return getInt()(getEngine()); }
};

template <typename T>
default_random_engine& Treap<T>::getEngine() {
    static default_random_engine engine(static_cast<unsigned int>(10123));
    return engine;
}

template <typename T>
uniform_int_distribution<int>& Treap<T>::getInt() {
    static uniform_int_distribution<int> randInt(1, 100);
    return randInt;
}

template <typename T>
struct Treap<T>::Node {
public:
    T data;
    int priority;
    int size = 1;

    Node *parent = nullptr;
    Node *left = nullptr;
    Node *right = nullptr;

    void update();
    Node(const T& data, int priority);
};

template <typename T>
void Treap<T>::Node::update() {
    size = 1;
    if( left ) size += left->size;
    if( right ) size += right->size;
}

template <typename T>
Treap<T>::Node::Node(const T& data_, int priority_) {
    data = data_;
    priority = priority_;
    size = 1;
}

template <typename T>
T Treap<T>::getAt(Node *node, int position) {
    Node *left = node->left;
    if( left != nullptr  ) {
        if( left->size == position )
            return node->data;
        if( left->size > position )
            return getAt(left, position);
        else
            return getAt(node->right, position - left->size - 1);
    }
    if( position == 0 ) return node->data;
    return getAt(node->right, position - 1);
}

template <typename T>
T Treap<T>::deleteAt(int position) {
    Node *left = nullptr;
    Node *right = nullptr;
    Node *tmp = nullptr;
    split(root, position, left, right);
    split(right, 1, tmp, right);
    root = merge(left, right);
    T data = tmp->data;
    delete tmp;
    return data;
}

template <typename T>
bool Treap<T>::split(Node *node, int size, Node*& left, Node*& right){
    // Split the treap into 2 treaps by the given key
    if( node == nullptr ){
        left = nullptr;
        right = nullptr;
        return true;
    }
    int Lsize = (node->left) ? node->left->size : 0;

    if( Lsize >= size ) {
        split(node->left, size, left, node->left);
        right = node;
        if( node->left ) node->left->parent = node;
        right->update();

        return true;
    }
    split(node->right, size - Lsize - 1, node->right, right);
    left = node;
    if( node->right ) node->right->parent = node;
    left->update();

    return true;
}

template <typename T>
typename Treap<T>::Node *Treap<T>::merge(Node *left, Node *right){
    // Create one treap of the given two
    if( left == nullptr || right == nullptr )
        return left == nullptr ? right : left;
    if( left->priority > right->priority ){
        left->right = merge(left->right, right);
        left->update();
        return left;
    }
    right->left = merge(left, right->left);
    right->update();
    return right;
}

template <typename T>
void Treap<T>::insertAt(T data, int position) {
    Node *elem = new Node(data, randomInt());
    Node *left = nullptr, *right = nullptr;
    split(root, position, left, right);
    left = merge(left, elem);
    root = merge(left, right);
    root->update();
}

template <typename T>
Treap<T>::~Treap() {
    stack<Node*> stack;
    Node *root_ = root;
    if( root_->right ) stack.push(root_->right);
    stack.push(root_);
    root_ = root_->left;
    while( !stack.empty() ) {
        while( root_ ) {
            if (root_->right) stack.push(root_->right);
            stack.push(root_);
            root_ = root_->left;
        }
        root_ = stack.top();
        stack.pop();
        if( root_->right && !stack.empty() && stack.top() == root_->right ) {
            stack.pop();
            stack.push(root_);
            root_ = root_->right;
        } else {
            delete root_;
            root_ = nullptr;
        }
    }
}
