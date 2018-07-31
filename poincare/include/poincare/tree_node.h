#ifndef POINCARE_TREE_NODE_H
#define POINCARE_TREE_NODE_H

#include <assert.h>
#include <stddef.h>
#include <strings.h>

#include <stdio.h>

/* What's in a TreeNode, really?
 *  - a vtable
 *  - an identifier
 *  - a reference counter
 */
#define TREE_LOG 1

namespace Poincare {

class TreeNode {
  friend class TreePool;
public:
  virtual ~TreeNode() {}

  // Attributes
  bool isStatic() const;
  virtual size_t size() const = 0;
  int identifier() const { return m_identifier; }
  int retainCount() const { return m_referenceCounter; }
  void setReferenceCounter(int refCount) { //TODO make this method privte with only friends that can access it
    if (isStatic()) {
      // Do not retain static nodes
      return;
    }
    m_referenceCounter = refCount;
  }

  void deepResetReferenceCounter() { //TODO make this method private with friends that can access it
    setReferenceCounter(0);
    for (TreeNode * t : depthFirstChildren()) {
      t->setReferenceCounter(1);
    }
  }

  virtual const char * description() const { return "UNKNOWN";}

  // Allocation failure
  virtual bool isAllocationFailure() const { return false; }
  virtual TreeNode * failedAllocationStaticNode() = 0;
  int allocationFailureNodeIdentifier() {
    return failedAllocationStaticNode()->identifier();
  }

  // Node operations
  void retain() {
    if (isStatic()) {
      // Do not retain static nodes
      return;
    }
    m_referenceCounter++;
  }
  void release(int currentNumberOfChildren);
  void releaseChildren(int currentNumberOfChildren);
  void releaseChildrenAndDestroy(int currentNumberOfChildren);
  void rename(int identifier, bool unregisterPreviousIdentifier);

  // Hierarchy
  virtual TreeNode * parent() const;
  virtual TreeNode * root();
  virtual int numberOfChildren() const = 0;
  virtual void incrementNumberOfChildren(int increment = 1) {} // Do no put an assert(false), we need this method for instance in GridLayout::removeRow
  virtual void decrementNumberOfChildren(int decrement = 1) {} // Do no put an assert(false), we need this method for instance in GridLayout::removeRow
  virtual void eraseNumberOfChildren() {} //TODO Put an assert false //TODO what if somebody i stealing a unary tree's only child ?
  int numberOfDescendants(bool includeSelf) const;
  virtual TreeNode * childAtIndex(int i) const;
  int indexOfChildByIdentifier(int childID) const;
  int indexOfChild(const TreeNode * child) const;
  int indexInParent() const;
  bool hasChild(const TreeNode * child) const;
  bool hasAncestor(const TreeNode * node, bool includeSelf) const;
  bool hasSibling(const TreeNode * e) const;

  template <typename T>
  class Iterator {
    public:
    Iterator(const T * node) : m_node(const_cast<T *>(node)) {}
    T * operator*() { return m_node; }
    bool operator!=(const Iterator& it) const { return (m_node != it.m_node); }
  protected:
    T * m_node;
  };

  template <typename T>
  class Direct {
  public:
    Direct(const T * node) : m_node(const_cast<T *>(node)) {}
    class Iterator : public TreeNode::Iterator<T> {
    public:
      using TreeNode::Iterator<T>::Iterator;
      Iterator & operator++() {
        this->m_node = this->m_node->nextSibling();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node->next()); }
    Iterator end() const { return Iterator(m_node->nextSibling()); }
  private:
    T * m_node;
  };

  template <typename T>
  class DepthFirst {
  public:
    DepthFirst(const TreeNode * node) : m_node(const_cast<TreeNode *>(node)) {}
    class Iterator : public TreeNode::Iterator<T> {
    public:
      using TreeNode::Iterator<T>::Iterator;
      Iterator & operator++() {
        this->m_node = this->m_node->next();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node->next()); }
    Iterator end() const { return Iterator(m_node->nextSibling()); }
  private:
    T * m_node;
  };

  Direct<TreeNode> directChildren() const { return Direct<TreeNode>(this); }
  DepthFirst<TreeNode> depthFirstChildren() const { return DepthFirst<TreeNode>(this); }

  virtual TreeNode * next() const {
    // Simple version would be "return this + 1;", with pointer arithmetics taken care of by the compiler.
    // Unfortunately, we want TreeNode to have a VARIABLE size
    return reinterpret_cast<TreeNode *>(reinterpret_cast<char *>(const_cast<TreeNode *>(this)) + size());
  }

  virtual TreeNode * nextSibling() const {
    int remainingNodesToVisit = numberOfChildren();
    TreeNode * node = const_cast<TreeNode *>(this)->next();
    while (remainingNodesToVisit > 0) {
      remainingNodesToVisit += node->numberOfChildren();
      node = node->next();
      remainingNodesToVisit--;
    }
    return node;
  }

  TreeNode * lastDescendant() const {
    TreeNode * node = const_cast<TreeNode *>(this);
    int remainingNodesToVisit = node->numberOfChildren();
    while (remainingNodesToVisit > 0) {
      node = node->next();
      remainingNodesToVisit--;
      remainingNodesToVisit += node->numberOfChildren();
    }
    return node;
  }

protected:
  TreeNode();

  TreeNode * lastChild() const {
    if (numberOfChildren() == 0) {
      return const_cast<TreeNode *>(this);
    }
    TreeNode * node = next();
    for (int i = 0; i < numberOfChildren() - 1; i++) {
      node = node->nextSibling();
    }
    return node;
  }

  size_t deepSize(int realNumberOfChildren) const;
private:
  int m_identifier;
  int m_referenceCounter;
};

}

#endif
