#ifndef TREEDYN_H_
#define TREEDYN_H_
#include "../dynamic.hpp"

class TreeDyn {
public:
	TreeDyn(int f);
	int *getAllTree();
	void insert(int row, int col);
	int getSize();
private:
	int k;
	dyn::suc_bv tl;
	int findSubMatrix(int row, int col);
	int getValAtpos(int pos);
};

#endif /* TREEDYN_H_ */
