#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rightRotate(AVLNode<Key, Value>* node);
    void leftRotate(AVLNode<Key, Value>* node); 
    Node<Key, Value>* insertHelper(Node<Key, Value>* cur, Node<Key, Value>* parent, AVLNode<Key, Value>** loc, const std::pair<const Key, Value>& keyValuePair);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix (AVLNode<Key, Value>* node, int8_t diff);
    AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current);
    void removeHelper(AVLNode<Key, Value>* ptr, const Key& key, uint8_t diff, AVLNode<Key, Value>** parentLoc);


};

template<class Key, class Value>
void AVLTree<Key, Value>::rightRotate(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* leftChild=node->getLeft(); //take the left child 
    AVLNode<Key, Value>* parent=node->getParent(); //get the parent
    AVLNode<Key, Value>* gradRightChild=leftChild->getRight(); //get the child's right child 
    if (node==this->root_) //node is the root node 
    {
        this->root_=leftChild; 
        this->root_->setParent(nullptr);
    }
    else //node is not root node 
    {
        if (parent->getLeft()==node) //node is the left child of its parent 
        {
            parent->setLeft(leftChild);
        }
        else if (parent->getRight()==node) //node is the right child of its parent 
        {
            parent->setRight(leftChild);
        }
        leftChild->setParent(parent); 
    }
    if (gradRightChild!=nullptr)
    {
        gradRightChild->setParent(node);
    }
    node->setParent(leftChild);
    leftChild->setRight(node);
    node->setLeft(gradRightChild); 
}

template<class Key, class Value>
void AVLTree<Key, Value>::leftRotate(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightChild=node->getRight(); //take the right child 
    AVLNode<Key, Value>* parent=node->getParent(); //get the parent
    AVLNode<Key, Value>* gradLeftChild=rightChild->getLeft(); //get the child's left child 
    if (node==this->root_) //node is the root node 
    {
        this->root_=rightChild; 
        this->root_->setParent(nullptr);
    }
    else //node is not root node 
    {
        if (parent->getLeft()==node) //node is the left child of its parent 
        {
            parent->setLeft(rightChild);
        }
        else if (parent->getRight()==node) //node is the right child of its parent 
        {
            parent->setRight(rightChild);
        }
        rightChild->setParent(parent); 
    }
    if (gradLeftChild!=nullptr)
    {
        gradLeftChild->setParent(node);
    }
    node->setParent(rightChild);
    rightChild->setLeft(node);
    node->setRight(gradLeftChild); 
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node)
{
    if (parent==nullptr || parent->getParent()==nullptr)
    {
        return; 
    }
    AVLNode<Key, Value>* grandp=parent->getParent(); 
    if (grandp->getLeft()==parent) //if parent is the left child of grandparent 
    {
        grandp->updateBalance(-1); 
        if (grandp->getBalance()==0)
        {
            return; 
        }
        else if (grandp->getBalance()==-1)
        {
            insertFix(grandp, parent); 
        }
        else if (grandp->getBalance()==-2)
        {
            if (grandp->getLeft()->getLeft()==node) //zig-zig case 
            {
                rightRotate(grandp); 
                parent->setBalance(0);
                grandp->setBalance(0); 
            }
            else if (grandp->getLeft()->getRight()==node) //zig-zag case 
            {
                leftRotate(parent);
                rightRotate(grandp); 
                if (node->getBalance()==-1) //case 3a
                {
                    parent->setBalance(0);
                    grandp->setBalance(1);
                    node->setBalance(0);
                }
                else if (node->getBalance()==0) //case 3b 
                {
                    parent->setBalance(0);
                    grandp->setBalance(0);
                    node->setBalance(0);   
                }
                else if (node->getBalance()==1) //case 3c 
                {
                    parent->setBalance(-1);
                    grandp->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }
    else if (grandp->getRight()==parent) 
    {
        grandp->updateBalance(1); 
        if (grandp->getBalance()==0)
        {
            return; 
        }
        else if (grandp->getBalance()==1)
        {
            insertFix(grandp, parent); 
        }
        else if (grandp->getBalance()==2)
        {
            if (grandp->getRight()->getRight()==node) //zig-zig case 
            {
                leftRotate(grandp); 
                parent->setBalance(0);
                grandp->setBalance(0); 
            }
            else if (grandp->getRight()->getLeft()==node) //zig-zag case 
            {
                rightRotate(parent);
                leftRotate(grandp); 
                if (node->getBalance()==1) //case 3a
                {
                    parent->setBalance(0);
                    grandp->setBalance(-1);
                    node->setBalance(0);
                }
                else if (node->getBalance()==0) //case 3b 
                {
                    parent->setBalance(0);
                    grandp->setBalance(0);
                    node->setBalance(0);   
                }
                else if (node->getBalance()== -1) //case 3c 
                {
                    parent->setBalance(1);
                    grandp->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }

}

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO (Complete)
    if (this->root_==nullptr) 
    {
        this->root_ = new AVLNode <Key, Value> (new_item.first, new_item.second, nullptr);
        //this->root_->setBalance(0);

        return; 
    }
    AVLNode<Key, Value>* insertLoc=nullptr; 
    insertHelper(this->root_, nullptr, &insertLoc, new_item); 
    if (insertLoc!=nullptr)
    {
        AVLNode<Key, Value>* parent=insertLoc->getParent(); 
        if (parent->getBalance()==-1 || parent->getBalance()==1)
        {
            parent->setBalance(0); 
        }
        else if (parent->getBalance()==0)
        {
            if (parent->getLeft()==insertLoc) //update balance to -1 
            {
                parent->updateBalance(-1); 

            }
            else if (parent->getRight()==insertLoc) //update balance to 1 
            {
                parent->updateBalance(1);
            }
            insertFix(parent, insertLoc); 
        }
    }

}

template<class Key, class Value> 
Node<Key, Value>* AVLTree<Key, Value>::insertHelper(Node<Key, Value>* cur, Node<Key, Value>* parent, AVLNode<Key, Value>** loc, const std::pair<const Key, Value>& keyValuePair)
{
    if (cur==nullptr)
    {
        *loc = new AVLNode<Key, Value>(keyValuePair.first, keyValuePair.second, nullptr); 
        (*loc)->setParent(parent);
        return *loc;
    }
    else if (cur->getKey()>keyValuePair.first)
    {
        cur->setLeft(insertHelper(cur->getLeft(), cur, loc, keyValuePair)); 
    }
    else if (cur->getKey()<keyValuePair.first)
    {
        cur->setRight(insertHelper(cur->getRight(), cur, loc, keyValuePair));
    }
    else 
    {
        cur->setValue(keyValuePair.second); 
    }
    return cur; 

}

template<class Key, class Value>
void AVLTree<Key, Value>::removeHelper(AVLNode<Key, Value>* ptr, const Key& key, uint8_t diff, AVLNode<Key, Value>** parentLoc)
{
    while (ptr!=nullptr) //find the key 
    {
        if (ptr->getKey()>key)
        {
            ptr=ptr->getLeft(); 
        }
        else if (ptr->getKey()<key)
        {
            ptr=ptr->getRight();
        }
        else 
        {
            break; //find the key 
        }
    }
    if (ptr==nullptr)
    {
        return; 
    }
    AVLNode<Key, Value>* parent=ptr->getParent(); //get this key's parent
    if (parent!=nullptr) 
    {
        if (ptr==parent->getLeft()) //n is p's left child 
        {
            parent->updateBalance(1);
        }
        else if (ptr==parent->getRight()) //n is p's right child 
        {
            parent->updateBalance(-1);
        }
    }
    if (ptr->getLeft()==nullptr && ptr->getRight()==nullptr) //case1: no children 
    {
        if (ptr== (this->root_)) //when the key is the root node
        {
            (this->root_)=nullptr; 
        }
        else
        {
            if (parent->getRight()==ptr) //means the ptr is the parent's right child 
            {
                //cout<<"enter here"<<endl; 
                parent->setRight(nullptr); 
            }
            else if (parent->getLeft()==ptr) //means the ptr is the parent's left child 
            {
                parent->setLeft(nullptr); 
            }
        }
        delete ptr; 
    }
    else if (ptr->getLeft()==nullptr && ptr->getRight()!=nullptr) //case2: 1 child-right child 
    {
        Node<Key, Value>* rightChild=ptr->getRight(); //get the right child c 
        
        if (ptr==(this->root_))
        {
            rightChild->setParent(nullptr);
            (this->root_)=rightChild; 
        }
        else 
        {
            if (parent->getLeft()==ptr) //it means n is p's left child 
            {
                parent->setLeft(rightChild);
                rightChild->setParent(parent); 
            }
            else //it means n is p's right child 
            {
                parent->setRight(rightChild);
                rightChild->setParent(parent); 
            }
        }
        delete ptr;
    }
    else if (ptr->getLeft()!=nullptr && ptr->getRight()==nullptr) //case2: n has only left child  
    {
        Node<Key, Value>* leftChild=ptr->getLeft(); //get the left child c 
        
        if (ptr==(this->root_))
        {
            leftChild->setParent(nullptr);
            (this->root_)=leftChild; 
        }
        else 
        {
            if (parent->getLeft()==ptr) //it means n is p's left child 
            {
                parent->setLeft(leftChild);
                leftChild->setParent(parent); 
            }
            else //it means n is p's right child 
            {
                parent->setRight(leftChild);
                leftChild->setParent(parent); 
            }
        }
        *parentLoc=parent; 
        delete ptr;   
    }
    else if (ptr->getLeft()!=nullptr && ptr->getRight()!=nullptr) //case 3: have both children 
    {
        AVLNode<Key, Value>* pred=predecessor(ptr);
        nodeSwap(pred, ptr); 
        removeHelper(ptr, key, diff, parentLoc); 
    }
}
/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    uint8_t diff=0; 
    AVLNode<Key, Value>* parent = nullptr;
    removeHelper(static_cast<AVLNode<Key, Value>*>(this->root_), key, diff, &parent);
    removeFix(parent, diff);
}

template<class Key, class Value> 
void AVLTree<Key, Value>:: removeFix (AVLNode<Key, Value>* node, int8_t diff)
{
    if (node==nullptr)
    {
        return; 
    }
    AVLNode<Key, Value>* parent= node->getParent(); 
    int8_t ndiff; 
    if (parent!=nullptr)
    {
        if (parent->getLeft()==node)
        {
            ndiff=1;
        }
        else if (parent->getRight()==node)
        {
            ndiff=-1; 
        }
    }
    if (diff==-1)
    {
        if ((node->getBalance()+diff) == -2) //case 1
        {
            AVLNode<Key, Value>* child=node->getLeft(); 
            if (child->getBalance()== -1) //case 1a, zig-zig case
            {
                rightRotate(node);
                node->setBalance(0);
                child->setBalance(0);
                removeFix(parent, ndiff); 
            }
            else if (child->getBalance()==0) //case 1b, zig-zig case 
            {
                rightRotate(node);
                node->setBalance(-1);
                child->setBalance(1);
            }
            else if (child->getBalance()==1) //case 1c, zig-zag case 
            {
                AVLNode<Key, Value>* grandchild=child->getRight(); 
                leftRotate(child);
                rightRotate(node); 
                if (grandchild->getBalance()==1)
                {
                    node->setBalance(0);
                    child->setBalance(-1);
                    grandchild->setBalance(0);
                }
                else if (grandchild->getBalance()==0)
                {
                    node->setBalance(0);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                else if (grandchild->getBalance()==-1)
                {
                    node->setBalance(1);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                removeFix(parent, ndiff); 
            }
        }
        else if ((node->getBalance())+diff==-1) //case 2
        {
            node->setBalance(-1);
        }
        else if ((node->getBalance())+diff==0) //case 3
        {
            node->setBalance(0);
            removeFix(parent, ndiff); 
        }
    }
    else if (diff==1)
    {
        if ((node->getBalance())+diff==2) //case 1
        {
            AVLNode<Key, Value>* child=node->getRight(); 
            if (child->getBalance()== 1) //case 1a, zig-zig case
            {
                leftRotate(node);
                node->setBalance(0);
                child->setBalance(0);
                removeFix(parent, ndiff); 
            }
            else if (child->getBalance()==0) //case 1b, zig-zig case 
            {
                leftRotate(node);
                node->setBalance(1);
                child->setBalance(-1);
            }
            else if (child->getBalance()== -1) //case 1c, zig-zag case 
            {
                AVLNode<Key, Value>* grandchild=child->getLeft(); 
                rightRotate(child);
                leftRotate(node); 
                if (grandchild->getBalance()== -1)
                {
                    node->setBalance(0);
                    child->setBalance(1);
                    grandchild->setBalance(0);
                }
                else if (grandchild->getBalance()==0)
                {
                    node->setBalance(0);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                else if (grandchild->getBalance()== 1)
                {
                    node->setBalance(-1);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                removeFix(parent, ndiff); 
            }
        }
        else if ((node->getBalance()+diff)== 1) //case 2
        {
            node->setBalance(1);
        }
        else if ((node->getBalance()+diff)==0) //case 3
        {
            node->setBalance(0);
            removeFix(parent, ndiff); 
        }

    }
}

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current)
{
    // TODO (complete)
    if (current==nullptr) //when the current node is nullptr 
    {
        return nullptr; 
    }
    if (current->getLeft()!=nullptr) //if left child exists 
    {
        current=current->getLeft(); 
        while (current->getRight()!=nullptr) 
        {
            current=current->getRight(); 
        }
        return current; 
    }
    else //when the left child does not exist 
    {
        while (current->getParent()!= nullptr && current->getParent()->getRight()!=current) 
        {
            current=current->getParent();
        }
        current=current->getParent(); //find it, and grab its parent 
        return current; 
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
