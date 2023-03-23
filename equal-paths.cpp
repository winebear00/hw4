#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalHelper (Node* root, int level, int* firstLeafLevel)
{
    if (root==NULL)
    {
        return true; //base case 
    }
    if (root->left==NULL && root->right==NULL)
    {
        if (*firstLeafLevel==0)
        {
            *firstLeafLevel=level; 
            return true; 
        }
        else
        {
            if (*firstLeafLevel==level)
            {
                return true;
            }
            else
            {
                return false; 
            }
        }
    }
    else
    {
        return equalHelper(root->left, level+1, firstLeafLevel)&&equalHelper(root->right, level+1, firstLeafLevel);
    }
}

/**
 * @brief Returns true if all paths from leaves to root are the same length (height),
 *        and false otherwise
 *
 *        Note: this doesn't mean we are checking if the tree is full, but just that
 *        any leaf node (wherever it may exist) has the same length path to the root 
 *        as all others.
 * 
 * @param root Pointer to the root of the tree to check for equal paths
 */
bool equalPaths(Node * root)
{
    int leafLevel=0;
    bool result=equalHelper(root, 0, &leafLevel);
    return result;
}

