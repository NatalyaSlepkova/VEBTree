#include <iostream>
#include <fstream>
#include <vector>
#include "veb.h"
#include <algorithm>
#include <memory.h>

using namespace std;

template <unsigned int S>
struct VEBTree : AbstractVEBTree<S>
{
	unsigned long long vebmin, vebmax;
	VEBTree<(S / 2)> *child[1ULL << S / 2], *aux;

	VEBTree() : vebmin(NO), aux(NULL)
	{
		memset(child, 0, sizeof(child));
	}

	~VEBTree()
	{
		delete aux;
		for (unsigned long long i = 0; i < (1ULL << S / 2); i++)
		{
			delete child[i];
		}
	}

	void add(unsigned long long x)
	{
		if (vebmin == NO)
		{
			vebmin = vebmax = x;
		}
		else
		{
			if (x < vebmin)
			{
				swap(x, vebmin);
			}
			if (x > vebmax)
			{
				vebmax = x;
			}
			unsigned long long hi = x >> (S / 2);
			unsigned long long lo = x & ((1ULL << S / 2) - 1);
			if (child[hi] == NULL)
			{
				child[hi] = new VEBTree<S / 2>();
			}
			if (child[hi]->vebmin == NO)
			{
				if (aux == NULL)
				{
					aux = new VEBTree<S / 2>();
				}
				aux->add(hi);
			}
			child[hi]->add(lo);
		}
	}

	unsigned long long getMin() const
	{
		return vebmin;
	}

	unsigned long long getMax() const
	{
		return vebmax;
	}
	
	unsigned long long next(unsigned long long x) const
	{
		if (x < vebmin)
		{
			return vebmin;
		}
		if (vebmin == NO || x >= vebmax)
		{
			return NO;
		}
		if (S == 1)
		{
			return NO;
		}
		unsigned long long hi, lo;
		hi = x >> (S / 2), lo = x & ((1ULL << S / 2) - 1);
		if (child[hi] != NULL && lo < child[hi]->getMax())
		{
			unsigned long long half2 = child[hi]->next(lo);
			return (hi << S / 2) + half2;
		}
		else if (aux != NULL)
		{
			unsigned long long half1 = aux->next(hi);
			if (half1 != NO)
			{
				return (half1 << S / 2) + child[half1]->getMin();
			}
		}
		return NO;
	}

	unsigned long long prev(unsigned long long x)
	{
		if (x > vebmax)
		{
			return vebmax;
		}
		if (vebmin == NO || x < vebmin)
		{
			return NO;
		}

	}
};

/*template<> 
struct VEBTree<1> : AbstractVEBTree<1> 
{
	void add(unsigned long long x)
	{

	}
	unsigned long long next(unsigned long long x)
	{
		return vebmax == x ? vebmax : NO;
	}
};*/

int main()
{
	VEBTree<8> tree;
	tree.add(5);
	tree.add(3);
	tree.add(7);
	tree.add(2);
	tree.add(14);
	cout << tree.getMin() << ' ' << tree.getMax() << "\n";
	cout << tree.next(4);
	int n;
	cin >> n;
	return 0;
}
