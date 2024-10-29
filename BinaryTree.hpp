#ifndef ERC_BINTREE
#define ERC_BINTREE


#include <cstdint>
#include <vector>

//! Contains an implementation of a Binary Tree system, utility functions, and traversal functions.
namespace BinTree {
/********!
 * @class c_BinaryNode
 * 
 * @brief
 * Implementation of a Binary Tree system using individual nodes, recursive functions, and a containing structure.
 * 
 * @date
 * 20 September 2024
 ********/
template<typename NodeData> struct c_BinaryNode {
	c_BinaryNode* child_L, *child_R;
	NodeData nodeData;
	uint32_t index;
	
	//! Construct the node with just an ID and a value. Both child nodes will be set to nullptr.
	c_BinaryNode(uint32_t id, const NodeData val) : child_L(nullptr), child_R(nullptr) {nodeData = val; index = id;}
	//! Construct the node with an ID, value, and two specified child nodes.
	c_BinaryNode(uint32_t id, const NodeData val, c_BinaryNode* l, c_BinaryNode* r) : child_L(l), child_R(r) {nodeData = val; index = id;}
	//! Returns the number of immediate connections this node has.
	uint8_t determineConnections() const noexcept {
		if (child_L == nullptr) {
			if (child_R == nullptr) return 0; else return 1;
		} else {
			if (child_R == nullptr) return 1; else return 2;
		}
	}
	//! Returns the depth of the binary tree as judged from this node.
	uint32_t determineDepth() const noexcept {
		uint32_t dL = 0, dR = 0;
		if (this->child_L != nullptr) {dL = this->child_L->determineDepth();}
		if (this->child_R != nullptr) {dR = this->child_R->determineDepth();}
		return 1 + ((dL > dR) ? dL : dR);
	}
	//! Returns the number of children this node has.
	uint32_t determineDescendants() const noexcept {
		uint32_t dL = 0, dR = 0;
		if (this->child_L != nullptr) {dL = this->child_L->determineDescendants();}
		if (this->child_R != nullptr) {dR = this->child_R->determineDescendants();}
		return 1 + dL + dR;
	}
	
	/********!
	 * @brief
	 *  	Returns the leftmost descendant of this node, if one exists.
	 * @param [in] idx_max
	 *  	The maximum depth of the search. Set to zero if you want the deepest leftmost node.
	 * @param [in] idx_recurse
	 * 		Not for use in standard method use. This value allows the function to stop at depth @c idx_max if specified.
	 * @return
	 * 		A pointer to the leftmost node, if valid.
	 ********/
	c_BinaryNode* getLeftmostNode(uint32_t idx_max = 0, uint32_t idx_recurse = 0) const noexcept {
		if (this->child_L != nullptr) {
			if ((idx_max != 0) && (idx_max >= idx_recurse)) {
				return this;
			} else return this->child_L->getLeftmostNode(idx_max, idx_recurse + 1);
		} else {
			return this;
		}
	}
	
	/********!
	 * @brief
	 *  	Returns the rightmost descendant of this node, if one exists.
	 * @param [in] idx_max
	 *  	The maximum depth of the search. Set to zero if you want the deepest rightmost node.
	 * @param [in] idx_recurse
	 * 		Not for use in standard method use. This value allows the function to stop at depth @c idx_max if specified.
	 * @return
	 * 		A pointer to the rightmost node, if valid.
	 ********/
	c_BinaryNode* getRightmostNode(uint32_t idx_max = 0, uint32_t idx_recurse = 0) const noexcept {
		if (this->child_R != nullptr) {
			if ((idx_max != 0) && (idx_max >= idx_recurse)) {
				return this;
			} else return this->child_R->getRightmostNode(idx_max, idx_recurse + 1);
		} else {
			return this;
		}
	}
};

/********!
 * @brief
 *  	Performs a Recursive Ordered Traversal from the specified Binary Node. Can operate in In-Order or in Reverse In-Order.
 * @param [in] what
 *  	Node to begin the traversal from. Must not be null.
 * @param [in] left
 * 		Whether to perform an In-Order (left-side, true) or Reverse In-Order (right-side, false) traversal.
 * @return
 * 		A vector of nonnull pointers in the specified order, as determined from the descendants of node @c what.
 ********/
template<typename NodeData> std::vector<c_BinaryNode<NodeData>*> devTraverseOrd(c_BinaryNode<NodeData>* what, bool left) {
	std::vector<c_BinaryNode<NodeData>*> out={};
	if (left && (what->child_L != nullptr)) {
		out = devTraverseOrd(what->child_L, true);
	} else if (!left && (what->child_R != nullptr)) {
		out = devTraverseOrd(what->child_R, false);
	}
	out.push_back(what);
	if (left && (what->child_R != nullptr)) {
		auto out2 = devTraverseOrd(what->child_R, true);
		for (c_BinaryNode<NodeData>* i : out2) {
			out.push_back(i);
		}
	} else if (!left && (what->child_L != nullptr)) {
		auto out2 = devTraverseOrd(what->child_L, false);
		for (c_BinaryNode<NodeData>* i : out2) {
			out.push_back(i);
		}
	}
	return out;
}

/********!
 * @brief
 *  	Performs a Recursive Special Traversal from the specified Binary Node. Can operate in Preorder or in Postorder.
 * @param [in] what
 *  	Node to begin the traversal from. Must not be null.
 * @param [in] left
 * 		Whether to perform a Preorder (true) or Postorder (false) traversal.
 * @return
 * 		A vector of nonnull pointers in the specified order, as determined from the descendants of node @c what.
 ********/
template<typename NodeData> std::vector<c_BinaryNode<NodeData>*> devTraverseSpc(c_BinaryNode<NodeData>* what, bool pre) {
	std::vector<c_BinaryNode<NodeData>*> out={};
	if (pre) {
		out.push_back(what);
	}
	if (what->child_L != nullptr) {
		auto out1 = devTraverseSpc(what->child_L, pre);
		for (c_BinaryNode<NodeData>* i : out1) {
			out.push_back(i);
		}
	}
	if (what->child_R != nullptr) {
		auto out2 = devTraverseSpc(what->child_R, pre);
		for (c_BinaryNode<NodeData>* i : out2) {
			out.push_back(i);
		}
	}
	if (!pre) {
		out.push_back(what);
	}
	return out;
}

//! Combined implementation of a Binary Tree, using a set of Binary Nodes and additional data controls.
template<typename NodeData> struct c_BinaryTree {
	uint8_t treeHeight;
	std::vector<c_BinaryNode<NodeData>*> rawData;
	c_BinaryNode<NodeData>* head;
	
	//! Calculates the height of the tree (sets @c treeHeight), and returns the total number of nodes.
	uint32_t calcStats() {
		this->treeHeight = this->head->determineDepth();
		return this->head->determineDescendants();
	}
	//! Safely deletes the entire binary tree, assuming that each Binary Node was created by the @c new operator.
	~c_BinaryTree() {
		auto deleteOrder = this->traversePostOrder();
		for (c_BinaryNode<NodeData>* i : deleteOrder) {
			delete i;
			i = nullptr;
		}
		head = nullptr;
	}
	/********!
	 * @brief
	 *  	Overwrites the entire object with a freshly-constructed, balanced binary tree. It will contain (pow(2, height) - 1)
	 * 		nodes that all contain the same value.
	 * @param [in] height
	 *  	Height of the binary tree to generate.
	 * @param [in] prefill
	 *  	The default value to set all nodes to.
	 * @example
	 * 		For a height of 4, the indexes of the tree will be organized as follows:
	 *	 	    1
	 *	   2         3
	 * 	 4   5     6   7
	 *  8 9 A B   C D E F
	 ********/
	void generateFull(uint8_t height, const NodeData prefill) {
		this->treeHeight = height;
		
		const uint32_t maxIndex = (1u << height);
		uint32_t curID = maxIndex - 1, lastSize = 0;
		for (uint8_t i = (height - 1); i > 0; i--) {
			const uint32_t levelSize = (1u << i);
			
			if (i == (height - 1)) {
				for (uint32_t j = 0; j < levelSize; j++) {
					rawData.push_back(new c_BinaryNode<NodeData>(curID, prefill));
					curID--;
				}
			} else {
				uint32_t prevIndex = rawData.size()-1;
				for (uint32_t j = 0; j < levelSize; j++) {
					const uint32_t tmpi = lastSize - (j << 1);
					rawData.push_back(new c_BinaryNode<NodeData>(curID, prefill,
						rawData.at( (prevIndex + 2) - tmpi ), rawData.at( (prevIndex + 1) - tmpi )));
					curID--;
				}
			}
			lastSize = levelSize;
		}
		rawData.push_back(new c_BinaryNode<NodeData>(1, prefill, rawData.back(), rawData.at(rawData.size() - 2)));
		head = rawData.back();
	}
	
	//! Traverses the entire structure in a Level-Order (Breadth-First) fashion and returns the reorganized pointers.
	std::vector<c_BinaryNode<NodeData>*> traverseBreadth() const noexcept {
		std::vector<c_BinaryNode<NodeData>*> output = {}, last = {this->head}, childs = {};
		for (uint8_t i=0; i < this->treeHeight; i++) {
			for (c_BinaryNode<NodeData>* ptr : last) {
				if (ptr == nullptr) continue;
				childs.push_back(ptr->child_L);
				childs.push_back(ptr->child_R);
				output.push_back(ptr);
			}
			if (childs.empty()) break;
			else {
				last = childs;
				childs.clear();
			}
		}
		return output;
	}
	
	//! Traverses the entire structure in an In-Order fashion and returns the reorganized pointers.
	std::vector<c_BinaryNode<NodeData>*> traverseInOrder() const noexcept {
		std::vector<c_BinaryNode<NodeData>*> output = devTraverseOrd(this->head, true);
		return output;
	}
	
	//! Traverses the entire structure in a Reverse-Order fashion and returns the reorganized pointers.
	std::vector<c_BinaryNode<NodeData>*> traverseRevOrder() const noexcept {
		std::vector<c_BinaryNode<NodeData>*> output = devTraverseOrd(this->head, false);
		return output;
	}
	
	//! Traverses the entire structure in a Pre-Order fashion and returns the reorganized pointers.
	std::vector<c_BinaryNode<NodeData>*> traversePreOrder() const noexcept {
		std::vector<c_BinaryNode<NodeData>*> output = devTraverseSpc(this->head, true);
		return output;
	}
	
	//! Traverses the entire structure in a Post-Order fashion and returns the reorganized pointers.
	std::vector<c_BinaryNode<NodeData>*> traversePostOrder() const noexcept {
		std::vector<c_BinaryNode<NodeData>*> output = devTraverseSpc(this->head, false);
		return output;
	}
	
	/********!
	 * @brief
	 *  	Inserts a new node with a specified value into the tree at the first possible open position.
	 * @param [in] value
	 *  	The new value to insert into the Binary Tree.
	 * @return
	 *  	The tree index (ID) of the new node.
	 ********/
	uint32_t insertNode(const NodeData value) {
		c_BinaryNode<NodeData>* newroot = nullptr;
		bool goLeft = true;
		std::vector<c_BinaryNode<NodeData>*> last = {this->head}, childs = {};
		while (newroot == nullptr) {
			for (auto ptr : last) {
				if (ptr->child_L == nullptr) {
					newroot = ptr;
				} else if (ptr->child_R == nullptr) {
					goLeft = false;
					newroot = ptr;
				} else {
					childs.push_back(ptr);
				}
			}
			last = childs; childs.clear();
		}
		uint32_t nindex = this->rawData.front()->index + 1;
		this->rawData.insert(this->rawData.begin(), new c_BinaryNode<NodeData>(nindex, value));
		if (goLeft) {newroot.child_L = this->rawData.front();} else {newroot.child_R = this->rawData.front();}
		return nindex;
	}
	
	/********!
	 * @brief
	 *  	Replaces the value stored in the node with the specified index without disrupting its connections.
	 * @param [in] indexFind
	 *  	The tree index (ID) of the node to update.
	 * @param [in] newVal
	 *  	The new value to set.
	 ********/
	void replaceNodeData(uint32_t indexFind, const NodeData newVal) {
		c_BinaryNode<NodeData>* target = nullptr;
		std::vector<c_BinaryNode<NodeData>*> last = {this->head}, childs = {};
		while (target == nullptr) {
			for (auto ptr : last) {
				if (ptr == nullptr) continue;
				if (ptr->index == indexFind) target = ptr;
			}
			last = childs; 	childs.clear();
			if (last.empty()) return; // Cannot complete if index doesn't exist.
		}
		target.nodeData = newVal;
	}
	
	/********!
	 * @brief
	 *  	Replaces the node at the specified index, resetting its index but retaining its connections.
	 * @param [in] indexFind
	 *  	The tree index (ID) of the node to update.
	 * @param [in] newVal
	 *  	The new value to set.
	 * @param [in] newIndex
	 *  	The new tree index (ID) to assign to the node.
	 ********/
	void replaceNodeFull(uint32_t indexFind, const NodeData newVal, const uint32_t newIndex) {
		c_BinaryNode<NodeData>* target = nullptr;
		std::vector<c_BinaryNode<NodeData>*> last = {this->head}, childs = {};
		while (target == nullptr) {
			for (auto ptr : last) {
				if (ptr == nullptr) continue;
				if (ptr->index == indexFind) target = ptr;
			}
			last = childs; childs.clear();
			if (last.empty()) return; // Cannot complete if index doesn't exist.
		}
		target.nodeData = newVal;
		target.index = newIndex;
	}
	
	/********!
	 * @brief
	 *  	Finds and deletes the (first) node it encounters with the specified index, updating the internal structure of
	 *  	the binary tree accordingly.
	 * @param [in] index
	 *  	The tree index (ID) of the node to remove.
	 * @return
	 *  	Returns the index of the node that was rearranged into the previous node's position if successful, or returns
	 *  	0 if the node could not be found or if the node was the last node within the binary tree.
	 ********/
	uint32_t deleteNode(uint32_t index) {
		c_BinaryNode<NodeData>* replacement = this->rawData.front(), *target = nullptr, *repParent = nullptr;
		bool parentLeft = true;
		for (uint32_t i=0; i < this->rawData.size(); i++) {
			if (this->rawData.at(i)->index == index) {
				target = this->rawData.at(i);
			} else if (this->rawData.at(i)->child_L == replacement) {
				repParent = this->rawData.at(i);
			} else if (this->rawData.at(i)->child_R == replacement) {
				repParent = this->rawData.at(i); parentLeft = false;
			}
		}
		if (target == nullptr) return 0;
		else if (target == replacement) {
			this->rawData.erase(this->rawData.begin());
			return 0;
		}
		target->index = replacement->index;
		target->nodeData = replacement->nodeData;
		replacement->index = 0;
		if (parentLeft) {
			repParent->child_L = nullptr;
		} else {
			repParent->child_R = nullptr;
		}
		delete replacement;
		this->rawData.erase(this->rawData.begin());
		return target->index;
	} 
};
}
#endif
