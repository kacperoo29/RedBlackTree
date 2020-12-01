#pragma once
#include <iostream>
// remove *parent
template<typename T>
class RedBlackTree
{
private:
	class Node {
	public:
		Node(T keyVal)
			: left(nullptr), right(nullptr), parent(nullptr), color(NodeColor::Red), key(keyVal) { }

		~Node()
		{
			delete left;
			delete right;
		}

		const T& GetKey() { return key; }
		Node* const GetParent() { return parent; }
		Node* const GetGrandparent() { return parent ? parent->parent : nullptr; }
		Node* const GetSibling();
		Node* const GetLeftChild() { return left; }
		Node* const GetRightChild() { return right; }

		static Node* const Find(Node* node, T keyVal);
		static void RecursiveInsert(Node* root, Node* node);
		static void InsertRepairTree(Node* node);
		static bool RemoveNode(Node* node);
		void RemoveRepairTree();

	private:
		enum class NodeColor { Black, Red };

	private:
		Node* parent;		
		Node* left;
		Node* right;
		NodeColor color;

		T key;

		void RotateLeft();
		void RotateRight();

	};

public:
	class Iterator
	{
	public:
		Iterator(Node* root, Node* ptr)
			: root(root), currNode(ptr) {}

		Iterator(const Iterator& iter)
			: root(iter.root), currNode(iter.currNode) {}

		const T& operator*() { return currNode->GetKey(); }
		Iterator& operator++();
		Iterator operator++(int);
		bool operator!= (const Iterator& iter) { return iter.currNode != currNode; }

	private:
		Node* root;
		Node* currNode;

	};

public:
	RedBlackTree()
		: mHead(nullptr) {}

	~RedBlackTree() { delete mHead; }

	void Insert(const T& key);
	bool Remove(const T& key);

	const T& Find(const T& key) const;
	const T& operator[](const T& key) const { return Find(key); }

	const T& Find(const std::string& key) const;
	const T& Find(int key) const;
	const T& operator[](const std::string& key) const;
	const T& operator[](int key) const;

	bool empty() { return !mHead; }
	const Iterator begin();
	const Iterator end() { return Iterator(mHead, nullptr); }

private:
	Node* mHead;

	static T emptyValue;
};

template<typename T>
T RedBlackTree<T>::emptyValue = T();

template<typename T>
inline void RedBlackTree<T>::Insert(const T& key)
{
	Node* node = new Node(key);
	Node::RecursiveInsert(mHead, node);
	Node::InsertRepairTree(node);

	mHead = node;
	while (mHead->GetParent())
		mHead = mHead->GetParent();
}

template<typename T>
inline bool RedBlackTree<T>::Remove(const T& key)
{
	Node* node = Node::Find(mHead, key);
	if (node)
	{
		if (Node::RemoveNode(node))
		{
			Node* iter = mHead;
			while (iter->GetParent())
				iter = iter->GetParent();
			mHead = iter;
		}
		else
			mHead = nullptr;
		return true;
	}
	return false;
}

template<typename T>
inline const T& RedBlackTree<T>::Find(const T& key) const
{
	Node* node = Node::Find(mHead, key);
	return node ? node->GetKey() : emptyValue;
}

template<typename T>
inline typename RedBlackTree<T>::Node* const RedBlackTree<T>::Node::GetSibling()
{
	if (!parent)
		return nullptr;

	if (this == parent->left)
		return parent->right;
	else
		return parent->left;
}

template<typename T>
inline void RedBlackTree<T>::Node::RecursiveInsert(Node* root, Node* node)
{
	if (root != nullptr)
	{
		if (node->key < root->key)
		{
			if (root->left != nullptr)
			{
				RecursiveInsert(root->left, node);
				return;
			}
			else
				root->left = node;
		}
		else
		{
			if (root->right != nullptr)
			{
				RecursiveInsert(root->right, node);
				return;
			}
			else
				root->right = node;
		}
	}

	node->parent = root;
}

template<typename T>
inline void RedBlackTree<T>::Node::InsertRepairTree(Node* node)
{
	Node* p = node->parent;
	Node* g = p ? p->parent : nullptr;
	if (!p)
		node->color = NodeColor::Black;
	else if (p->color == NodeColor::Black)
		return;
	else if (p->GetSibling() && p->GetSibling()->color == NodeColor::Red)
	{
		p->GetSibling()->color = p->color = NodeColor::Black;
		g->color = NodeColor::Red;
		InsertRepairTree(g);
	}
	else
	{
		if (node == p->right && p == g->left)
		{
			p->RotateLeft();
			node = node->left;
		}
		else if (node == p->left && p == g->right)
		{
			p->RotateRight();
			node = node->right;
		}

		p = node->parent;
		g = node->parent->parent;

		if (node == p->left)
			g->RotateRight();
		else
			g->RotateLeft();

		p->color = NodeColor::Black;
		g->color = NodeColor::Red;
	}
}

template<typename T>
inline bool RedBlackTree<T>::Node::RemoveNode(Node* node)
{	
	if (!node->parent || node->left && node->right)
	{
		if (node->left)
		{
			Node* iter = node->left;
			while (iter->right)
				iter = iter->right;
			node->key = iter->key;
			RemoveNode(iter);
		}
		else if (node->right)
		{
			node->key = node->right->key;
			delete node->right;
			node->right = nullptr;
		}
		else
		{
			delete node;
			return false;
		}
	}
	else if (!node->right && !node->left)
	{
		if (node->color == NodeColor::Black)
			node->RemoveRepairTree();

		if (node->parent->left == node)
			node->parent->left = nullptr;
		else
			node->parent->right = nullptr;

		node->parent = node->left = node->right = nullptr;
		delete node;
	}
	else
	{
		Node* next = node->right ? node->right : node->left;
		next->color = NodeColor::Black;
		if (node == node->parent->left)
			node->parent->left = next;
		else
			node->parent->right = next;
		next->parent = node->parent;

		node->parent = node->left = node->right = nullptr;
		delete node;
	}
	return true;
}

template<typename T>
inline void RedBlackTree<T>::Node::RemoveRepairTree()
{
	Node* s = GetSibling();
	if (!s)
		return;
	if (s->color == NodeColor::Black)
	{
		if ((!s->left || s->left->color == NodeColor::Black) &&
			!s->right || s->right->color == NodeColor::Black)
		{
			s->color = NodeColor::Red;
			if (parent->color == NodeColor::Black)
			{
				parent->color = NodeColor::Black;
				parent->RemoveRepairTree();
			}
			else
				parent->color = NodeColor::Black;
		}
		else if (this == parent->right)
		{
			if (!s->left || s->left->color == NodeColor::Black)
			{
				s->RotateLeft();
				s->color = NodeColor::Red;
				s->parent->color = NodeColor::Black;
				RemoveRepairTree();
			}
			else
			{
				parent->RotateRight();
				if (s->left)
					s->left->color = NodeColor::Black;
				if (s->right)
					s->right->color = NodeColor::Black;
				if (s->parent->color == NodeColor::Black)
					s->color = NodeColor::Red;
			}
		}
		else
		{
			if (!s->right || s->right->color == NodeColor::Black)
			{
				s->RotateRight();
				s->color = NodeColor::Red;
				s->parent->color = NodeColor::Black;
				RemoveRepairTree();
			}
			else
			{
				parent->RotateLeft();
				if (s->left)
					s->left->color = NodeColor::Black;
				if (s->right)
					s->right->color = NodeColor::Black;
				if (s->parent->color == NodeColor::Black)
					s->color = NodeColor::Red;
			}
		}
	}
	else
	{
		if (this == parent->left)
		{
			parent->RotateLeft();
			s->color = NodeColor::Black;
			s->left->color = NodeColor::Red;
		}
		else
		{
			parent->RotateRight();
			s->color = NodeColor::Black;
			s->right->color = NodeColor::Red;
		}
		RemoveRepairTree();
	}
}

template<typename T>
inline typename RedBlackTree<T>::Node* const RedBlackTree<T>::Node::Find(Node* node, T keyVal)
{
	if (!node || node->key == keyVal)
		return node ? node : nullptr;
	if (node->key < keyVal)
		return Find(node->right, keyVal);
	return Find(node->left, keyVal);
}

template<typename T>
inline void RedBlackTree<T>::Node::RotateLeft()
{
	//Node* n = right;
	//Node* p = parent;

	//right = n->left;
	//n->left = this;
	//parent = n;

	//if (right)
	//	right->parent = this;

	//if (p)
	//{
	//	if (this == p->left)
	//		p->left = n;
	//	else if (this == p->right)
	//		p->right = n;
	//}

	//n->parent = p;

	Node* A = this;
	Node* B = right;	
	Node* tmp = B->left;	

	if (tmp)
		tmp->parent = A;

	B->parent = A->parent;

	if (A->parent)
	{
		if (A == A->parent->left)
			A->parent->left = B;
		else
			A->parent->right = B;
	}

	B->left = A;
	A->parent = B;
	A->right = tmp;

}

template<typename T>
inline void RedBlackTree<T>::Node::RotateRight()
{
	//Node* n = left;
	//Node* p = parent;

	//left = n->right;
	//n->right = this;
	//parent = n;

	//if (left)
	//	left->parent = this;

	//if (p)
	//{
	//	if (this == p->left)
	//		p->left = n;
	//	else if (this == p->right)
	//		p->right = n;
	//}

	//n->parent = p;

	Node* A = this;
	Node* B = left;
	Node* tmp = B->right;

	if (tmp)
		tmp->parent = A;

	B->parent = A->parent;

	if (A->parent)
	{
		if (A == A->parent->left)
			A->parent->left = B;
		else
			A->parent->right = B;
	}

	B->right = A;
	A->parent = B;
	A->left = tmp;
}

template<typename T>
inline const typename RedBlackTree<T>::Iterator RedBlackTree<T>::begin()
{
	Node* begin = mHead;
	while (begin->GetLeftChild())
		begin = begin->GetLeftChild();
	return Iterator(mHead, begin);
}

template<typename T>
inline typename RedBlackTree<T>::Iterator& RedBlackTree<T>::Iterator::operator++()
{
	Node* p;
	if (!currNode)
	{
		currNode = root;
		while (currNode->GetLeftChild())
			currNode = currNode->GetLeftChild();
	}
	else
	{
		if (currNode->GetRightChild())
		{
			currNode = currNode->GetRightChild();
			while (currNode->GetLeftChild())
				currNode = currNode->GetLeftChild();
		}
		else
		{
			p = currNode->GetParent();
			while (p && currNode == p->GetRightChild())
			{
				currNode = p;
				p = p->GetParent();
			}
			currNode = p;
		}
	}

	return *this;
}

template<typename T>
inline typename RedBlackTree<T>::Iterator RedBlackTree<T>::Iterator::operator++(int)
{
	Iterator tmp = *this;
	++* this;
	return tmp;
}
