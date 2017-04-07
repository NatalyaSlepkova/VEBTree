#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include "veb.h"
#include <algorithm>
#include <memory.h>

using namespace std;

unsigned long long const NO = -1;

template<unsigned int S>
class AbstractVEBTree {
public:
	virtual void add(unsigned long long x) = 0;
	virtual void remove(unsigned long long x) = 0;
	virtual unsigned long long next(unsigned long long x) const = 0;
	virtual unsigned long long prev(unsigned long long x) const = 0;
	virtual unsigned long long getMin() const = 0;
	virtual unsigned long long getMax() const = 0;
};

template<unsigned int S>
struct VEBTree;

template <unsigned int S>
struct VEBTree : AbstractVEBTree<S>
{
	unsigned long long vebmin, vebmax;
	VEBTree<(S / 2)> *child[1ULL << S / 2], *aux;

	VEBTree() : vebmin(NO), vebmax(NO), aux(NULL)
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
			if (S != 1)
			{
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

	unsigned long long prev(unsigned long long x) const
	{
		unsigned int k = S;
		this->getMin();
		if (x > vebmax)
		{
			return vebmax;
		}
		if (vebmin == NO || x <= vebmin)
		{
			return NO;
		}
		unsigned long long hi, lo, res;
		hi = x >> S / 2, lo = x & ((1 << S / 2) - 1);
		if (child[hi] != NULL && lo > child[hi]->getMin())
		{
			unsigned long long half2 = child[hi]->prev(lo);
			return (hi << S / 2) + half2;
		}
		else if (aux != NULL)
		{
			unsigned long long half1 = aux->prev(hi);
			if (half1 != NO)
			{
				return (half1 << S / 2) + child[half1]->getMax();
			}
		}
		return vebmin;
	}

	void remove(unsigned long long x)
	{
		unsigned long long hi, lo;
		unsigned int k = S;
		if (x == vebmax)
		{
			vebmax = this->prev(vebmax);
		}
		if (x == vebmin)
		{
			this->getMin();
			if (aux == NULL || aux->vebmin == NO)
			{
				vebmin = NO;
				return;
			}
			hi = aux->getMin(), lo = child[hi]->getMin();
			vebmin = (hi << S / 2) + lo;
		}
		else
		{
			hi = x >> S / 2, lo = x & ((1ULL << S / 2) - 1);
		}
		child[hi]->remove(lo);
		if (child[hi]->vebmin == NO)
		{
			aux->remove(hi);
		}
	}
};


template<>
struct VEBTree<1> : AbstractVEBTree<1>
{
	unsigned long long vebmin, vebmax;
	VEBTree<1> *aux;
	VEBTree() : vebmin(NO), vebmax(NO), aux(NULL)
	{
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
		}
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
		return vebmax;
	}
	unsigned long long prev(unsigned long long x) const
	{
		if (x > vebmax)
		{
			return vebmax;
		}
		if (vebmin == NO || x <= vebmin)
		{
			return NO;
		}
		return vebmin;
	}
	unsigned long long getMin() const
	{
		return vebmin;
	}
	unsigned long long getMax() const
	{
		return vebmax;
	}

	void remove(unsigned long long)
	{
		vebmin = vebmax = NO;
	}
};


int main()
{
	ifstream cin("vebtree.in");
	ofstream cout("vebtree.out");
	int n;
	cin >> n;

	VEBTree<16> t;
	for (int i = 0; i < n; i++)
	{
		string st;
		cin >> st;
		unsigned long long x;
		if (st == "A")
		{
			cin >> x;
			t.add(x);
		}
		else if (st == "R")
		{
			cin >> x;
			t.remove(x);
		}
		else if (st == "N")
		{
			cin >> x;
			cout << t.next(x) << "\n";
		}
		else if (st == "P")
		{
			cin >> x;
			cout << t.prev(x) << "\n";
		}
		else if (st == "min")
		{
			cout << t.getMin() << "\n";
		}
		else if (st == "max")
		{
			cout << t.getMax() << "\n";
		}
	}
	//system("pause");
	return 0;
}
