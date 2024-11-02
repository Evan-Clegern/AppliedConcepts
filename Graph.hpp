#ifndef ERC_GRAPH
#define ERC_GRAPH

#define NODEBUG

#ifndef NODEBUG
#include <iostream> // debugging only

void dbgprint(const char* what) {
	std::cout << what << '\n';
	return;
}
#else
void dbgprint(const char* what) {
	return;
}
#endif

#include <cstdint>
#include <vector>

//! Contains an implementation of a Directed Graph data structure and the two most common traversal methods for it.
namespace GraphStruct {

/********!
 * @class c_GraphNode
 * 
 * @brief
 * Represents one individual node on a graph structure, which possesses a unique index, a stored value, a list of
 * external connections (outbound), and a list of registered internal connections (inbound). Has methods to add,
 * traverse, and remove externally-directed connections.
 * 
 * @date
 * 29 October 2024
 ********/
template<typename NodeData> struct c_GraphNode {
	uint32_t index;
	NodeData nodeData;
	std::vector<c_GraphNode<NodeData>*> cnt_out, cnt_in;
	
	//! Constructs the node with just an ID and a value, associating it with no connections.
	c_GraphNode(uint32_t id, const NodeData val) : index(id), nodeData(val) {}
	//! Construct the node with an ID, a value, and an initializer list of outgoing connections that will be verified.
	c_GraphNode(uint32_t id, const NodeData val, std::initializer_list<c_GraphNode<NodeData>*> list) : index(id), nodeData(val) {
		for (c_GraphNode<NodeData>* i : list) {
			if ((i != nullptr) && (i->index != id)) {
				cnt_out.push_back(i);
				i->cnt_in.push_back(this);
			}
		}
	}
	//! Returns the number of connections this node has outgoing to other nodes.
	uint32_t externalDegree() const noexcept {
		return this->cnt_out.size();
	}
	//! Returns the number of (registered) connections this node has incoming.
	uint32_t internalDegree() const noexcept {
		return this->cnt_in.size();
	}
	//! Add a new, directed connection. Returns the # of outgoing connections this node has.
	uint32_t establishCnt(c_GraphNode<NodeData>* const other) {
		if ((other != nullptr) && (other->index != this->index)) {
			this->cnt_out.push_back(other);
			other->cnt_in.push_back(this);
		}
		return this->cnt_out.size();
	}
	//! Removes an existing external connection to the specified node in O(n) time. Returns the # of outgoing connections this node has.
	//! It will only remove the first instance of a connection, in the rare case that multiple exist.
	uint32_t severCnt(c_GraphNode<NodeData>* const other) {
		uint32_t extnum = this->cnt_out.size();
		if ((other == nullptr) || (other->index == this->index)) {
			return extnum;
		}
		uint32_t intnum = other->cnt_in.size();
		uint32_t maxidx = (extnum > intnum) ? extnum : intnum;
		bool gotExt = false, gotInt = false;
		for (uint32_t i=0; i < maxidx; i++) {
			c_GraphNode<NodeData>* tmpExt = nullptr, * tmpInt = nullptr;
			if (i < extnum) {
				tmpExt = this->cnt_out.at(i);
			}
			if (i < intnum) {
				tmpInt = other->cnt_in.at(i);
			}
			if (tmpExt == other) {
				this->cnt_out.erase(this->cnt_out.begin() + i);
				gotExt = true;
			}
			if (tmpInt == this) {
				other->cnt_in.erase(other->cnt_in.begin() + i);
				gotInt = true;
			}
			if (gotExt && gotInt) {
				break;
			}
		}
		return this->cnt_out.size();
	}
	//! Removes an existing external connection at the specified INTERNAL index for this node. Returns the # of outgoing connections this node has.
	uint32_t severCnt(uint32_t index) {
		uint32_t extnum = this->cnt_out.size();
		if (index >= extnum) {
			return extnum;
		}
		c_GraphNode<NodeData>* other = this->cnt_out.at(index);
		uint32_t intnum = other->cnt_in.size();
		for (uint32_t i=0; i < intnum; i++) {
			c_GraphNode<NodeData>* tmp = other->cnt_in.at(i);
			if (tmp == this) {
				other->cnt_in.erase(other->cnt_in.begin() + i); break;
			}
		}
		this->cnt_out.erase(this->cnt_out.begin() + index);
		return this->cnt_out.size();
	}
	//! Traverses the specified index of external connection, if valid.
	c_GraphNode<NodeData>* traverseCnt(uint32_t index) {
		if (index >= this->cnt_out.size()) {
			return nullptr;
		}
		return this->cnt_out.at(index);
	}
};

//! Helper for listing paired connections when constructing a map, with an optional PRIORITY field (only for higher level use).
//! For non-prioritized connections, do it as {index_from, index_to}. For prioritized, do {index_from, index_to, prio}.
struct c_GraphCnt {
	uint32_t from, to;
	uint8_t priority = 0;
};

//! Helper structure for running the graph-level traversal, so we don't have to make four shared pointer parameters.
template<typename NodeData> struct c_SearchHeader {
	// I'm aware this would be more efficient as a <queue> structure, but I wanted to make this code with as few headers as possible.
	std::vector<c_GraphNode<NodeData>*> visit_queue;
	std::vector<uint32_t> idx_visited;
	uint32_t min_idx = 4294967293, max_idx = 0; // init with a value higher than max supported, so it always goes down
	//! Returns TRUE if the index has not been visited, and then adds the index to the list. Returns FALSE if the index has been visited.
	bool testAdd(uint32_t index) {
		if (index > max_idx) {
			max_idx = index;
			if (index < min_idx) {
				min_idx = index;
			}
		} else if (index < min_idx) {
			min_idx  = index;
		} else if (index == max_idx || index == min_idx) {
			return false;
		} else {
			// return false in this section as needed
			for (uint32_t value : idx_visited) {
				if (index == value) return false;
			}
		}
		if (index < 10) {
			char printable[] = "Adding new node index  ";
			printable[22] = '0' + index;
			dbgprint(printable);
		}
		idx_visited.push_back(index);
		return true;
	}
};

//! Helper function for running a Breadth-First traversal from the specified node. Runs recursively!
template<typename NodeData> std::vector<c_GraphNode<NodeData>*> devTraverseBfs(c_GraphNode<NodeData>* start, c_SearchHeader<NodeData> *recurseHeader = nullptr) {
	if (start == nullptr) {
		return {};
	}
	std::vector<c_GraphNode<NodeData>*> output_list = {};
	
	bool owns_header = false;
	c_SearchHeader<NodeData> * header = recurseHeader;
	if (header == nullptr) {
		dbgprint("Header is being created.");
		header = new c_SearchHeader<NodeData>;
		owns_header = true;
	}
	
	uint32_t degrees = start->externalDegree();
	
	char iter = '?';
	if (start->index <= 9) {
		iter = ('0' + start->index);
	}
	if (header->testAdd(start->index)) {
		output_list.push_back(start);
		if (start->index <= 9) {
			char debugprnt[] = "BFS level  ";
			debugprnt[10] = iter;
			dbgprint(debugprnt);
		}
	}
	
	for (uint32_t i=0; i < degrees; i++) {
		c_GraphNode<NodeData>* node = start->cnt_out.at(i);
		// Load each node that isn't already registered.
		if (node != nullptr && header->testAdd(node->index)) {
			if (i < 10) {
				char printable[] = " Added degr   from index  ";
				printable[12] = '0' + i;
				printable[25] = iter;
				dbgprint(printable);
			}
			output_list.push_back(node);
			header->visit_queue.push_back(node); // this was the culprit of 20 mins of debugging. forgot to add the node to the queue.
		}
	}
	std::vector<c_GraphNode<NodeData>*> dequeue, newqueue;
	// Dequeue all pending jobs.
	do {
		// Go through all dequeues until one returns new data (skip redundant calls)
		while (dequeue.empty() && !header->visit_queue.empty()) {
			c_GraphNode<NodeData>* handle = header->visit_queue.at(0);
			header->visit_queue.erase(header->visit_queue.begin()); // erase that entry BEFORE we call this to prevent recursion
			dequeue = devTraverseBfs(handle, header);
		}
		for (c_GraphNode<NodeData>* i : dequeue) {
			if (i != nullptr) { //this is mostly a sanity check, but still important
				newqueue.push_back(i);
				output_list.push_back(i);
			}
		}
		dequeue.clear();
		// Exclusively for the "owner" level, keep dequeueing until physically empty.
		// Recursive ones will break once the visit_queue empties
		if (owns_header && header->visit_queue.empty() && !newqueue.empty()) {
			header->visit_queue = newqueue;
			if (newqueue.size() < 10) {
				char tmpextra[] = "Adding new queue of size  ";
				tmpextra[25] = '0' + newqueue.size();
				dbgprint(tmpextra);
			} else {
				dbgprint("Adding big queue.");
			}
			newqueue = {};
		}
	} while (!header->visit_queue.empty());
	if (owns_header) {
		delete header;
		return output_list;
	} else {
		// Assign the next level's queue (the collected nodes from dequeueing this level)
		if (!newqueue.empty()) {
			dbgprint("Leaving a level.");
			header->visit_queue = newqueue;
		}
		return output_list;
	}
}


//! Helper function for running a Breadth-First "Filtered" traversal, from the specified node. Runs recursively!
//! The function must return TRUE to add it to the output vector, and must take two parameters: the last-level node as the first parameter,  and the currently-tested node as the second.
//! The search function must be prepared to handle null pointers, almost exclusively for the first node alone ("last-level" will be nullptr).
template<typename NodeData> std::vector<c_GraphNode<NodeData>*> devTraverseBfs_Filt(c_GraphNode<NodeData>* start, bool (* searchFunc)(c_GraphNode<NodeData>*, c_GraphNode<NodeData>*), c_SearchHeader<NodeData> *recurseHeader = nullptr) {
	if (start == nullptr) {
		return {};
	}
	std::vector<c_GraphNode<NodeData>*> output_list = {};
	
	bool owns_header = false;
	c_SearchHeader<NodeData> * header = recurseHeader;
	if (header == nullptr) {
		dbgprint("Header is being created.");
		header = new c_SearchHeader<NodeData>;
		owns_header = true;
	}
	
	uint32_t degrees = start->externalDegree();
	
	char iter = '?';
	if (start->index <= 9) {
		iter = ('0' + start->index);
	}
	if (header->testAdd(start->index)) {
		if (owns_header) {
			if (searchFunc(nullptr, start))
				output_list.push_back(start);
		}
		if (start->index <= 9) {
			char debugprnt[] = "BFS level  ";
			debugprnt[10] = iter;
			dbgprint(debugprnt);
		}
	}
	
	for (uint32_t i=0; i < degrees; i++) {
		c_GraphNode<NodeData>* node = start->cnt_out.at(i);
		// Load each node that isn't already registered.
		if (node != nullptr && header->testAdd(node->index)) {
			if (i < 10) {
				char printable[] = " Added degr   from index  ";
				printable[12] = '0' + i;
				printable[25] = iter;
				dbgprint(printable);
			}
			if (searchFunc(start, node)) {
				output_list.push_back(node);
			}
			header->visit_queue.push_back(node); // this was the culprit of 20 mins of debugging. forgot to add the node to the queue.
		}
	}
	std::vector<c_GraphNode<NodeData>*> dequeue, newqueue;
	// Dequeue all pending jobs.
	do {
		// Go through all dequeues until one returns new data (skip redundant calls)
		while (dequeue.empty() && !header->visit_queue.empty()) {
			c_GraphNode<NodeData>* handle = header->visit_queue.at(0);
			header->visit_queue.erase(header->visit_queue.begin()); // erase that entry BEFORE we call this to prevent recursion
			dequeue = devTraverseBfs_Filt(handle, searchFunc, header);
		}
		for (c_GraphNode<NodeData>* i : dequeue) {
			if (i != nullptr) { //this is mostly a sanity check, but still important
				newqueue.push_back(i);
				// We've already tested anything that gets returned.
				output_list.push_back(i);
			}
		}
		dequeue.clear();
		// Exclusively for the "owner" level, keep dequeueing until physically empty.
		// Recursive ones will break once the visit_queue empties
		if (owns_header && header->visit_queue.empty() && !newqueue.empty()) {
			header->visit_queue = newqueue;
			if (newqueue.size() < 10) {
				char tmpextra[] = "Adding new queue of size  ";
				tmpextra[25] = '0' + newqueue.size();
				dbgprint(tmpextra);
			} else {
				dbgprint("Adding big queue.");
			}
			newqueue = {};
		}
	} while (!header->visit_queue.empty());
	if (owns_header) {
		delete header;
		return output_list;
	} else {
		// Assign the next level's queue (the collected nodes from dequeueing this level)
		if (!newqueue.empty()) {
			dbgprint("Leaving a level.");
			header->visit_queue = newqueue;
		}
		return output_list;
	}
}

//! Runs a Depth-First traversal from the specified node.
template<typename NodeData> std::vector<c_GraphNode<NodeData>*> devTraverseDfs(c_GraphNode<NodeData>* start, c_SearchHeader<NodeData>* recurseHeader = nullptr) {
	if (start == nullptr) {
		return {};
	}
	std::vector<c_GraphNode<NodeData>*> output_list = {};
	
	bool owns_header = false;
	c_SearchHeader<NodeData> * header = recurseHeader;
	if (header == nullptr) {
		dbgprint("Header is being created.");
		header = new c_SearchHeader<NodeData>;
		owns_header = true;
	}
	
	uint32_t degrees = start->externalDegree();

	// We add this node before traversing as far as possible
	if (header->testAdd(start->index)) {
		output_list.push_back(start);
	}
	std::vector<c_GraphNode<NodeData>*> dequeue;
	for (uint32_t i=0; i < degrees; i++) {
		c_GraphNode<NodeData>* node = start->cnt_out.at(i);
		if (header->testAdd(node->index)) {
			output_list.push_back(node);
			dequeue = devTraverseDfs(node, header);
			for (c_GraphNode<NodeData>* i : dequeue) {
				output_list.push_back(i);
			}
		}
	}
	if (owns_header) {
		delete header;
	}
	return output_list;
}


//! Runs a Depth-First filtered traversal from the specified node.
//! The function must return TRUE to add it to the output vector, and must take two parameters: the last-level node as the first parameter,  and the currently-tested node as the second.
//! The search function must be prepared to handle null pointers, almost exclusively for the first node alone ("last-level" will be nullptr).
template<typename NodeData> std::vector<c_GraphNode<NodeData>*> devTraverseDfs_Filt(c_GraphNode<NodeData>* start, bool (* searchFunc)(c_GraphNode<NodeData>*, c_GraphNode<NodeData>*), c_SearchHeader<NodeData>* recurseHeader = nullptr) {
	if (start == nullptr) {
		return {};
	}
	std::vector<c_GraphNode<NodeData>*> output_list = {};
	
	bool owns_header = false;
	c_SearchHeader<NodeData> * header = recurseHeader;
	if (header == nullptr) {
		dbgprint("Header is being created.");
		header = new c_SearchHeader<NodeData>;
		owns_header = true;
	}
	
	uint32_t degrees = start->externalDegree();

	// We add this node before traversing as far as possible
	if (header->testAdd(start->index)) {
		if (owns_header && searchFunc(nullptr, start)) {
			output_list.push_back(start);
		} else {
			output_list.push_back(start); // We'll filter it on the upper level in this version, because of the "two node" search function
		}
	}
	std::vector<c_GraphNode<NodeData>*> dequeue;
	for (uint32_t i=0; i < degrees; i++) {
		c_GraphNode<NodeData>* node = start->cnt_out.at(i);
		if (header->testAdd(node->index)) {
			if (searchFunc(start, node))
				output_list.push_back(node);
			dequeue = devTraverseDfs_Filt(node, searchFunc, header);
			for (c_GraphNode<NodeData>* i : dequeue) {
				if (searchFunc(node, i))
					output_list.push_back(i);
			}
		}
	}
	if (owns_header) {
		delete header;
	}
	return output_list;
}

/********!
 * @class c_GraphNode
 * 
 * @brief
 * Represents a combined graph structure, with multiple means of construction and with some utility handlers. Supports the
 * storage and optimization of up to 4,294,967,290 unique nodes.
 * 
 * @date
 * 30 October 2024
 ********/
template<typename NodeData> class c_Graph {
	std::vector<c_GraphNode<NodeData>*> raw_ptrs;
	std::vector<c_GraphCnt> calc_cnts;
	std::vector<uint32_t> degrees, indexes; // BOTH of these vectors are aligned with the order of raw_ptrs, not the node indexes!
	uint32_t count = 0;
	// Allows ID mapping, assuming that 'indexes' and 'raw_ptrs' are in the same order (true when constructed).
	// Returns -1 (4294967293 for uint32_t) if the ID provided is not in the vector.
	uint32_t idxMap(uint32_t ID) {
		for (uint32_t i=0; i < indexes.size(); i++) {
			if (indexes.at(i) == ID) {
				return i;
			}
		}
		return -1;
	}
public:
	//! Initialize an empty graph structure.
	c_Graph() {}
	//! Initialize a pre-filled graph structure, where all nodes share the same value. All connections added must be specified.
	//! Connections will be processed and nodes will be created as needed for the indexes present.
	//! Report connections, at least for the initializer list, as {ID_from, ID_to, priority} or just {ID_from, ID_to}.
	c_Graph(const NodeData prefill, std::initializer_list<c_GraphCnt> connectionList) {
		for (c_GraphCnt TMP : connectionList) {
			uint32_t i = idxMap(TMP.from);
			if ((i == 4294967293) || (i == (uint32_t)(-1))) {
				indexes.push_back(TMP.from);
				raw_ptrs.push_back(new c_GraphNode<NodeData>(TMP.from, prefill));
				i = indexes.size() - 1;
			}
			uint32_t j = idxMap(TMP.to);
			if ((j == 4294967293) || (j == (uint32_t)(-1))) {
				indexes.push_back(TMP.to);
				raw_ptrs.push_back(new c_GraphNode<NodeData>(TMP.to, prefill));
				j = indexes.size() - 1;
			}
			raw_ptrs.at(i)->establishCnt(raw_ptrs.at(j));
			calc_cnts.push_back(TMP);
			// Hit upper limit for nodes in some fashion
			if (i >= 4294967290 || j >= 4294967290) {
				break;
			}
		}
		count = raw_ptrs.size();
		calcDegrees();
	}
	// later, add one htat lets you generate data based on a function; function takes the node's index as only param, and returns a valid NodeData
	
	//! Deletes the structure, calling the @c delete method for each node to prevent memory leaks.
	~c_Graph() {
		for (uint32_t i=0; i < this->count; i++) {
			delete this->raw_ptrs.at(i);
		}
		this->raw_ptrs.clear();
	}
	//! Calculates the external degree of each node in the structure, and returns the largest encountered.
	uint32_t calcDegrees() {
		this->degrees.clear();
		uint32_t largest = 0;
		for (uint32_t i=0; i < this->count; i++) {
			uint32_t tmp = (this->raw_ptrs.at(i)->externalDegree());
			if (tmp > largest) largest = tmp;
			this->degrees.push_back(tmp);
		}
		return largest;
	}
	
	/********!
	 * @date	30 October 2024
	 * @brief
	 *  	Clears the internal tracking list of connections, then iterates through every single node stored in the graph to
	 * 		determine their connections, removing any duplicates found from those nodes.
	 * @return
	 *  	The newly-determined number of connections in the graph structure.
	 * @note
	 *  	This will reset all connection priorities to zero. If using priorities, use the overload such that the priorities
	 *  	can be recalculated, unless you intentionally want them all to be zero.
	 ********/
	uint32_t optimizeCnts() {
		this->calc_cnts.clear();
		for (uint32_t i=0; i < this->count; i++) {
			c_GraphNode<NodeData>* tmp = this->raw_ptrs.at(i);
			
			// Test for node-level external duplicates and expunge them.
			std::vector<uint32_t> marked = {};
			std::vector<c_GraphNode<NodeData>*> to_sever = {};
			uint32_t minidx = -1, maxidx = 0;
			for (c_GraphNode<NodeData>* node : tmp->cnt_out) {
				bool good = true;
				uint32_t erridx = 0;
				// See if we can avoid the expensive iteration loop by seeing if it's out of (existing) range.
				if (node->index < minidx) {
					minidx = node->index;
				} else if (node->index > maxidx) {
					maxidx = node->index;
				} else {
					for (uint32_t j : marked) {
						if (node->index == j) {
							good = false;
							erridx = j;
							break;
						}
					}
				}
				if (good) {
					marked.push_back(node->index);
					c_GraphCnt outtmp = {tmp->index, node->index};
					this->calc_cnts.push_back(outtmp);
				} else {
					to_sever.push_back(node);
				}
			}
			for (c_GraphNode<NodeData>* dupl : to_sever) {
				tmp->severCnt(dupl);
			}
		}
		return this->calc_cnts.size();
	}

	/********!
	 * @date	30 October 2024
	 * @brief
	 *  	Clears the internal tracking list of connections, then iterates through every single node stored in the graph to
	 *  	determine their connections, removing any duplicates found from those nodes. Assigns connection priorities based
	 *  	on the provided function.
	 * @param [in] prioFunc
	 *  	The function or lambda to run, which must take the "From" node pointer as its first parameter, and the "To" node
	 *  	as its second parameter, and which must return a uint8_t value for the priority of that connection. The function
	 *  	must not throw an exception, and must not modify the nodes (which are passed as constant pointers).
	 * @return
	 *  	The newly-determined number of connections in the graph structure.
	 ********/
	uint32_t optimizeCnts(uint8_t (* const prioFunc)(const c_GraphNode<NodeData>*, const c_GraphNode<NodeData>*)) {
		this->calc_cnts.clear();
		for (uint32_t i=0; i < this->count; i++) {
			c_GraphNode<NodeData>* tmp = this->raw_ptrs.at(i);
			
			// Test for node-level external duplicates and expunge them.
			std::vector<uint32_t> marked = {};
			std::vector<c_GraphNode<NodeData>*> to_sever = {};
			uint32_t minidx = -1, maxidx = 0;
			for (c_GraphNode<NodeData>* node : tmp->cnt_out) {
				bool good = true;
				uint32_t erridx = 0;
				// See if we can avoid the expensive iteration loop by seeing if it's out of (existing) range.
				if (node->index < minidx) {
					minidx = node->index;
				} else if (node->index > maxidx) {
					maxidx = node->index;
				} else {
					for (uint32_t j : marked) {
						if (node->index == j) {
							good = false;
							erridx = j;
							break;
						}
					}
				}
				if (good) {
					marked.push_back(node->index);
					uint8_t prio = prioFunc(tmp, node);
					c_GraphCnt outtmp = {tmp->index, node->index, prio};
					this->calc_cnts.push_back(outtmp);
				} else {
					to_sever.push_back(node);
				}
			}
			for (c_GraphNode<NodeData>* dupl : to_sever) {
				tmp->severCnt(dupl);
			}
		}
		return this->calc_cnts.size();
	}
	
	/********!
	 * @date	31 October 2024
	 * @brief
	 *  	Traverses the entire Graph in a Breadth-First Search fashion from the indicated initial node; that is, it will
	 * 		sequentially register the directed connections from each node, and then search through each of those connection's
	 * 		target node, until all possible paths have been traversed.
	 * @param [in] index_start
	 *  	The node index (ID) to begin the BFS from.
	 * @return
	 *  	The organized breadth-first pointer vector.
	 ********/
	std::vector<c_GraphNode<NodeData>*> runBreadthFirst(uint32_t index_start) {
		uint32_t init = this->idxMap(index_start);
		if ((init == 4294967293) || (init == (uint32_t)(-1))) {
			return {};
		}
		c_GraphNode<NodeData>* start_parent = this->raw_ptrs.at(init);
		std::vector<c_GraphNode<NodeData>*> output = devTraverseBfs(start_parent);
		return output;
	}
	/********!
	 * @date	2 November 2024
	 * @brief
	 *  	Traverses the entire Graph in a Breadth-First Search fashion from the indicated initial node; that is, it will
	 * 		sequentially register the directed connections from each node, and then search through each of those connection's
	 * 		target node, until all possible paths have been traversed. Filters the output results based on the searchFunc.
	 * @param [in] index_start
	 *  	The node index (ID) to begin the BFS from.
	 * @param [in] searchFunc
	 *  	The boolean function to use for filtering the output. It must take two parameters: the previously-visited node,
	 *  	and then the current node in question. The current node will be added to the output vector if this function
	 *  	returns true. The function must be able to handle nullptr nodes, primarily for handling the topmost node.
	 * @return
	 *  	The organized breadth-first pointer vector.
	 ********/
	std::vector<c_GraphNode<NodeData>*> runBreadthFirst(uint32_t index_start, bool (* searchFunc)(c_GraphNode<NodeData>*, c_GraphNode<NodeData>*)) {
		uint32_t init = this->idxMap(index_start);
		if ((init == 4294967293) || (init == (uint32_t)(-1))) {
			return {};
		}
		c_GraphNode<NodeData>* start_parent = this->raw_ptrs.at(init);
		std::vector<c_GraphNode<NodeData>*> output = devTraverseBfs_Filt(start_parent, searchFunc);
		return output;
	}
	
	/********!
	 * @date	2 November 2024
	 * @brief
	 *  	Traverses the entire Graph in a Depth-First Search fashion from the indicated initial node; that is, it will try
	 *  	to completely traverse all of a node's connections as soon as it is added to the output list.
	 * @param [in] index_start
	 *  	The node index (ID) to begin the DFS from.
	 * @return
	 *  	The organized depth-first pointer vector.
	 ********/
	std::vector<c_GraphNode<NodeData>*> runDepthFirst(uint32_t index_start) {
		uint32_t init = this->idxMap(index_start);
		if ((init == 4294967293) || (init == (uint32_t)(-1))) {
			return {};
		}
		c_GraphNode<NodeData>* start_parent = this->raw_ptrs.at(init);
		std::vector<c_GraphNode<NodeData>*> output = devTraverseDfs(start_parent);
		return output;
	}
	/********!
	 * @date	2 November 2024
	 * @brief
	 *  	Traverses the entire Graph in a Depth-First Search fashion from the indicated initial node; that is, it will try
	 *  	to completely traverse all of a node's connections as soon as it is added to the output list.  Filters the output
	 *  	results based on the searchFunc.
	 * @param [in] index_start
	 *  	The node index (ID) to begin the DFS from.
	 * @param [in] searchFunc
	 *  	The boolean function to use for filtering the output. It must take two parameters: the previously-visited node,
	 *  	and then the current node in question. The current node will be added to the output vector if this function
	 *  	returns true. The function must be able to handle nullptr nodes, primarily for handling the topmost node.
	 * @return
	 *  	The organized depth-first pointer vector.
	 ********/
	std::vector<c_GraphNode<NodeData>*> runDepthFirst(uint32_t index_start, bool (* searchFunc)(c_GraphNode<NodeData>*, c_GraphNode<NodeData>*)) {
		uint32_t init = this->idxMap(index_start);
		if ((init == 4294967293) || (init == (uint32_t)(-1))) {
			return {};
		}
		c_GraphNode<NodeData>* start_parent = this->raw_ptrs.at(init);
		std::vector<c_GraphNode<NodeData>*> output = devTraverseDfs_Filt(start_parent, searchFunc);
		return output;
	}
};

}

#endif
