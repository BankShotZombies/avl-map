/**
 * @file avl-map.cpp
 * @author Adam Lonstein (adam.lonstein@digipen.com)
 * @brief This implements a binary search tree, with each node having a corresonding value to create a key-value map. Pairs, or "nodes", can be inserted, deleted, and found
 *        in the map. This also implements both iterators and const iterators for the AVL.
 *        As nodes are inserted or removed, the tree gets rebalanced.
 * @date 03-24-2024
 */

#include "avl-map.h"

// static data members
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator        
		CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::end_it        = CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator(nullptr);

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const  
		CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::const_end_it  = CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const(nullptr);

/**
 * @brief Construct AVL, sets root to null
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap() : mRoot(nullptr)
{
    
}

/**
 * @brief Copy constructor for AVL
 * 
 * @param rhs - copy
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap(const AVLmap& rhs) : mRoot(nullptr)
{
    DeepCopyTree(rhs.mRoot);
}

/**
 * @brief Move constructor. Moves data from rhs map into this map.
 * 
 * @param rhs - data to move
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap(AVLmap&& rhs) : mRoot(std::move(rhs.mRoot)), size_(rhs.size_)
{
    rhs.size_ = 0;
    rhs.mRoot = nullptr;
}

/**
 * @brief Assignment operator
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap& CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap::operator=(const AVLmap& rhs)
{
    if(this != &rhs)
    {
        ClearTree(mRoot);

        DeepCopyTree(rhs.mRoot);
    }

    return *this;
}

/**
 * @brief Move assignment operator. Moves data from rhs into this map via operator=
 * 
 * @param rhs - map to move into this map
 * @return CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap& 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap& CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap::operator=(AVLmap&& rhs)
{
    std::swap(mRoot, rhs.mRoot); // Swap root
    std::swap(size_, rhs.size_); // Swap size

    return *this;
}

/**
 * @brief Destructor. Clears the tree
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::~AVLmap()
{
    ClearTree(mRoot);
}

/**
 * @brief Returns the size (number of nodes) in the map
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
unsigned int CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::size()
{
    return size_;
}

/**
 * @brief Returns the value corresponding to the given key. If a node with the given key does not exist, a node will be inserted into the tree.
 * 
 * @param key - key of value to return
 * @return VALUE_TYPE& - value
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
VALUE_TYPE& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::operator[](KEY_TYPE const& key)
{
    Node* node = FindNode(mRoot, key); // Attempt to find the node

    if(node != nullptr)
    {
        return node->value; // Return the node's value if it exists
    }

    std::stack<Node*> nodes;

    InsertItem(mRoot, mRoot, key, VALUE_TYPE{}, nodes); // If the node did not exist, insert it

    BalanceTree(nodes, true);

    Node* insert = FindNode(mRoot, key); // Find the inserted node

    return insert->value; // Return the inserted node's value
}

////////////////////////////////////////////////////////////
// typename is used to help compiler to parse

/**
 * @brief Returns the begin iterator of the map
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::begin() {
	if (mRoot) return AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator(mRoot->first());
	else       return end_it;
}

/**
 * @brief Returns the end iterator
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::end() {
    return end_it;
}

/**
 * @brief Finds the node of given key and returns as an iterator
 * 
 * @param type - key of iterator to return
 * @return CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const& type)
{
    Node* foundNode = FindNode(mRoot, type);
    return AVLmap_iterator(foundNode);
}

/**
 * @brief Erase a node from the map based off the given iterator
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::erase(AVLmap_iterator it)
{
    if (it == end_it)
        return;
		
    std::stack<Node*> nodes;
    GetVisitedNodes(it.mNode, nodes);

    DeleteItem(it.mNode, it.mNode->key);

    BalanceTree(nodes, false);
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::begin() const {
	if (mRoot) return AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const(mRoot->first());
	else       return const_end_it;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::end() const {
	return const_end_it;
}

/**
 * @brief Finds the node of given key and returns as a const iterator
 * 
 * @param type - key of iterator to return
 * @return CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const& type) const
{
    Node* foundNode = FindNode(mRoot, type);
    return AVLmap_iterator_const(foundNode);
}

/**
 * @brief Returns the height of a node.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
int CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::getdepth(const Node* node) const {
	return node->height;
}

////////////////////////////////////////////////////////////
// do not change this code from here to the end of the file
/* figure out whether node is left or right child or root 
 * used in print_backwards_padded 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
char CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::getedgesymbol(const Node* node) const {
	const Node* parent = node->parent;
	if ( parent == nullptr) return '-';
	else                 return ( parent->left == node)?'\\':'/';
}

/* this is another "ASCII-graphical" print, but using 
 * iterative function. 
 * Left branch of the tree is at the bottom
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
std::ostream& CS280::operator<<(std::ostream& os, AVLmap<KEY_TYPE,VALUE_TYPE> const& map) {
	map.print(os);
	return os;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::print(std::ostream& os, bool print_value ) const {
	if (mRoot) {
		AVLmap<KEY_TYPE,VALUE_TYPE>::Node* b = mRoot->last();
		while ( b ) { 
			int depth = getdepth(b);
			int i;
			/* printf(b); */

			char edge = getedgesymbol(b);
			switch (edge) {
				case '-': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					break;
				case '\\': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << edge << std::endl;
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					break;
				case '/': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					for (i=0; i<depth; ++i) std::printf("       ");
					os << edge << std::endl;
					break;
			}
			b = b->decrement();
		}
	}
	std::printf("\n");
}

/**
 * @brief Finds a node in the tree with the given key.
 * 
 * @param tree - node to start search at
 * @param key - key to find
 * @return the found node
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::Node* CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::FindNode(Node* tree, KEY_TYPE key)
{
    // If the key has been found, return the node.
    if(tree == nullptr || tree->key == key)
    {
        return tree;
    }
    
    // If this node's key is less than the key, search the right subtree
    if(tree->key < key)
    {
        return FindNode(tree->right, key);
    }

    // If this node's key is more than the key, search the left subtree
    return FindNode(tree->left, key);
}

/**
 * @brief Inserts an item into the tree
 * 
 * @param tree - tree to insert
 * @param parentNode - keeps track of parent node
 * @param key - key to insert
 * @param value - value to insert
 * @return CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::Node* 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::Node* CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::InsertItem(Node* tree, Node* parentNode, KEY_TYPE key, VALUE_TYPE value, std::stack<Node*>& nodes)
{
    // Once the spot to insert has been found
    if(tree == nullptr)
    {
        int height = 0;
        
        // Calculate the height of the node
        if(parentNode != nullptr)
        {
            height = parentNode->height + 1;
        }

        // Create the node
        tree = new Node(key, value, parentNode, height, 0, nullptr, nullptr);

        ++size_;

        // If it's the first node, set it to root
        if(mRoot == nullptr)
        {
            mRoot = tree;
        }

        return tree;
    }
    else if(key < tree->key)
    {
        nodes.push(tree);

        // Keep going down the left side of the tree
        tree->left = InsertItem(tree->left, tree, key, value, nodes);
    }
    else if(key > tree->key)
    {
        nodes.push(tree);

        // Keep going down the right side of the tree
        tree->right = InsertItem(tree->right, tree, key, value, nodes);
    }

    return tree;
}

/**
 * @brief Deletes an item from the tree.
 * 
 * @param tree - tree to find item in
 * @param key - key's item to delete
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::DeleteItem(Node* tree, KEY_TYPE key)
{
    if(tree == nullptr)
    {
        return;
    }
    else if(key < tree->key)
    {
        DeleteItem(tree->left, key);
    }
    else if(key > tree->key)
    {
        DeleteItem(tree->right, key);
    }
    else // If this key is the tree's key
    {
        // If the node is a leaf node
        if(tree->left == nullptr && tree->right == nullptr)
        {
            DeleteLeafNode(tree);
        }
        else if(tree->left != nullptr && tree->right != nullptr) // Ihe node to be deleted has both children non-empty.
        {
            Node* pred = tree->decrement(); // Find the node's predecessor under inorder traversal

            // Set the data in the node to the predecessor's data
            tree->value = pred->value;
            tree->key = pred->key;

            // Delete the predecessor's node
            DeleteItem(tree->left, tree->key);
        }
        else // If the node to be deleted has only one empty child.
        {
            Node* child = tree->left ? tree->left : tree->right; // Get the empty child

            // Replace the deleted node with its child
            if(tree != mRoot)
            {
                if(tree == tree->parent->left)
                {
                    tree->parent->left = child;
                }
                else
                {
                    tree->parent->right = child;
                }

                // Update the parent to point to the updated tree.
                child->parent = tree->parent;
            }
            else
            {
                // If it's just the root and a child, set the child to root.
                mRoot = child;

                mRoot->parent = nullptr;
            }

            FreeNode(tree);
        }
    }
}

/**
 * @brief Delete's a leaf node (no left or right pointer) from the tree
 * 
 * @param node - leaf node
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::DeleteLeafNode(Node* node)
{
    // If the node is the root, set root to null
    if(node == mRoot)
    {
        mRoot = nullptr;
    }
    else
    {
        // Update the node's parent pointers
        if(node == node->parent->left)
        {
            node->parent->left = nullptr;
        }
        else
        {
            node->parent->right = nullptr;
        }
    }

    FreeNode(node);
}

/**
 * @brief Frees a node
 * 
 * @param node - node to free
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::FreeNode(Node* node)
{
    delete node;
    node = nullptr;
    --size_;
}

/**
 * @brief Deep copies the tree using preorder traversal
 * 
 * @param node - current node (should be called with root)
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::DeepCopyTree(Node* root)
{
    if(root == nullptr)
        return;

    (*this)[root->key] = root->value; // Copy the current node
    DeepCopyTree(root->left); // Copy the current node's left
    DeepCopyTree(root->right); // Copy the current node's right
}

/**
 * @brief Clears the tree using preorder traversal
 * 
 * @param node - current node (should be called with root)
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::ClearTree(Node* node)
{
    if(node == nullptr)
        return;

    Node* tempLeft = node->left;
    Node* tempRight = node->right;

    FreeNode(node); // Free the node

    ClearTree(tempLeft); // Free the left node
    ClearTree(tempRight); // Free the right node

    mRoot = nullptr;
}

/**
 * @brief Balances the tree. Finds the correct node to be rotated.
 * 
 * @param nodes - the nodes that were visited up until the recently inserted/deleted node.
 * @param inserting - whether or not node is being inserted (if false then deleted)
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::BalanceTree(std::stack<Node*>& nodes, bool inserting)
{
    // Go through the stack
    while(!nodes.empty())
    {
        // Remove the top node pointer from the stack and call it y.
        Node* y = nodes.top();
        nodes.pop();

        Node* leftSubtree = y->left;
        Node* rightSubtree = y->right;

        // Find the balance of y
        int balance = GetSubtreeBalance(y);
        int leftSubtreeHeight = GetSubtreeHeight(leftSubtree);
        int rightSubtreeHeight = GetSubtreeHeight(rightSubtree);

        // If the height of left and right subtree are equal or differ by no more than 1 (hence, balanced), go to the next node on the stack
        if(std::abs(leftSubtreeHeight - rightSubtreeHeight) <= 1)
        {
            continue;
        }

        if(balance > 1)
        {
            if(GetSubtreeHeight(leftSubtree->left) >= GetSubtreeHeight(leftSubtree->right))
            {
                RotateRight(y);
            }
            else
            {
                RotateLeft(leftSubtree);
                RotateRight(y);
            }
        }

        if(balance < -1)
        {
            if(GetSubtreeHeight(rightSubtree->right) >= GetSubtreeHeight(rightSubtree->left))
            {
                RotateLeft(y);
            }
            else
            {
                RotateRight(rightSubtree);
                RotateLeft(y);
            }
        }

        if(inserting)
            break;
    }
}

/**
 * @brief Returns the height of a subtree.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
int CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::GetSubtreeHeight(Node* node)
{
    if(node == nullptr)
        return -1;
    else
        return (1 + std::max(GetSubtreeHeight(node->left), GetSubtreeHeight(node->right)));
}

/**
 * @brief Returns the balance of a subtree.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
int CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::GetSubtreeBalance(Node* node)
{
    if(node == nullptr)
        return -1;
    else
        return GetSubtreeHeight(node->left) - GetSubtreeHeight(node->right);
}

/**
 * @brief Rotates a node right.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::RotateRight(Node*& node)
{
    // If the root is being rotated
    if(node == mRoot)
    {
        RotateRootRight();
        return;
    }

    Node* temp = node;

    Node* nodeParent = node->parent;

    // Update the node parent child pointer
    if(node == nodeParent->left)
    {
        nodeParent->left = node->left;
    }
    else
    {
        nodeParent->right = node->left;
    }

    // Promote the left child
    node = node->left;

    // Update the promoted node's parent pointer
    node->parent = nodeParent;

    // Update the old node's parent pointer
    temp->parent = node;

    // Update the old node's child pointer
    temp->left = node->right;

    // Update the old node's child's parent pointer
    if(temp->left)
        temp->left->parent = temp;

    // Update the promoted node's child pointer
    node->right = temp;

    // Update all node heights
    UpdateSubtreeHeights(node);
}

/**
 * @brief Rotates a node left.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::RotateLeft(Node*& node)
{
    // If the root is being rotated
    if(node == mRoot)
    {
        RotateRootLeft();
        return;
    }

    Node* temp = node;

    Node* nodeParent = node->parent;

    // Update the node parent child pointer
    if(node == nodeParent->left)
    {
        nodeParent->left = node->right;
    }
    else
    {
        nodeParent->right = node->right;
    }

    // Promote the right child
    node = node->right;

    // Update the promoted node's parent pointer
    node->parent = nodeParent;

    // Update the old node's parent pointer
    temp->parent = node;

    // Update the old node's child pointer
    temp->right = node->left;

    // Update the old node's child's parent pointer
    if(temp->right)
        temp->right->parent = temp;

    // Update the promoted node's child pointer
    node->left = temp;

    // Update all node heights
    UpdateSubtreeHeights(node);
}

/**
 * @brief Rotates the root node right.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::RotateRootRight()
{
    Node* temp = mRoot;

    Node* newRoot = mRoot->left;

    // Promote the left child as the root
    mRoot = mRoot->left;

    // Update the root node's parent pointer
    mRoot->parent = nullptr;

    // Update the old root's parent pointer
    temp->parent = newRoot;

    // Update the old root's child pointer
    temp->left = newRoot->right;

    // Update the old root's child's parent pointer
    if(temp->left)
        temp->left->parent = temp;

    // Update the promoted root's child pointer
    newRoot->right = temp;

    // Update all node heights
    UpdateSubtreeHeights(mRoot);
}


/**
 * @brief Rotates the root node left.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::RotateRootLeft()
{
    Node* temp = mRoot;

    Node* newRoot = mRoot->right;

    // Promote the right child as the root
    mRoot = mRoot->right;

    // Update the root node's parent pointer
    mRoot->parent = nullptr;

    // Update the old root's parent pointer
    temp->parent = newRoot;

    // Update the old root's child pointer
    temp->right = newRoot->left;

    // Update the old root's child's parent pointer
    if(temp->right)
        temp->right->parent = temp;

    // Update the promoted root's child pointer
    newRoot->left = temp;

    // Update all node heights
    UpdateSubtreeHeights(mRoot);
}

/**
 * @brief Traverses a subtree through preorder traversal and updates the height values.
 * @param subtree - subtree to traverse
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::UpdateSubtreeHeights(Node* subtree)
{
    if(subtree == nullptr)
        return;

    subtree->height = subtree->parent ? subtree->parent->height + 1 : 0;
    UpdateSubtreeHeights(subtree->left);
    UpdateSubtreeHeights(subtree->right);
}

/**
 * @brief Adds all parent pointers all the way up to the root to the given stack.
 * 
 * @param node - the node to find visited nodes for
 * @param stack - stack to add nodes to
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::GetVisitedNodes(Node* node, std::stack<Node*>& stack)
{
    if(!node->parent)
        return;

    // Start with the nodes parents
    Node* parentWalker = node->parent;

    while(parentWalker != nullptr)
    {
        // Add parents to stack
        stack.push(parentWalker);

        // Go to next parent
        parentWalker = parentWalker->parent;
    }
}

/**
 * @brief Node constructor
 * 
 * @param k - key
 * @param val - value
 * @param p - parent
 * @param h - height
 * @param b - balance
 * @param l - left
 * @param r - right
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::Node(KEY_TYPE k, VALUE_TYPE val, Node* p, int h, int b, Node* l, Node* r)
    : key(k), value(val), height(h), balance(b), parent(p), left(l), right(r)
{

}

/**
 * @brief Returns the node's key
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
KEY_TYPE const& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::Key() const
{
    return key;
}

/**
 * @brief Returns the node's value
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
VALUE_TYPE& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::Value()
{
    return value;
}

/**
 * @brief Returns the node as from left from this node as possible.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::first()
{
    if(left == nullptr)
    {
        return this;
    }
    else
    {
        return left->first();
    }
}

/**
 * @brief Returns the node as from right from this node as possible.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::last()
{
    if(right == nullptr)
    {
        return this;
    }
    else
    {
        return right->last();
    }
}

/**
 * @brief Returns the next key in the tree after this node.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::increment()
{
    // If the right exists, get the minimum value after this node's value
    if(right != nullptr)
    {
        return right->first();
    }

    if(parent == nullptr)
    {
        return nullptr;
    }

    Node* walker = this;
    Node* parentWalker = parent;
    while(parentWalker != nullptr && parentWalker->right == walker)
    {
        walker = parentWalker;
        parentWalker = parentWalker->parent;
    }

    return parentWalker;
}

/**
 * @brief Returns the previous key in the tree before this node.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::decrement()
{
    // If the right exists, get the maximum value before this node's value
    if(left != nullptr)
    {
        return left->last();
    }
    
    if(parent != nullptr && parent->right == this)
    {
        return parent;
    }

    Node* walker = this;
    Node* parentWalker = parent;
    while(parentWalker != nullptr && parentWalker->left == walker)
    {
        walker = parentWalker;
        parentWalker = parentWalker->parent;
    }

    return parentWalker;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::print(std::ostream& os, bool print_value) const
{
    for (int i=0; i < height; ++i) std::printf("       ");

    os << key;

    if ( print_value ) { os << " -> " << value; }

    os << std::endl;
}

/**
 * @brief Constructor for iterator
 * 
 * @param p - node for iterator
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::AVLmap_iterator(Node* p) : mNode(p)
{

}

/**
 * @brief Copy constructor for iterator
 * 
 * @param rhs - iterator to copy
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::AVLmap_iterator(const AVLmap_iterator& rhs) : mNode(rhs.mNode)
{
    
}

/**
 * @brief Assignment operator for iterator.
 * 
 * @param rhs - iterator to assign this to.
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator=(const AVLmap_iterator& rhs)
{
    mNode = rhs.mNode;
    return *this;
}

/**
 * @brief Prefix increment operator for iterators
 * 
 * @return CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator& 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator++()
{
    mNode = mNode->increment();
    return *this;
}

/**
 * @brief Postfix increment operator for iterators
 * 
 * @return CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator& 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator++(int)
{
    AVLmap_iterator temp = *this;
    mNode = mNode->increment();
    return temp;
}

/**
 * @brief Dereference operator for iterator
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator*()
{
    return *mNode;
}

/**
 * @brief Arrow operator for iterator
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator->()
{
    return mNode;
}

/**
 * @brief Not equal operator for iterator.
 * @param rhs - check value against
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator!=(const AVLmap_iterator& rhs)
{
    return mNode != rhs.mNode;
}

/**
 * @brief Equal operator for iterator.
 * @param rhs - check value against
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator==(const AVLmap_iterator& rhs)
{
    return mNode == rhs.mNode;
}



template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::AVLmap_iterator_const(Node* p) : mNode(p)
{

}

template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::AVLmap_iterator_const(const AVLmap_iterator_const& rhs) : mNode(rhs.mNode)
{
    
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::operator=(const AVLmap_iterator_const& rhs)
{
    mNode = rhs.mNode;
    return *this;
}

/**
 * @brief Prefix increment operator for const iterators
 * 
 * @return CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const& 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::operator++()
{
    mNode = mNode->increment();
    return *this;
}

/**
 * @brief Postfix increment operator for const iterators
 * 
 * @return CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const& 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::operator++(int)
{
    AVLmap_iterator temp = *this;
    mNode = mNode->increment();
    return temp;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node const& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::operator*()
{
    return *mNode;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node const* CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::operator->()
{
    return mNode;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::operator!=(const AVLmap_iterator_const& rhs)
{
    return mNode != rhs.mNode;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::operator==(const AVLmap_iterator_const& rhs)
{
    return mNode == rhs.mNode;
}