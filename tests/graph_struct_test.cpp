// Tester for graph structures and their traversal

#include <iostream>
#include "../Graph.hpp"

// Replica of the one featured on wikipedia here:
// https://en.wikipedia.org/wiki/Depth-first_search
GraphStruct::c_Graph<uint8_t> testgraph_lo(17, {
	{1, 2}, {1, 3}, {1, 4},
	{2, 3}, {2, 4},
	{3, 5}, {3, 6},
	{4, 7},
	{7, 8}, {7, 9}, {7, 12},
	{8, 10}, {8, 11}
});

GraphStruct::c_Graph<uint8_t> testgraph_hi(10, {
	{1, 2}, {1, 3},
	{2, 4},
	{4, 6},
	{6, 8}, {6, 2},
	{3, 5},
	{5, 7},
	{7, 20},
	{8, 10},
	{10, 11},
	{11, 12},
	{12, 13}, {12, 14},
	{13, 15},
	{14, 16}, {14, 9},
	{9, 17}, {9, 20},
	{17, 19}, {17, 18},
	{18, 19}, {18, 17},
	{19, 20}
});

int main() {
	std::cout << "Starting traversal attempts.\n";
	auto output_traversal = testgraph_lo.runDepthFirst(1);
	std::cout << "Output is size " << output_traversal.size() << '\n';
	for (GraphStruct::c_GraphNode<uint8_t>* i : output_traversal) {
		std::cout << i->index << ' ';
	}
	std::cout << '\n' << std::flush;
	output_traversal = testgraph_lo.runBreadthFirst(1);
	std::cout << "Output is size " << output_traversal.size() << '\n';
	for (GraphStruct::c_GraphNode<uint8_t>* i : output_traversal) {
		std::cout << i->index << ' ';
	}
	std::cout << '\n' << '\n' << std::flush;
	
	output_traversal = testgraph_hi.runDepthFirst(1);
	std::cout << "Output is size " << output_traversal.size() << '\n';
	for (GraphStruct::c_GraphNode<uint8_t>* i : output_traversal) {
		std::cout << i->index << ' ';
	}
	std::cout << '\n' << std::flush;
	output_traversal = testgraph_hi.runBreadthFirst(1);
	std::cout << "Output is size " << output_traversal.size() << '\n';
	for (GraphStruct::c_GraphNode<uint8_t>* i : output_traversal) {
		std::cout << i->index << ' ';
	}
	std::cout << '\n' << std::flush;
	return 0;
}
