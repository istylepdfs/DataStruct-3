/******************************************************************************************
Copyright (c) Bit Software, Inc.(2013), All rights reserved.

Purpose: 二叉树的前/中/后序线索化

Author: xjh

Reviser: yyy

Created Time: 2015-8-6
******************************************************************************************/

#pragma once

enum PointerTag {THREAD, LINK};

template<class T>
struct BinaryTreeNodeThd
{
	T _data;						// 数据
	BinaryTreeNodeThd<T>* _left;	// 左孩子
	BinaryTreeNodeThd<T>* _right;	// 右孩子
	BinaryTreeNodeThd<T>* _parent;	// 父节点（为后序遍历的线索化而生）
	PointerTag	_leftTag;			// 左孩子线索标志
	PointerTag	_rightTag;			// 右孩子线索标志

	BinaryTreeNodeThd(const T& x)
		:_data(x)
		,_left(NULL)
		,_right(NULL)
		,_parent(NULL)
		,_leftTag(LINK)
		,_rightTag(LINK)
	{}
};

// 增加二叉树线索化以后的迭代器
template<class T, class Ref, class Ptr>
struct BinaryTreeIterator
{			
	typedef BinaryTreeNodeThd<T> Node;
	typedef BinaryTreeIterator<T, Ref, Ptr> Self;

	Node* _node;

	BinaryTreeIterator(Node* node)
		:_node(node)
	{}

	Ref operator*()
	{
		return _node->_data;
	}

	Ptr operator->()
	{
		return &(operator*());
	}

	Self& operator++()
	{
		_node = _Next(_node);
		return *this;
	}

	Self operator++(int)
	{
		Self tmp(*this);
		_node = _Next(_node);
		return tmp;
	}

	bool operator != (const Self& s) const
	{
		return _node != s._node;
	}

protected:
	// _Inc
	Node* _Next(Node* node)
	{
		if (node->_rightTag == THREAD)
		{
			return node->_right;
		}
		else
		{
			Node* cur = node->_right;
			while (cur->_leftTag == LINK)
			{
				cur = cur->_left;
			}

			return cur;
		}
	}

	Node* _Prev(Node* node)
	{
		if (node->_leftTag == THREAD)
		{
			return node->_left;
		}
		else
		{
			Node* cur = node;
			while (cur->_rightTag == LINK)
			{
				cur = cur->_right;
			}

			return cur;
		}
	}
};

template<class T>
class BinaryTreeThd
{
public:
	BinaryTreeThd()
		:_root(NULL)
	{}

	BinaryTreeThd(T array[], size_t size)
	{
		int index = 0;
		_CreateTree(_root, array, size, index);
	}

	void InThreading()
	{
		BinaryTreeNodeThd<T>* prev = NULL;
		_InThreading(_root, prev);
	}

	void PrevThreading()
	{
		BinaryTreeNodeThd<T>* prev = NULL;
		_PrevThreading(_root, prev);
	}

	void PostThreading()
	{
		BinaryTreeNodeThd<T>* prev = NULL;
		_PostThreading(_root, prev);
	}

	void InOrder()
	{
		cout<<"InOrder:";
		_InOrder(_root);
		cout<<endl;
	}

	void PrevOrder()
	{
		cout<<"PrevOrder:";
		_PrevOrder(_root);
		cout<<endl;
	}

	void PostOrder()
	{
		cout<<"PostOrder:";
		_PostOrder(_root);
		cout<<endl;
	}

protected:
	// 构建二叉树
	void _CreateTree(BinaryTreeNodeThd<T>*& root, T array1[], size_t size, int& index)
	{
		if (index < size && array1[index] != '#')
		{
			root = new BinaryTreeNodeThd<T>(array1[index]);
			_CreateTree(root->_left, array1, size, ++index);
			_CreateTree(root->_right, array1, size, ++index);

			if (root->_left)
			{
				root->_left->_parent = root;
			}

			if (root->_right)
			{
				root->_right->_parent = root;
			}
		}
	}

	void _InThreading(BinaryTreeNodeThd<T>* cur, BinaryTreeNodeThd<T>*& prev)
	{
		if (cur)
		{
			// 1.线索化左子树
			_InThreading(cur->_left, prev);

			// 2.线索化当前节点的前驱
			if(cur->_left == NULL)
			{
				cur->_leftTag = THREAD;
				cur->_left = prev;
			}
			
			// 3.线索化前驱节点的后继节点
			if (prev && prev->_right == NULL)
			{
				prev->_rightTag = THREAD;
				prev->_right = cur;
			}

			prev = cur;
			
			// 4.线索化右子树
			_InThreading(cur->_right, prev);
		}
	}

	void _PrevThreading(BinaryTreeNodeThd<T>* cur, BinaryTreeNodeThd<T>*& prev)
	{
		if(cur)
		{
			// 1.线索化当前节点的前驱
			if (cur->_left == NULL)
			{
				cur->_leftTag = THREAD;
				cur->_left = prev;
			}

			// 2.线索化前一个节点的后继为当前节点
			if (prev && prev->_right == NULL)
			{
				prev->_rightTag = THREAD;
				prev->_right = cur;
			}

			prev = cur;

			// 只有LINK的节点才需要递归，否则前序遍历的节点已线索化。
			if (cur->_leftTag == LINK)
				_PrevThreading(cur->_left, prev);

			if (cur->_rightTag == LINK)
				_PrevThreading(cur->_right, prev);
		}
	}

	void _PostThreading(BinaryTreeNodeThd<T>* cur, BinaryTreeNodeThd<T>*& prev)
	{
		if(cur)
		{
			// 只有LINK的节点才需要递归，否则前序遍历的节点已线索化。
			if (cur->_leftTag == LINK)
				_PostThreading(cur->_left, prev);

			if (cur->_rightTag == LINK)
				_PostThreading(cur->_right, prev);

			// 1.线索化当前节点的前驱
			if (cur->_left == NULL)
			{
				cur->_leftTag = THREAD;
				cur->_left = prev;
			}

			// 2.线索化前一个节点的后继为当前节点
			if (prev && prev->_right == NULL)
			{
				prev->_rightTag = THREAD;
				prev->_right = cur;
			}

			prev = cur;
		}
	}

	void _InOrder(BinaryTreeNodeThd<T>* cur)
	{
		while(cur)
		{
			// 走左子树，找到第一个要访问的前驱节点
			while (cur && cur->_leftTag == LINK)
			{
				cur = cur->_left;
			}

			// 访问当前节点
			cout<<cur->_data<<" ";

			// 访问连接在一起的后继节点
			while (cur->_rightTag == THREAD && cur->_right)
			{
				cur = cur->_right;
				cout<<cur->_data<<" ";
			}

			cur = cur->_right;
		}
	}

	void _PrevOrder(BinaryTreeNodeThd<T>* cur)
	{
		while(cur)
		{
			// 前序遍历路径上的所经节点
			while (cur)
			{
				// 访问当前节点
				cout<<cur->_data<<" ";
				if (cur->_leftTag == THREAD)
					break;

				cur = cur->_left;
			}

			// 访问连接在一起的后继节点--这样处理有bug
			/*while (cur->_rightTag == THREAD && cur->_right)
			{
				cur = cur->_right;
				cout<<cur->_data<<" ";
			}
			cur = cur->_right;*/

			//
			// 不论是Thread后继节点，还是右子树都直接跳转过去访问
			// 所有节点都当成二叉树的左路节点来访问(子问题的形式看待)
			//
			cur = cur->_right;
		}
	}

	void _PostOrder(BinaryTreeNodeThd<T>* root)
	{
		BinaryTreeNodeThd<T>* cur = root;
		BinaryTreeNodeThd<T>* prev = NULL;

		while (cur)
		{
			// 走左子树，先找到最左节点
			while (cur && cur->_leftTag == LINK)
			{
				cur = cur->_left;
			}

			// 访问后继节点
			while(cur->_rightTag == THREAD)
			{
				cout<<cur->_data<<" ";
				prev = cur;

				cur = cur->_right;
			}

			if (cur == root)
			{
				cout<<cur->_data<<" ";
				break;
			}

			// 如果当前节点的右节点已访问，则访问当前节点并跳到父节点
			while(cur->_right == prev)
			{
				cout<<cur->_data<<" ";
				prev = cur;

				if (cur == root)
					return;

				cur = cur->_parent;		
			}
			
			// 跳转到当前节点的右树，当做子树访问
			if (cur->_rightTag == LINK)
				cur = cur->_right;
		}
	}

private:
	BinaryTreeNodeThd<T>* _root;
};


// 测试线索化二叉树
void TestBinaryTreeThd()
{
	int array1[20] = {1, 2, 3, '#', '#', 4, '#', '#', 5, 6};
	BinaryTreeThd<int> t1(array1, 10);
	t1.InThreading();
	t1.InOrder();

	BinaryTreeThd<int>::Iterator it = t1.Begin();
	while (it != t1.End())
	{
		cout<<*it<<" ";
		++it;
	}
	cout<<endl;

	BinaryTreeThd<int> t2(array1, 10);
	t2.PrevThreading();
	t2.PrevOrder();

	BinaryTreeThd<int> t3(array1, 10);
	t3.PostThreading();
	t3.PostOrder();

	cout<<"==================================="<<endl;

	int array2[15] = {1,2,'#',3,'#','#',4,5,'#',6,'#',7,'#','#',8};
	BinaryTreeThd<int> t4(array2, 15);
	t4.InThreading();
	t4.InOrder();

	BinaryTreeThd<int> t5(array2, 15);
	t5.PostThreading();
	t5.PostOrder();

	BinaryTreeThd<int> t6(array2, 15);
	t6.PrevThreading();
	t6.PrevOrder();
}