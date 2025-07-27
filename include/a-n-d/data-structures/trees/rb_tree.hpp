#include <cassert>
#include <cstdint>
#include <span>
#include <vector>
//---------------------------------------------------------------------------
using std::byte;
using std::pair;
using std::span;
using std::vector;
//---------------------------------------------------------------------------
namespace data_structures::rb_tree {
//---------------------------------------------------------------------------
enum class Color : uint8_t { RED, BLACK };
enum class Direction : uint8_t { LEFT, RIGHT };
//---------------------------------------------------------------------------
template <typename KeyT, typename ValueT> struct RedBlackNode {
  RedBlackNode<KeyT, ValueT> *children[2] = {nullptr, nullptr};
  RedBlackNode<KeyT, ValueT> *parent = nullptr;
  KeyT key;
  ValueT value;
  Color color = Color::RED;
  //---------------------------------------------------------------------------
  RedBlackNode(KeyT key, ValueT value) : key(key), value(value) {}
};
//---------------------------------------------------------------------------
template <typename KeyT, typename ValueT> class RedBlackTree {
public:
  static const uint64_t kNodeAlignment = sizeof(RedBlackNode<KeyT, ValueT>);
  //---------------------------------------------------------------------------
  /// Default Constructor.
  RedBlackTree() = delete;
  //---------------------------------------------------------------------------
  /// Constructor. Places the RedBlackTree at given location.
  explicit RedBlackTree(span<byte> buffer) : buffer(buffer) {}
  //---------------------------------------------------------------------------
  /// Destructor.
  ~RedBlackTree() = default;
  //---------------------------------------------------------------------------
  /// @brief Inserts a node into the tree.
  /// @param key The key to be inserted.
  /// @param value The value to be inserted.
  /// @returns A pointer to the inserted node.
  RedBlackNode<KeyT, ValueT> *insert(KeyT key, ValueT value) {
    RedBlackNode<KeyT, ValueT> *node = allocateNode(key, value);
    //---------------------------------------------------------------------------
    if (size == 1) {
      root = node;
    } else {
      bool left = false;
      auto parent = findParent(key, left);
      assert(parent != nullptr);
      //---------------------------------------------------------------------------
      node->parent = parent;
      parent->children[static_cast<uint32_t>(left)] = node;
      //---------------------------------------------------------------------------
      if (parent->color == Color::RED) {
        rotate(node);
      }
    }
    //---------------------------------------------------------------------------
    return node;
  }
  //---------------------------------------------------------------------------
  /// @brief Finds a node in the tree, if it exists.
  /// @param key The key to be looked up.
  /// @returns A pointer to the found node.
  RedBlackNode<KeyT, ValueT> *lookup(KeyT key) const {
    vector<RedBlackNode<KeyT, ValueT> *> stack;
    stack.push_back(root);
    //---------------------------------------------------------------------------
    while (stack.size() > 0) {
      auto cur = stack.back();
      stack.pop_back();
      if (cur->key == key) {
        return cur;
      }
      if (cur->key < key) {
        if (cur->children[1] != nullptr)
          stack.push_back(cur->children[1]);
      } else {
        if (cur->children[0] != nullptr)
          stack.push_back(cur->children[0]);
      }
    }
    //---------------------------------------------------------------------------
    return nullptr;
  }
  //---------------------------------------------------------------------------
  /// @brief Prints a visual representation of the tree into the console.
  void print() { print(root); }
  //---------------------------------------------------------------------------
  /// @brief Validates the tree against the Red-Black-Tree properties.
  /// 1. Every node is either red or black.
  /// 2. All null nodes are considered black.
  /// 3. A red node does not have a red child.
  /// 4. Every path from a given node to any of its leaf nodes goes through the
  /// same number of black nodes.
  bool validate() {
    vector<pair<RedBlackNode<KeyT, ValueT> *, int>> stack;
    stack.push_back({root, 1});
    //---------------------------------------------------------------------------
    // The number of black nodes on a path.
    int black_depth = 0;
    //---------------------------------------------------------------------------
    while (stack.size() > 0) {
      auto top = stack.back();
      auto cur = top.first;
      int cur_black_depth = top.second;
      stack.pop_back();
      if (cur == nullptr) {
        // Rule 4.
        if (black_depth == 0)
          black_depth = cur_black_depth;
        if (cur_black_depth != black_depth) {
          std::cout << "Black depth is different: " << black_depth << " vs. "
                    << cur_black_depth << std::endl;
          this->print();
          return false;
        }
        continue;
      }
      if (cur->color != Color::RED && cur->color != Color::BLACK) {
        std::cout << "Color of node with key: \"" << cur->key
                  << "\" is neither black nor red." << std::endl;
        this->print();
        return false; // Rule 1.
      }
      if (cur->color == Color::RED &&
          ((cur->children[0] && cur->children[0]->color == Color::RED) ||
           (cur->children[1] && cur->children[1]->color == Color::RED))) {
        std::cout << "Color of red node with key: \"" << cur->key
                  << "\" has at least one red child." << std::endl;
        this->print();
        return false; // Rule 3.
      }
      auto new_black_depth =
          cur_black_depth + (cur->color == Color::BLACK ? 1 : 0);
      stack.push_back({cur->children[0], new_black_depth});
      stack.push_back({cur->children[1], new_black_depth});
    }
    //---------------------------------------------------------------------------
    return true;
  }

private:
  RedBlackNode<KeyT, ValueT> *findParent(KeyT key, bool &left) const {
    assert(root != nullptr);
    //---------------------------------------------------------------------------
    RedBlackNode<KeyT, ValueT> *pred = nullptr;
    vector<RedBlackNode<KeyT, ValueT> *> stack;
    stack.push_back(root);
    //---------------------------------------------------------------------------
    while (stack.size() > 0) {
      auto cur = stack.back();
      stack.pop_back();
      if (cur->key <= key) {
        if (cur->children[1] != nullptr)
          stack.push_back(cur->children[1]);
        else
          left = true;
      } else {
        if (cur->children[0] != nullptr)
          stack.push_back(cur->children[0]);
        else
          left = false;
      }
      pred = cur;
    }
    //---------------------------------------------------------------------------
    return pred;
  }
  //---------------------------------------------------------------------------
  void rotate(RedBlackNode<KeyT, ValueT> *cur) {
    while (cur != nullptr) {
      //---------------------------------------------------------------------------
      if (cur == root || cur->color == Color::BLACK)
        return;
      //---------------------------------------------------------------------------
      assert(cur->parent != nullptr);
      auto parent = cur->parent;
      if (parent == root) {
        root->color = Color::BLACK;
        return;
      }
      //---------------------------------------------------------------------------
      assert(cur->parent->parent != nullptr);
      auto grandparent = parent->parent;
      if (parent->color == Color::RED) {
        auto dir = static_cast<uint8_t>(getDir(parent));
        auto aunt = grandparent->children[1 - dir];
        if (aunt != nullptr && aunt->color == Color::RED) { // Case 2
          parent->color = Color::BLACK;
          aunt->color = Color::BLACK;
          grandparent->color = Color::RED;
        } else {
          if (cur == parent->children[1 - dir]) { // Case 5
            // Rotate
            parent->children[1 - dir] = cur->children[dir];
            if (parent->children[1 - dir])
              parent->children[1 - dir]->parent = parent;
            cur->children[dir] = parent;
            cur->parent = grandparent;
            parent->parent = cur;
            grandparent->children[dir] = cur;
            // Reassign
            auto temp = cur;
            cur = parent;
            parent = temp;
          }
          // Case 6
          // Rotate
          grandparent->children[dir] = parent->children[1 - dir];
          if (grandparent->children[dir])
            grandparent->children[dir]->parent = grandparent;
          parent->children[1 - dir] = grandparent;
          parent->parent = grandparent->parent;
          if (parent->parent != nullptr) {
            // Make grandgrandparent aware of changes
            grandparent->parent
                ->children[static_cast<uint8_t>(getDir(grandparent))] = parent;
          }
          grandparent->parent = parent;
          if (root->parent != nullptr)
            root = root->parent;
          // Color
          parent->color = Color::BLACK;
          grandparent->color = Color::RED;
        }
      }
      //---------------------------------------------------------------------------
      cur = cur->parent->parent;
    }
  }
  //---------------------------------------------------------------------------q
  Direction getDir(RedBlackNode<KeyT, ValueT> *node) const {
    assert(node != nullptr);
    //---------------------------------------------------------------------------
    if (node->parent == nullptr || node->parent->children[0] == node)
      return Direction::LEFT;
    return Direction::RIGHT;
  }
  //---------------------------------------------------------------------------
  RedBlackNode<KeyT, ValueT> *allocateNode(KeyT key, ValueT value) {
    uint64_t offset = size * kNodeAlignment;
    //---------------------------------------------------------------------------
    if (offset + kNodeAlignment > buffer.size())
      throw std::bad_alloc();
    void *node_ptr = buffer.data() + offset;
    //---------------------------------------------------------------------------
    ++size;
    return new (node_ptr) RedBlackNode<KeyT, ValueT>(key, value);
  }
  //---------------------------------------------------------------------------
  void print(const RedBlackNode<KeyT, ValueT> *node,
             const std::string &prefix = "", bool isLeft = true) {
    if (node == nullptr)
      return;
    //---------------------------------------------------------------------------
    std::cout << prefix << std::endl;
    if (node->parent == nullptr) {
      std::cout << "Root: ";
    } else {
      std::cout << prefix;
      if (isLeft)
        std::cout << "├── L: ";
      else
        std::cout << "└── R: ";
    }
    std::cout << "Key: " << node->key
              << " Color: " << (node->color == Color::RED ? "RED" : "BLACK")
              << std::endl;
    //---------------------------------------------------------------------------
    print(node->children[0], prefix + (isLeft ? "│   " : "    "), true);
    print(node->children[1], prefix + (isLeft ? "│   " : "    "), false);
  }

  span<byte> buffer;
  RedBlackNode<KeyT, ValueT> *root = nullptr;
  uint64_t size = 0;
};
//---------------------------------------------------------------------------
} // namespace data_structures::rb_tree