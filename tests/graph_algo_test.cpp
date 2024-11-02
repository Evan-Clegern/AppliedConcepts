#include <iostream>
#include "./AppliedConcepts/Graph.hpp"

GraphStruct::c_Graph<uint8_t> testgraph_maze(1, {
	{1, 2}, {1, 3},
	{2, 1}, {2, 4},
	{3, 1}, {3, 5}, {3, 6},
	{4, 2}, {4, 7},
	{5, 3}, {5, 8},
	{6, 3}, {6, 8},
	{7, 4}, {7, 9},
	{8, 5}, {8, 6}, {8, 10},
	{9, 7}, {9, 10},
	{10, 11},
	// Add redundant ones to test the optimizeCnt function
	{2, 1}, {2, 1},
	{3, 5}, {5, 3}
});

uint8_t testPrioFunc(const GraphStruct::c_GraphNode<uint8_t>* from, const GraphStruct::c_GraphNode<uint8_t>* to) {
	if (to->index > from->index) {
		return (to->index - from->index);
	} else {
		return (from->index - to->index);
	}
}

int main() {
	auto output = testgraph_maze.runBreadthFirst(1);
	for (GraphStruct::c_GraphNode<uint8_t>* i : output) {
		std::cout << i->index << ' ';
	}
	std::cout << '\n';
	auto connects = testgraph_maze.viewConnections();
	for (GraphStruct::c_GraphCnt i : connects) {
		std::cout << i.from << '-' << i.to << ':' << (int)(i.priority) << ' ';
	}
	std::cout << '\n';
	std::cout << testgraph_maze.optimizeCnts(testPrioFunc) << " connections present.\n";
	output = testgraph_maze.runBreadthFirst(1);
	for (GraphStruct::c_GraphNode<uint8_t>* i : output) {
		std::cout << i->index << ' ';
	}
	std::cout << '\n';
	connects = testgraph_maze.viewConnections();
	for (GraphStruct::c_GraphCnt i : connects) {
		std::cout << i.from << '-' << i.to << ':' << (int)(i.priority) << ' ';
	}
	std::cout << '\n';
	return 0;
}
