#include<iostream>
#include<vector>
#include<queue>
using namespace std;

/*
m阶B+树：所有数据存在叶结点中，中间结点只是索引。
根结点或者是叶子，或者至少有两棵子树，至多有m棵子树。
除根节点外，所有非终端结点至少有⌈m/2⌉棵子树，至多有m棵子树。
所有叶结点在同一层次。
若一个结点有n棵子树，则必含有n个关键字。
所有叶子结点中包含了全部记录的关键字信息以及这些关键字记录的指针，而且叶结点按关键字大小从小到大顺序链接。
所有非叶子结点可以看作索引部分，结点中只含有其子树的最大（或最小）关键字。

3阶B+树，根节点子树 2-3
其他非叶子节点子树 2-3
*/

struct treenode
{
	int layer;
	int num[4];
	int child[4];
	int nextleaf;
	int size;
	bool isleaf;
	treenode()
	{
		nextleaf = -1;
		size = 0;
		layer = 0;
		isleaf = true;
	}
	void push(int inum)
	{
		int i = size - 1;
		for (; i >= 0; i--)
		{
			if (num[i] > inum)
			{
				num[i + 1] = num[i];
			}
			else
				break;
		}
		num[i + 1] = inum;
		size++;
	}
	void insertchild(int inum,int ichild)
	{
		int i = size - 1;
		for (; i >= 0; i--)
		{
			if (num[i] > inum)
			{
				num[i + 1] = num[i];
				child[i + 1] = child[i];
			}
			else
				break;
		}
		num[i + 1] = inum;
		child[i + 1] = ichild;
		size++;
	}
};
struct meg
{
	int fa;
	int ch;
	int faindex;
};
int root;
int first;
treenode tree[100];
int treesize = 0;
void output1()
{
	queue<int> v;
	v.push(root);//根节点
	while (!v.empty())
	{
		int tmp = v.front();
		v.pop();
		cout << "Layer " << tree[tmp].layer << ":(";
		for (int i = 0; i < tree[tmp].size; i++)
		{
			cout << tree[tmp].num[i];
			if (i != tree[tmp].size - 1)
				cout << ",";
			if (!tree[tmp].isleaf)
			{
				v.push(tree[tmp].child[i]);
			}
		}
		cout << ")" <<endl;
	}
}
void output2(int index)
{
	cout << "Layer " << tree[index].layer << ":(";
	for (int i = 0; i < tree[index].size; i++)
	{
		cout << tree[index].num[i];
		if (i != tree[index].size - 1)
			cout << ",";
	}
	cout << ")" << endl;
	if (!tree[index].isleaf)
	{
		for (int i = 0; i < tree[index].size; i++)
		{
			output2(tree[index].child[i]);
		}
	}
}
void divideroot(int index)//分裂初始根节点
{
	int index1 = ++treesize;
	int index2 = ++treesize;
	tree[index1].push(tree[index].num[0]);
	tree[index1].push(tree[index].num[1]);
	tree[index2].push(tree[index].num[2]);
	tree[index2].push(tree[index].num[3]);
	tree[index].num[0] = tree[index].num[1];
	tree[index].num[1] = tree[index].num[3];
	tree[index].size = 2;
	tree[index].child[0] = index1;
	tree[index].child[1] = index2;
	tree[index].isleaf = false;
	tree[index1].layer = tree[index].layer;
	tree[index2].layer = tree[index].layer;
	tree[index].layer++;
	first = index1;
	tree[index1].nextleaf = index2;
}
meg findleaf(int num)
{
	meg m;
	int fa = root; int current = root;
	while (!tree[current].isleaf)
	{
		for (int i = 0; i < tree[current].size;i++)
		{
			if (tree[current].num[i] >= num)
			{
				if (tree[current].size == 4)//找中间节点
				{
					m.fa = fa;
					m.ch = current;
					return m;
				}
				fa = current;
				current = tree[current].child[i];
				m.faindex = i;
				break;
			}
			if (i == tree[current].size - 1&&tree[current].num[i]<num)//更改末尾节点
			{
				fa = current;
				m.faindex = i;
				tree[current].num[i] = num;
				current = tree[current].child[i];
				break;
			}
		}
	}
	m.fa = fa;
	m.ch = current;
	return m;
}
void dividemid(meg sit)
{
	int index1 = sit.ch;//当前子节点
	int index2 = ++treesize;
	tree[index2].insertchild(tree[index1].num[2], tree[index1].child[2]);
	tree[index2].insertchild(tree[index1].num[3], tree[index1].child[3]);
	tree[index2].isleaf = false;
	tree[index1].size -= 2;
	if (sit.ch == root)//根节点
	{
		tree[index1].layer = tree[root].layer;
		tree[index2].layer = tree[root].layer;
		root = ++treesize;
		tree[root].insertchild(tree[index1].num[1], index1);
		tree[root].insertchild(tree[index2].num[1], index2);
		tree[root].layer = tree[index2].layer + 1;
		tree[root].isleaf = false;
		//for (int i = 0; i <= treesize; i++)
		//{
		//	cout << tree[i].layer<<"size"<<tree[i].size<<"child"<<tree[i].child[0] << endl;
		//}
		return;
	}
	tree[sit.fa].num[sit.faindex] = tree[index1].num[1];
	tree[sit.fa].insertchild(tree[index2].num[1], index2);
	tree[index2].layer = tree[index1].layer;
	if (tree[sit.fa].size == 4)
		dividemid(findleaf(tree[index2].num[1]));
}
void divideleaf(meg sit)
{
	int index1 = sit.ch;//当前子节点
	int index2 = ++treesize;
	tree[index2].push(tree[index1].num[2]);
	tree[index2].push(tree[index1].num[3]);
	tree[index1].size -= 2;

	tree[index2].nextleaf = tree[index1].nextleaf;//更新叶子间的链
	tree[index1].nextleaf = index2;


	tree[sit.fa].num[sit.faindex] = tree[index1].num[1];
	tree[sit.fa].insertchild(tree[index2].num[1], index2);
	tree[index2].layer = tree[index1].layer;

	if (tree[sit.fa].size == 4)
		dividemid(findleaf(tree[index2].num[1]));
}


void insert(int num)
{
	if (treesize == 0)//只有根节点
	{
		if (tree[0].size < 3)
		{
			tree[0].push(num);
		}
		else if (tree[0].size == 3)//拆分根节点
		{
			tree[0].push(num);
			divideroot(0);
		}
	}
	else
	{
		meg sit = findleaf(num);
		tree[sit.ch].push(num);
		if (tree[sit.ch].size == 4)
			divideleaf(sit);
	}
	cout << "层次遍历：" <<endl;
	output1();
	cout << "先序遍历：" << endl;
	output2(root);
}


int main()
{
	int num;
	root = 0;
	first = 0;
	while (cin >> num)
	{
		insert(num);
	}
}