#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            explicit Node(const TYPE &val)
				: elt{ val }, child{ nullptr }, sibling{ nullptr }, prev{ nullptr }
           {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child; //left child
            Node *sibling; //right sibling
			Node *prev;
    }; // Node


    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
		root = nullptr;
		num = 0;
    } // PairingPQ()


    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
		num = 1;
		root = new Node(*start);
		start++;
		while (start != end) {
			push(*start);
			start++;
		}
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
        BaseClass{ other.compare } {
		root = nullptr;
		*this = other;
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ& operator=(const PairingPQ& rhs) {
		if (this != &rhs) {
			clear(root);
			root = clone(rhs.root);
		}
		num = rhs.size();
		return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
		clear(root);
		root = nullptr;
		num = 0;
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
		std::deque<Node *> buffer;
		std::deque<Node *> backtrack;
		Node *current = root;
		bool flag = false;
		while (!flag) {
			if (current != nullptr) {
				backtrack.push_back(current);
				current = current->child;
			}
			else {
				if (backtrack.empty()) {
					flag = true;
				} 
				else {
					current = backtrack.back();
					backtrack.pop_back();
					Node * temp = current->sibling;
					current->sibling = nullptr;
					current->child = nullptr;
					buffer.push_back(current);
					current = temp;
				}
			}
		}
		root = buffer[0];
		buffer.pop_back();
		while (!buffer.empty()) {
			meld(root, buffer.back());
			buffer.pop_back();
		}
    } // updatePriorities()


    // Description: Add a new element to the priority_queue. This has been provided for you,
    //              in that you should implement push functionality in the
    //              addNode function.
    // Runtime: Amortized O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
		addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority_queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority_queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
		Node * destroy = root;
		if (root->child == nullptr) {
			root = nullptr;
		}
		else {
			root = merge(root->child);
		}
		delete destroy;
		num--;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
		return root->elt;
    } // top()


    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual std::size_t size() const {
		return num;
    } // size()

    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const {
		return (root == nullptr);
    } // empty()


    // Description: Updates the priority of an element already in the priority_queue by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain priority_queue invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE & new_value) {
		if (this->compare(new_value, node->elt)) {
			return;
		}
		node->elt = new_value;
		if (node != root) {
			//If has sibling
			if (node->sibling != nullptr) {
				node->sibling->prev = node->prev;
			} 
			if (node->prev->child == node) {
				node->prev->child = node->sibling;
			}
			else if (node->prev->sibling == node) {
				node->prev->sibling = node->sibling;
			}
			node->prev = nullptr;
			node->sibling = nullptr;
			meld(root, node);
		}
    } // updateElt()


    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: Amortized O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    Node* addNode(const TYPE & val) {
		num++;
		Node * birth = new Node(val);
		if (root == nullptr) {
			root = birth;
		}
		else {
			meld(root, birth);
		}
		return birth;
    } // addNode()


private:
	Node* root;
	std::size_t num; 
    // TODO: Add any additional member functions or data you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
	Node * clone(Node * node) {
		if (node == nullptr) {
			return nullptr;
		}
		else {
			Node *p = new Node(*node);
			if ((p->child = clone(node->child)) != nullptr) {
				p->child->prev = p;
			}
			if ((p->sibling = clone(node->sibling)) != nullptr) {
				p->sibling->prev = p;
			}
			return p;
		}
	}
	void clear(Node * node) {
		if (node != nullptr) {
			clear(node->child);
			clear(node->sibling);
			delete node;
		}
	}
	void meld(Node * &node_1, Node * node_2) {
		if (node_2 == nullptr) {
			return;
		}
		if (this->compare(node_1->elt, node_2->elt)) {
			node_2->prev = node_1->prev;
			node_1->prev = node_2;
			node_1->sibling = node_2->child;
			if (node_1->sibling != nullptr) {
				node_1->sibling->prev = node_1;
			}
			node_2->child = node_1;
			node_1 = node_2;
		}
		else {
			node_2->prev = node_1;
			node_1->sibling = node_2->sibling;
			if (node_1->sibling != nullptr) {
				node_1->sibling->prev = node_1;
			}
			node_2->sibling = node_1->child;
			if (node_2->sibling != nullptr) {
				node_2->sibling->prev = node_2;
			}
			node_1->child = node_2;
		}
	}
	Node * merge(Node * node_one) {
		if (node_one->sibling == nullptr) {
			return node_one;
		} 
		std::vector<Node *> tree(1);
		int i = 0;
		while(node_one != nullptr) {
			if (i == static_cast<int> (tree.size())) {
				tree.resize(tree.size() * 2);
			}
			tree[i] = node_one;
			node_one->prev->sibling = nullptr;
			node_one = node_one->sibling;
			i++;
		}
		if (i == static_cast<int> (tree.size())) {
			tree.resize(i + 1);
		}
		tree[i] = nullptr;
		int j = 0;
		while (j + 1 < i) {
			meld(tree[j], tree[j + 1]);
			j += 2;
		}
		j -= 2;
		if (j == i - 3) {
			meld(tree[j], tree[j + 2]);
		}
		while (j >= 2) {
			meld(tree[j - 2], tree[j]);
			j -= 2;
		}
		return tree[0];
	}
};


#endif // PAIRINGPQ_H
