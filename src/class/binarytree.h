#pragma once
#include <iostream>
#include <string>

using namespace std;

namespace tr {
  template <typename T>
  struct Node {
    unsigned int id;
    T data;
    Node<T> *left = nullptr;
    Node<T> *right = nullptr;
    Node(unsigned int id, T value) : id(id), data(value), left(nullptr), right(nullptr) {}
    ~Node() {}
  };

  template <typename T>
  class BinaryTree {
  private:
    Node<T> *root;

    Node<T> *findPositionHelper(Node<T> *node, unsigned int id) {
      if (node == nullptr) {
        return nullptr;
      }

      if (id < node->id) {
        if (node->left == nullptr) {
          return node;
        } else {
          return findPositionHelper(node->left, id);
        }
      } else if (id > node->id) {
        if (node->right == nullptr) {
          return node;
        } else {
          return findPositionHelper(node->right, id);
        }
      } else {
        return nullptr; // id sudah ada di tree
      }
    }

    Node<T> *searchHelper(Node<T> *node, unsigned int id) const {
      if (node == nullptr || node->id == id) {
        return node;
      }

      if (id < node->id) {
        return searchHelper(node->left, id);
      } else {
        return searchHelper(node->right, id);
      }
    }

    Node<T> *findMin(Node<T> *node) const {
      while (node->left != nullptr) {
        node = node->left;
      }
      return node;
    }

    Node<T> *deleteNodeHelper(Node<T> *node, unsigned int id) {
      if (node == nullptr) {
        return node;
      }

      if (id < node->id) {
        node->left = deleteNodeHelper(node->left, id);
      } else if (id > node->id) {
        node->right = deleteNodeHelper(node->right, id);
      } else {
        if (node->left == nullptr) {
          Node<T> *temp = node->right;
          delete node;
          return temp;
        } else if (node->right == nullptr) {
          Node<T> *temp = node->left;
          delete node;
          return temp;
        }

        Node<T> *temp = findMin(node->right);
        node->id = temp->id;
        node->data = temp->data;
        node->right = deleteNodeHelper(node->right, temp->id);
      }
      return node;
    }

    void inOrderTraversalHelper(Node<T> *node) const {
      if (node == nullptr) {
        return;
      }
      inOrderTraversalHelper(node->left);
      std::cout << node->id << ": " << node->data << " ";
      inOrderTraversalHelper(node->right);
    }

    void preOrderTraversalHelper(Node<T> *node) const {
      if (node == nullptr) {
        return;
      }
      std::cout << node->id << ": " << node->data << " ";
      preOrderTraversalHelper(node->left);
      preOrderTraversalHelper(node->right);
    }

    void postOrderTraversalHelper(Node<T> *node) const {
      if (node == nullptr) {
        return;
      }
      postOrderTraversalHelper(node->left);
      postOrderTraversalHelper(node->right);
      std::cout << node->id << ": " << node->data << " ";
    }

  public:
    BinaryTree() : root(nullptr) {}

    Node<T> *getRoot() { return root; }

    Node<T> *findPosition(unsigned int id) {
      if (root == nullptr) {
        return nullptr;
      }
      return findPositionHelper(root, id);
    }

    bool insert(unsigned int id, T value) {
      Node<T> *parent = findPosition(id);
      if (parent == nullptr) {
        if (root == nullptr) {
          root = new Node<T>(id, value);
          return true;
        } else {
          return false;
        }
      } else {
        if (id < parent->id) {
          parent->left = new Node<T>(id, value);
        } else {
          parent->right = new Node<T>(id, value);
        }
        return true;
      }
    }

    Node<T> *search(unsigned int id) const {
      return searchHelper(root, id);
    }

    void inOrderTraversal() const {
      inOrderTraversalHelper(root);
    }

    void preOrderTraversal() const {
      preOrderTraversalHelper(root);
    }

    void postOrderTraversal() const {
      postOrderTraversalHelper(root);
    }

    void deleteNode(unsigned int id) {
      root = deleteNodeHelper(root, id);
    }
  };
}