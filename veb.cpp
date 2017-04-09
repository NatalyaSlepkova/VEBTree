#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include "veb.h"
#include <algorithm>
#include <memory.h>
#include <unordered_map>

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
	unordered_map <unsigned long long, VEBTree<(S / 2)>*> child;
	VEBTree<(S / 2)>* aux;

	VEBTree() : vebmin(NO), vebmax(NO), aux(NULL) {}

	~VEBTree()
	{
		delete aux;
		/*for (unsigned long long i = 0; i < (1ULL << S / 2); i++)
		{
			delete child[i];
		}*/
		for (auto& it : child)
		{
			delete child[it.first];
		}
		//child.clear();
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
				if (child.count(hi) == 0)
				{
					child[hi] = new VEBTree<S / 2>();
				}
				if (child.count(hi) == 0 || child.at(hi)->vebmin == NO)
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
		//this->getMax();
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
		if (child.count(hi) > 0 && lo < child.at(hi)->getMax() && child.at(hi)->getMax() != NO)
		{
			unsigned long long half2 = child.at(hi)->next(lo);
			return (hi << S / 2) + half2;
		}
		else if (aux != NULL)
		{
			unsigned long long half1 = aux->next(hi);
			if (half1 != NO)
			{
				return (half1 << S / 2) + child.at(half1)->getMin();
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
		if (child.count(hi) > 0 && lo > child.at(hi)->getMin() && child.at(hi)->getMin() != NO)
		{
			VEBTree<S / 2>* temp = child.at(hi);
			unsigned long long half2 = temp->prev(lo);
			return (hi << S / 2) + half2;
		}
		else if (aux != NULL)
		{
			unsigned long long half1 = aux->prev(hi);
			if (half1 != NO)
			{
				return (half1 << S / 2) + child.at(half1)->getMax();
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
			//this->getMin();
			if (aux == NULL || aux->vebmin == NO)
			{
				vebmin = vebmax = NO;
				return;
			}
			hi = aux->getMin(), lo = child.at(hi)->getMin();
			vebmin = (hi << S / 2) + lo;
		}
		else
		{
			hi = x >> S / 2, lo = x & ((1ULL << S / 2) - 1);
		}
		child.at(hi)->remove(lo);
		if (child.at(hi)->vebmin == NO)
		{
			child.erase(hi);
			aux->remove(hi);
		}
	}

	/*vector <unsigned long long> go(unsigned long long hi)
	{
		vector <unsigned long long> res;
		res.clear();
		res.push_back(this->vebmin);
		unsigned long long i = 0;
		if (S != 1)
		{
			while (i < (1ULL << S / 2))
			{
				if (child[i] != NULL && child[i]->vebmin != NO)
				{
					vector <unsigned long long> kids = child[i]->go(i);
					for (int j = 0; j < kids.size(); j++)
					{
						res.push_back(kids[j]);
					}
				}
				++i;
			}
		}
		for (int i = 0; i < res.size(); i++)
		{
			res[i] = (hi << S) + res[i];
		}
		return res;
	}*/
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

	void remove(unsigned long long x)
	{
		if (vebmin == vebmax)
		{
			vebmin = vebmax = NO;
		}
		else if (x == vebmin)
		{
			vebmin = vebmax;
		}
		else
		{
			vebmax = vebmin;
		}
	}
	/*vector <unsigned long long> go(unsigned long long hi)
	{
		vector <unsigned long long> res;
		res.push_back((hi << 1) + vebmin);
		return res;
	}*/
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
		//cerr << i << ' ';
		unsigned long long x, ans;
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
		else
		{
			if (st == "N")
			{
				cin >> x;
				ans = t.next(x);
			}
			else if (st == "P")
			{
				cin >> x;
				ans = t.prev(x);
			}
			else if (st == "min")
			{
				ans = t.getMin();
			}
			else if (st == "max")
			{
				ans = t.getMax();
			}
			ans = (ans == NO ? 0 : ans);
			cout << ans << "\n";
		}
		//cout << "\n";
		/*cerr << st << ' ' << x << " : ";
		if (t.aux != NULL)
		{
			vector <unsigned long long> v = t.aux->go(0);
			for (int j = 0; j < v.size(); j++)
			{
				cerr << v[j] << ' ';
			}
		}
		cerr << "\n";*/
	}

	return 0;
}
