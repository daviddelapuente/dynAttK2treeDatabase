// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 * spsi.hpp
 *
 *  Created on: Oct 19, 2015
 *      Author: nico
 *
 *  Searchable partial sums with insert.
 *
 *  represents a vector of integers I_0, ..., I_(n-1) >= 0
 *  supports random access, set, partial sum, search, insert, update (increment/decrement).
 *
 *  The structure is a B+-tree. This improves data locality and space efficiency.
 *
 */

#ifndef INTERNAL_SPSI_HPP_
#define INTERNAL_SPSI_HPP_

#include "includes.hpp"

namespace dyn{

template <class Container> class spsi_reference{

public:

	spsi_reference(Container &c, uint64_t idx): _spsi(c), _idx(idx) {}

    operator uint64_t() {
        return _spsi.at(_idx);
    }

    spsi_reference const&operator=(uint64_t v) const {

		_spsi.set(_idx, v);

        return *this;
    }

    spsi_reference const&operator=(spsi_reference& ref) const {

		_spsi.set(_idx, uint64_t(ref));

        return *this;
    }

    //++spsi[i]
    spsi_reference const&operator++() const {

		_spsi.increment(_idx,1);

		return *this;

    }

    //spsi[i]++
    spsi_reference const operator++(int) const {

    	spsi_reference copy(*this);

		++(*this);

		return copy;

    }

    //--spsi[i]
    spsi_reference const&operator--() const {

		_spsi.decrement(_idx,1);

		return *this;

    }

    //spsi[i]--
    spsi_reference const operator--(int) const {

    	spsi_reference copy(*this);

		--(*this);

		return copy;

    }

    spsi_reference const&operator+=(uint64_t d) const {

		_spsi.increment(_idx,d);

		return *this;

    }

    spsi_reference const&operator-=(uint64_t d) const {

		_spsi.decrement(_idx,d);

		return *this;

    }

private:

	Container &_spsi;
	uint64_t _idx;

};

template< 	class leaf_type, 	//underlying representation of the integers
			uint32_t B_LEAF,	//number of integers m allowed for a
								//leaf is B_LEAF <= m <= 2*B_LEAF (except at the beginning)
			uint32_t B			//Order of the tree: number of elements n in each internal node
								//is always B <= n <= 2B+1  (except at the beginning)
			>
class spsi{

public:

	/*
	 * copy constructor
	 */
	spsi ( const spsi & sp){

		root = new node(*sp.root);

	}

	/*
	 * copy operator
	 */
	void operator=( const spsi & sp){

		root->free_mem();
		delete root;

		root = new node(*sp.root);

	}

    using spsi_ref = spsi_reference<spsi>;

	/*
	 * create empty spsi. Input parameters are not used (legacy option). This structure
	 * does not need a max size, and width is automatically detected.
	 */
	spsi(uint64_t max_len = 0, uint64_t width = 0){

		assert(root==NULL);
		root = new node();

	}

	~spsi(){

		root->free_mem();

		assert(root!=NULL);

		delete root;
		root = NULL;

	}

	/*
	 * high-level access to the SPSI. Supports assign, access,
	 * increment (++, +=), decrement (--, -=)
	 */
	spsi_ref operator[](uint64_t i){

		return { *this, i };

	}

	/*
	 * returns I_0 + ... + I_i = sum up to i-th integer included
	 */
	uint64_t psum(uint64_t i){

		//empty sum
		if(size()==0) return 0;

		assert(i<size());

		return root->psum(i);

	}

	/*
	 * returns smallest i such that I_0 + ... + I_i >= x
	 */
	uint64_t search(uint64_t x){

		assert(x<=psum());

		return root->search(x);

	}

	/*
	 * Works only on bitvectors! (failed assertion otherwise).
	 *
	 * This function corresponds to select_0:
	 *
	 * returns i such that number of zeros before position i (included)
	 * is == x
	 */
	uint64_t search_0(uint64_t x){

		assert(x <= size() - psum());

		return root->search_0(x);

	}

	/*
	 * returns smallest i such that (i+1) + I_0 + ... + I_i >= x
	 */
	uint64_t search_r(uint64_t x){

		assert(x<=psum()+size());

		return root->search_r(x);

	}

	/*
	 * true iif x is one of the partial sums  0, I_0, I_0+I_1, ...
	 */
	bool contains(uint64_t x){

		assert(x<=psum());

		return root->contains(x);

	}

	/*
	 * true iif x is one of  0, I_0+1, I_0+I_1+2, ...
	 */
	/*bool contains_r(uint64_t x){

		assert(x<=psum()+size());

		return root->contains_r(x);

	}*/

	void push_back(uint64_t x){
		insert(size(),x);
	}

	/*
	 * insert a new integer x at position i
	 */
	void insert(uint64_t i, uint64_t x){

		assert(i<=root->size());

		node* new_root = root->insert(i,x);

		if(new_root != NULL){

			root = new_root;

		}

	}

	/*
	 * return number of integers stored in the structure
	 */
	uint64_t size(){

		assert(root != NULL);
		return root->size();
	}

	/*
	 * return sum of all integers
	 */
	uint64_t psum(){
		return root->psum();
	}

	/*
	 * Total number of bits allocated in RAM for this structure
	 */
	uint64_t bit_size(){

		assert(root != NULL);

		uint64_t bs = 8*sizeof(spsi<leaf_type,B_LEAF,B>);

		if(root != NULL) bs += root->bit_size();

		return bs;

	}

	/*
	 * return i-th integer
	 */
	uint64_t at(uint64_t i){

		assert(size()>0);
		assert(i<size());

		//return psum(i) - (i==0?0:psum(i-1));
		return root->at(i);

	}

	/*
	 * decrement/increment i-th integer by delta units
	 */
	void decrement(uint64_t i, uint64_t delta){
		increment(i,delta,true);
	}

	void increment(uint64_t i, uint64_t delta, bool subtract = false){

		assert(size()>0);
		assert(i<size());

		assert(not subtract or delta <= at(i));

		root->increment(i, delta, subtract);
	}

	/*
	 * set i-th element to x
	 */
	void set(uint64_t i, uint64_t x){

		auto val = at(i);

		increment(i, (val>x?val-x:x-val), x<val);

	}

	ulint serialize(ostream &out){

		assert(root);
		return root->serialize(out);

	}

	void load(istream &in){

		root = new node();
		root->load(in);

	}


private:

	class node{

	public:

		/*
		 * copy constructor
		 */
		node(const node & n){

			subtree_sizes = {n.subtree_sizes};
			subtree_psums = {n.subtree_psums};

			if(n.has_leaves_){

				leaves = vector<leaf_type*>(n.nr_children,NULL);

				for(uint64_t i=0;i<n.nr_children;++i){

					leaves[i] = new leaf_type(*n.leaves[i]);

				}

			}else{

				children = vector<node*>(n.nr_children, NULL);

				for(uint64_t i=0;i<n.nr_children;++i){

					children[i] = new node(*n.children[i]);
					children[i]->overwrite_parent(this);

				}

			}

			node* parent = NULL; 	//NULL for root

			rank_ = n.rank_; 		//rank of this node among its siblings

			nr_children = n.nr_children; 	//number of subtrees

			has_leaves_ = n.has_leaves_;		//if true, leaves array is nonempty and children is empty

		}

		/*
		 * create new root node. This node has only 1 (empty) child, which is a leaf.
		 */
		node(){

			subtree_sizes = vector<uint64_t>(2*B+2);
			subtree_psums = vector<uint64_t>(2*B+2);

			nr_children = 1;
			has_leaves_ = true;

			leaves = vector<leaf_type*>(1);
			leaves[0] = new leaf_type();

		}

		/*
		 * create new node given some children (other internal nodes),the parent, and the rank of this
		 * node among its siblings
		 */
		node(vector<node*> &c, node* P=NULL, uint32_t rank=0){

			this->rank_ = rank;
			this->parent = P;

			subtree_sizes = vector<uint64_t>(2*B+2);
			subtree_psums = vector<uint64_t>(2*B+2);

			uint64_t si = 0;
			uint64_t ps = 0;

			assert(c.size()<=2*B+2);

			for(uint32_t i = 0;i<c.size();++i){

				si += c[i]->size();
				ps += c[i]->psum();

				subtree_sizes[i] = si;
				subtree_psums[i] = ps;

			}

			nr_children = c.size();
			has_leaves_ = false;

			children = vector<node*>(c);

			uint32_t r = 0;
			for(auto cc : children){

				cc->overwrite_rank(r++);
				cc->overwrite_parent(this);

			}

		}

		/*
		 * create new node given some children (leaves),the parent, and the rank of this
		 * node among its siblings
		 */
		node(vector<leaf_type*> &c, node* P=NULL, uint32_t rank=0){

			this->rank_ = rank;
			this->parent = P;

			subtree_sizes = vector<uint64_t>(2*B+2);
			subtree_psums = vector<uint64_t>(2*B+2);

			assert(c.size()<=2*B+2);

			uint64_t si = 0;
			uint64_t ps = 0;

			for(uint32_t i = 0;i<c.size();++i){

				si += c[i]->size();
				ps += c[i]->psum();

				subtree_sizes[i] = si;
				subtree_psums[i] = ps;

			}

			nr_children = c.size();
			has_leaves_ = true;

			leaves = vector<leaf_type*>(c);

		}

		/*
		 * return bit size of all structures rooted in this node
		 */
		uint64_t bit_size(){

			uint64_t bs = 8*sizeof(node);

			bs += subtree_sizes.capacity()*sizeof(uint64_t)*8;

			bs += subtree_psums.capacity()*sizeof(uint64_t)*8;

			bs += children.capacity()*sizeof(node*)*8;

			bs += leaves.capacity()*sizeof(leaf_type*)*8;

			if(has_leaves()){

				for(ulint i=0;i<nr_children;++i){

					assert(leaves[i] != NULL);
					bs += leaves[i]->bit_size();

				}

			}else{

				for(ulint i=0;i<nr_children;++i){

					assert(children[i] != NULL);
					bs += children[i]->bit_size();

				}

			}

			return bs;

		}

		bool has_leaves(){

			return has_leaves_;

		}

		void free_mem(){

			if(has_leaves()){

				for(uint32_t i = 0;i<nr_children;++i) delete leaves[i];

			}else{

				for(uint32_t i = 0;i<nr_children;++i) children[i]->free_mem();
				for(uint32_t i = 0;i<nr_children;++i) delete children[i];

			}

		}

		/*
		 * return i-th integer in the subtree rooted in this node
		 */
		uint64_t at(uint64_t i){

			assert(i < size());

			uint32_t j = 0;

			while(subtree_sizes[j] <= i){

				j++;
				assert(j<subtree_sizes.size());

			}

			//size stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);

			assert(i >= previous_size);

			//i-th element is in the j-th children

			//if children are leaves, extract element form j-th leaf
			if(has_leaves()){

				assert(j<leaves.size());
				assert(j<nr_children);
				assert(leaves[j]!=NULL);
				assert(i-previous_size<leaves[j]->size());

				return leaves[j]->at(i-previous_size);

			}

			//else: recurse on children
			return children[j]->at(i-previous_size);

		}

		/*
		 * returns sum up to i-th integer included
		 */
		uint64_t psum(uint64_t i){

			assert(i < size());

			uint32_t j = 0;

			while(subtree_sizes[j] <= i){

				j++;
				assert(j<subtree_sizes.size());

			}

			//size/psum stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);
			uint64_t previous_psum = (j==0?0:subtree_psums[j-1]);

			assert(i >= previous_size);

			//i-th element is in the j-th children

			//if children are leaves, extract psum from j-th leaf
			if(has_leaves()){

				return previous_psum + leaves[j]->psum(i-previous_size);

			}

			//else: recurse on children
			return previous_psum + children[j]->psum(i-previous_size);

		}

		/*
		 * returns smallest i such that I_0 + ... + I_i >= x
		 */
		uint64_t search(uint64_t x){

			assert(x <= psum());

			uint32_t j = 0;

			while(subtree_psums[j] < x or (subtree_psums[j]==0 and x == 0)){

				j++;
				assert(j<subtree_psums.size());

			}

			//size/psum stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);
			uint64_t previous_psum = (j==0?0:subtree_psums[j-1]);

			assert(x > previous_psum or (previous_psum==0 and x == 0));

			//i-th element is in the j-th children

			//if children are leaves, extract psum from j-th leaf
			if(has_leaves()){

				return previous_size + leaves[j]->search(x-previous_psum);

			}

			//else: recurse on children
			return previous_size + children[j]->search(x-previous_psum);

		}

		/*
		 * This function corresponds to select_0 on bitvectors,
		 * and works only if all integers are 0 or 1 (causes a failed
		 * assertion otherwise!)
		 *
		 * returns smallest i such that the number of zeros before position
		 * i (included) is == x. x must be > 0
		 */
		uint64_t search_0(uint64_t x){

			assert(x <= size() - psum());
			assert(x>0);

			uint32_t j = 0;

			while( (subtree_sizes[j] - subtree_psums[j]) < x){

				j++;
				assert(j<subtree_psums.size());

			}

			//size/psum stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);
			uint64_t previous_psum = (j==0?0:subtree_psums[j-1]);
			uint64_t previous_zeros = previous_size-previous_psum;

			assert(x > previous_zeros);

			//i-th element is in the j-th children

			if(has_leaves()){

				return previous_size + leaves[j]->search_0(x-previous_zeros);

			}

			//else: recurse on children
			return previous_size + children[j]->search_0(x-previous_zeros);

		}

		/*
		 * returns smallest i such that (i+1) + I_0 + ... + I_i >= x
		 */
		uint64_t search_r(uint64_t x){

			assert(x <= psum()+size() );

			uint32_t j = 0;

			while(subtree_psums[j]+subtree_sizes[j] < x){

				j++;
				assert(j<subtree_psums.size());

			}

			//size/psum stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);
			uint64_t previous_psum = (j==0?0:subtree_psums[j-1]);

			assert(x > previous_psum + previous_size or (x==0 and (previous_psum + previous_size == 0)));

			//i-th element is in the j-th children

			//if children are leaves, extract psum from j-th leaf
			if(has_leaves()){

				return previous_size + leaves[j]->search_r(x - (previous_psum+previous_size));

			}

			//else: recurse on children
			return previous_size + children[j]->search_r(x- (previous_psum+previous_size));

		}

		bool contains(uint64_t x){

			if(x==0) return true;

			assert(x <= psum());

			uint32_t j = 0;

			while(subtree_psums[j] < x or (x==0 and subtree_psums[j] == 0)){

				j++;
				assert(j<subtree_psums.size());

			}

			if(subtree_psums[j] == x) return true;

			//size/psum stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);
			uint64_t previous_psum = (j==0?0:subtree_psums[j-1]);

			assert(x > previous_psum or (x==0 and previous_psum == 0));

			//i-th element is in the j-th children

			//if children are leaves, extract psum from j-th leaf
			if(has_leaves()){

				return leaves[j]->contains(x-previous_psum);

			}

			//else: recurse on children
			return children[j]->contains(x-previous_psum);

		}

		bool contains_r(uint64_t x){

			if(x==0) return true;

			assert(x <= psum()+size());

			uint32_t j = 0;

			while(subtree_psums[j]+subtree_sizes[j] < x){

				j++;
				assert(j<subtree_psums.size());

			}

			if(subtree_psums[j]+subtree_sizes[j] == x) return true;

			//size/psum stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);
			uint64_t previous_psum = (j==0?0:subtree_psums[j-1]);

			assert(x > previous_psum+previous_size or (x + previous_psum+previous_size == 0));

			//i-th element is in the j-th children

			//if children are leaves, extract psum from j-th leaf
			if(has_leaves()){

				return leaves[j]->contains_r(x - (previous_psum+previous_size));

			}

			//else: recurse on children
			return children[j]->contains_r(x - (previous_psum+previous_size));

		}

		/*
		 * increment or decrement i-th integer by delta
		 */
		void increment(uint64_t i, uint64_t delta, bool subtract = false){

			assert(i < size());

			uint32_t j = 0;

			while(subtree_sizes[j] <= i){

				j++;
				assert(j<subtree_sizes.size());

			}

			//size stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);

			assert(i >= previous_size);

			//i-th element is in the j-th children

			//if children are leaves, increment in the j-th leaf
			if(has_leaves()){

				assert(j<nr_children);
				assert(j<leaves.size());
				assert(leaves[j]!=NULL);
				leaves[j]->increment(i-previous_size, delta, subtract);

			}else{

				//else: recurse on children
				assert(j<nr_children);
				assert(j<children.size());
				assert(children[j]!=NULL);
				children[j]->increment(i-previous_size, delta, subtract);

			}

			//after the increment, modify psum counters
			for(uint32_t k = j;k<nr_children;++k){

				//check for under/overflows
				assert(subtract or ( subtree_psums[k] <= (~uint64_t(0))-delta ));
				assert((not subtract) or (delta <= subtree_psums[k] ));

				subtree_psums[k] = (subtract?subtree_psums[k]-delta:subtree_psums[k]+delta);

			}

		}

		bool is_root(){
			return parent == NULL;
		}

		bool is_full(){
			assert(nr_children<=2*B+2);
			return nr_children == (2*B + 2);
		}

		void increment_rank(){
			rank_++;
		}

		node* get_parent(){
			return parent;
		}

		/*
		 * insert integer x at position i.
		 * If this node is the root, return the new root.
		 *
		 * new root could be different than current root if current root
		 * is full and is splitted
		 *
		 */
		node* insert(uint64_t i, uint64_t x){

			assert(i <= size());
			assert(is_root() || not parent->is_full());

			node* new_root = NULL;
			node* right = NULL;

			bool splitted = false;

			if(is_full()){

				splitted = true;

				right = split();

				assert(not is_full());
				assert(not right->is_full());

				//insert recursively
				if(i<size()){

					insert_without_split(i,x);

				}else{

					assert(right!=NULL);
					right->insert_without_split(i-size(),x);

				}

				//if this is the root, create new root
				if(is_root()){

					vector<node*> vn {this, right};

					new_root = new node(vn);
					assert(not new_root->is_full());

					this->overwrite_parent(new_root);
					right->overwrite_parent(new_root);

				}else{

					//else: pass the info to parent

					assert(rank()<parent->number_of_children());
					parent->new_children(rank(), this, right);

				}

			}else{

				insert_without_split(i,x);

			}

			//if not root, do not return anything.
			return new_root;

		}


		uint32_t rank(){return rank_;}

		void overwrite_rank(uint32_t r){rank_=r;}

		uint64_t size(){

			assert(nr_children>0);
			assert(nr_children-1 < subtree_sizes.size());
			return subtree_sizes[nr_children-1];

		}

		uint64_t psum(){
			return subtree_psums[nr_children-1];
		}

		void overwrite_parent(node *P){
			parent = P;
		}

		uint32_t number_of_children(){
			return nr_children;
		}

		ulint serialize(ostream &out){

			ulint w_bytes=0;
			ulint subtree_sizes_len = subtree_sizes.size();
			ulint subtree_psums_len = subtree_psums.size();
			ulint children_len = children.size();
			ulint leaves_len = leaves.size();


			out.write((char*)&subtree_sizes_len,sizeof(subtree_sizes_len));
			w_bytes += sizeof(subtree_sizes_len);

			out.write((char*)&subtree_psums_len,sizeof(subtree_psums_len));
			w_bytes += sizeof(subtree_psums_len);

			out.write((char*)&children_len,sizeof(children_len));
			w_bytes += sizeof(children_len);

			out.write((char*)&leaves_len,sizeof(leaves_len));
			w_bytes += sizeof(leaves_len);


			out.write((char*)subtree_sizes.data(),sizeof(uint64_t)*subtree_sizes_len);
			w_bytes += sizeof(uint64_t)*subtree_sizes_len;

			out.write((char*)subtree_psums.data(),sizeof(uint64_t)*subtree_psums_len);
			w_bytes += sizeof(uint64_t)*subtree_psums_len;

			out.write((char*)&has_leaves_,sizeof(has_leaves_));
			w_bytes += sizeof(has_leaves_);

			if(has_leaves_){

				for(auto l : leaves) w_bytes += l->serialize(out);

			}else{

				for(auto c : children) w_bytes += c->serialize(out);

			}

			out.write((char*)&rank_,sizeof(rank_));
			w_bytes += sizeof(rank_);

			out.write((char*)&nr_children,sizeof(nr_children));
			w_bytes += sizeof(nr_children);

			return w_bytes;

		}

		void load(istream &in){

			ulint subtree_sizes_len;
			ulint subtree_psums_len;
			ulint children_len;
			ulint leaves_len;

			in.read((char*)&subtree_sizes_len,sizeof(subtree_sizes_len));

			in.read((char*)&subtree_psums_len,sizeof(subtree_psums_len));

			in.read((char*)&children_len,sizeof(children_len));

			in.read((char*)&leaves_len,sizeof(leaves_len));

			assert(subtree_sizes_len>0);
			assert(subtree_psums_len>0);

			subtree_sizes = vector<uint64_t>(subtree_sizes_len);
			in.read((char*)subtree_sizes.data(),sizeof(uint64_t)*subtree_sizes_len);

			subtree_psums = vector<uint64_t>(subtree_psums_len);
			in.read((char*)subtree_psums.data(),sizeof(uint64_t)*subtree_psums_len);


			in.read((char*)&has_leaves_,sizeof(has_leaves_));


			if(has_leaves_){

				assert(leaves_len>0);
				leaves = vector<leaf_type*>(leaves_len);

				for(auto& l : leaves) l = new leaf_type();
				for(auto& l : leaves) l->load(in);

			}else{

				assert(children_len>0);
				children = vector<node*>(children_len);

				for(auto& c : children) c = new node();
				for(auto& c : children) c->overwrite_parent(this);
				for(auto& c : children) c->load(in);

			}

			in.read((char*)&rank_,sizeof(rank_));

			in.read((char*)&nr_children,sizeof(nr_children));

		}

	private:

		/*
		 * new element between elements i and i+1
		 */
		void new_children(uint32_t i, node* left, node* right){

			assert(i<nr_children);
			assert(not is_full()); 		//this node must not be full!
			assert(not has_leaves()); 	//this procedure can be called only on nodes
										//whise children are not leaves

			//size/psum stored in previous counter
			uint64_t previous_size = (i==0?0:subtree_sizes[i-1]);
			uint64_t previous_psum = (i==0?0:subtree_psums[i-1]);

			//first of all, move forward counters i+1, i+2, ...
			for(uint32_t j=subtree_sizes.size()-1; j>i; j--){

				//node is not full so overwriting subtree_sizes[subtree_sizes.size()-1] is safe
				subtree_sizes[j] = subtree_sizes[j-1];
				subtree_psums[j] = subtree_psums[j-1];

			}

			subtree_sizes[i] = previous_size + left->size();
			subtree_psums[i] = previous_psum + left->psum();

			//number of children increases by 1
			nr_children++;

			//temporary copy children
			vector<node*> temp(children);

			//reset children
			children = vector<node*>(nr_children);
			uint32_t k=0;//index in children

			for(uint32_t j = 0;j < nr_children-1;++j){

				if(i==j){

					//insert left and right, ignore child i
					assert(k<nr_children);
					children[k++] = left;
					assert(k<nr_children);
					children[k++] = right;

				}else{

					//insert child i
					assert(k<nr_children);
					assert(temp[j]!=NULL);
					children[k++] = temp[j];

				}

			}

			assert(k==nr_children);

			//children i and i+1 are new; we have now to increase the rank
			//of children i+2,...

			for(uint32_t j = i+2;j<nr_children;j++){

				children[j]->increment_rank();
				assert(children[j]->rank()<nr_children);

			}

		}

		void new_children(uint32_t i, leaf_type* left, leaf_type* right){

			assert(i<nr_children);
			assert(not is_full()); //this node must not be full!
			assert(has_leaves());

			//treat this case separately
			if(nr_children==1){

				subtree_sizes[0] = left->size();
				subtree_sizes[1] = left->size() + right->size();

				subtree_psums[0] = left->psum();
				subtree_psums[1] = left->psum() + right->psum();

				leaves = vector<leaf_type*> {left, right};

				nr_children++;

				return;

			}

			//size/psum stored in previous counter
			uint64_t previous_size = (i==0?0:subtree_sizes[i-1]);
			uint64_t previous_psum = (i==0?0:subtree_psums[i-1]);

			//first of all, move forward counters i+1, i+2, ...
			for(uint32_t j=nr_children; j>i; j--){

				//node is not full so overwriting subtree_sizes[subtree_sizes.size()-1] is safe
				subtree_sizes[j] = subtree_sizes[j-1];
				subtree_psums[j] = subtree_psums[j-1];

			}

			subtree_sizes[i] = previous_size + left->size();
			subtree_psums[i] = previous_psum + left->psum();

			//number of children increases by 1
			nr_children++;

			//temporary copy leaves
			vector<leaf_type*> temp(leaves);

			//reset leaves
			leaves = vector<leaf_type*>(nr_children);
			uint32_t k=0;//index in leaves

			for(uint32_t j = 0;j < nr_children-1 ;++j){

				if(i==j){

					//insert left and right, ignore child i
					assert(k<nr_children);
					leaves[k++] = left;
					assert(k<nr_children);
					leaves[k++] = right;

				}else{

					//insert child i
					assert(k<nr_children);
					leaves[k++] = temp[j];

				}

			}

		}

		/*
		 * insert in a node, where we know that this node is not full
		 */
		void insert_without_split(uint64_t i, uint64_t x){

			assert(not is_full());
			assert(i <= size());

			uint32_t j = nr_children-1;

			//if i==size, then insert in last children
			if(i<size()){

				j = 0;

				while(subtree_sizes[j] <= i){

					j++;
					assert(j<subtree_sizes.size());

				}

			}

			//size stored in previous counter
			uint64_t previous_size = (j==0?0:subtree_sizes[j-1]);

			assert(i >= previous_size);

			//i-th element is in the j-th children

			uint64_t insert_pos = i-previous_size;

			if(not has_leaves()){

				assert(not is_full());
				assert(insert_pos<=children[j]->size());
				assert(children[j]->get_parent()==this);
				children[j]->insert(insert_pos, x);

			}else{

				if(leaf_is_full(leaves[j])){

					//if leaf full, split it

					leaf_type* right = leaves[j]->split();
					leaf_type* left = leaves[j];

					assert(not leaf_is_full(leaves[j]));

					//insert new children in this node
					this->new_children(j,left,right);

					//insert in the correct leaf half
					if(insert_pos<left->size()){

						left->insert(insert_pos, x);

					}else{

						right->insert(insert_pos-left->size(), x);

					}

				}else{

					leaves[j]->insert(insert_pos, x);

				}

			}

			uint64_t ps=0;
			uint64_t si=0;

			/*
			 * we inserted an integer in some children, and number of
			 * children may have increased. re-compute counters
			 */

			assert(not has_leaves() or nr_children<=leaves.size());
			assert(has_leaves() or nr_children<=children.size());
			assert(nr_children<=subtree_psums.size());
			assert(nr_children<=subtree_sizes.size());

			for(uint32_t k=0;k<nr_children;++k){

				if(has_leaves()){

					assert(leaves[k]!=NULL);
					ps += leaves[k]->psum();
					si += leaves[k]->size();

				}else{

					assert(children[k]!=NULL);
					ps += children[k]->psum();
					si += children[k]->size();

				}

				subtree_psums[k] = ps;
				subtree_sizes[k] = si;

			}


		}

		/*
		 * splits this (full) node into 2 nodes with B keys each.
		 * The left node is this node, and we return the right node
		 */
		node* split(){

			assert(nr_children == 2*B+2);

			node* right = NULL;

			if(has_leaves()){

				vector<leaf_type*> right_children_l(nr_children-nr_children/2);

				ulint k=0;

				for(uint32_t i = nr_children/2; i<nr_children;++i)
					right_children_l[k++] = leaves[i];

				assert(k==right_children_l.size());

				right = new node(right_children_l, parent, rank()+1);

			}else{

				vector<node*> right_children_n(nr_children-nr_children/2);

				ulint k=0;

				for(uint32_t i = nr_children/2; i<nr_children;++i)
					right_children_n[k++] = children[i];

				assert(k==right_children_n.size());

				right = new node(right_children_n, parent, rank()+1);

			}

			//update new number of children of this node
			nr_children = nr_children/2;

			return right;

		}

		bool leaf_is_full(leaf_type* l){

			assert(l->size()<=2*B_LEAF);
			return (l->size() == 2*B_LEAF);

		}

		/*
		 * in the following 2 vectors, the first nr_subtrees+1 elements refer to the
		 * nr_subtrees subtrees
		 */
		vector<uint64_t> subtree_sizes;
		vector<uint64_t> subtree_psums;

		vector<node*> children;
		vector<leaf_type*> leaves;

		node* parent = NULL; 		//NULL for root
		uint32_t rank_ = 0; 		//rank of this node among its siblings

		uint32_t nr_children=0; 	//number of subtrees

		bool has_leaves_=false;		//if true, leaves array is nonempty and children is empty

	};

	node* root = NULL;		//tree root

};

}

#endif /* INTERNAL_SPSI_HPP_ */
