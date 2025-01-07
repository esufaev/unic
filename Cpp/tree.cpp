#include <iostream>
#include <memory>

template <typename Key, typename Value>
class tree
{
private:
    struct Node
    {
        Key key;
        Value value;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;

        Node(Key k, Value v) : key(k), value(v), left(nullptr), right(nullptr) {}
    };

    std::shared_ptr<Node> root;

    Node *find_node(const std::shared_ptr<Node> &node, const Key &key)
    {
        if (!node)
            return nullptr;
        if (key == node->key)
            return node.get();
        if (key < node->key)
            return find_node(node->left, key);
        return find_node(node->right, key);
    }

    void add_node(std::shared_ptr<Node> &node, Key key, Value value)
    {
        if (!node)
        {
            node = std::make_shared<Node>(key, value);
            return;
        }
        if (key < node->key)
            add_node(node->left, key, value);
        else
            add_node(node->right, key, value);
    }

    std::shared_ptr<Node> remove_node(std::shared_ptr<Node> node, const Key &key)
    {
        if (!node)
            return nullptr;
        if (key < node->key)
            node->left = remove_node(node->left, key);
        else if (key > node->key)
            node->right = remove_node(node->right, key);
        else
        {
            if (!node->left)
                return node->right;
            if (!node->right)
                return node->left;
            Node *min_larger_node = node->right.get();
            while (min_larger_node->left)
                min_larger_node = min_larger_node->left.get();
            node->key = min_larger_node->key;
            node->value = min_larger_node->value;
            node->right = remove_node(node->right, min_larger_node->key);
        }
        return node;
    }

    int depth(const std::shared_ptr<Node> &node)
    {
        if (!node)
            return 0;
        return 1 + std::max(depth(node->left), depth(node->right));
    }

    void display(const std::shared_ptr<Node> &node, int level)
    {
        if (!node)
            return;
        for (int i = 0; i < level; ++i)
            std::cout << "  ";
        std::cout << node->key << ": " << node->value << "\n";
        display(node->left, level + 1);
        display(node->right, level + 1);
    }

public:
    tree() : root(nullptr) {}

    void add(Key key, Value value)
    {
        add_node(root, key, value);
    }

    void remove(const Key &key)
    {
        root = remove_node(root, key);
    }

    Value *find(const Key &key)
    {
        Node *node = find_node(root, key);
        return node ? &node->value : nullptr;
    }

    int find_depth()
    {
        return depth(root);
    }

    void display()
    {
        display(root, 0);
    }
};

int main()
{
    tree<int, std::string> t;
    t.add(10, "root");
    t.add(5, "l");
    t.add(15, "r");
    t.add(3, "ll");
    t.add(7, "lr");

    std::cout << "Tree structure:\n";
    t.display();

    std::cout << "\nDepth: " << t.find_depth() << "\n";

    if (auto *value = t.find(7))
        std::cout << "\nFound key 7: " << *value << "\n";
    t.remove(5);
    std::cout << "\nTree after removing key 5:\n";
    t.display();

    return 0;
}