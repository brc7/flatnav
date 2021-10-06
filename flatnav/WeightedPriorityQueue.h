#pragma once

#include <cstdlib>
#include <vector>
#include <climits>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <utility> 

/*
This is an implementation of a weighted priority queue
based on linked lists.

Doing skip lists would provide an asymptotic O(logN)
increment/decrement operator, but in practice I do not think
the extra list hierarchy is necessary because there are few
elements with nonzero weights.

-BC
*/


template<typename node_id_t>
class WeightedPriorityQueue{

	typedef std::unordered_map<node_id_t, int> map_t;

	struct Node {
		node_id_t key;
		float priority;
		int left;
		int right;
	};

	std::vector<Node> linked_list;
	int head_node_index;
	int tail_node_index;
	int N;
	map_t index_table;

	void unstitch_node(int index){
		// unstitches the node at array location "index" from the linkedlist
		// searching the linkedlist won't return this node anymore, but the 
		// node isn't deleted - just not involved in search
		int left = linked_list[index].left;
		int right = linked_list[index].right;
		if (left >= 0)  // there is a node to the left
			linked_list[left].right = right;
		if (right >= 0)  // there is a node to the right
			linked_list[right].left = left;
		if (left == -1) // this was (previously) the head node
			head_node_index = right;
		if (right == -1) // this was (previously) the tail node
			tail_node_index = left;
		linked_list[index].left = -1;
		linked_list[index].right = -1;
		N = N - 1;
	}

	void insert_node(int node_index){
		// inserts the node at array location "index" to the linkedlist
		// with its currently-set priority
		int current = head_node_index;
		float node_priority = linked_list[node_index].priority;
		if (current < 0){
			// if list is empty, and current
			head_node_index = node_index;
			linked_list[node_index].left = -1;
			linked_list[node_index].right = -1;
			return;
		}
		// do the search to find insertion point
		while((linked_list[current].priority > node_priority) &&
			  (current >= 0)){
			// if node we visited has a higher priority AND
			// node is not the tail (i.e. right is nonzero)
			current = linked_list[current].right;  // traverse!
		}
		// at this point we have either found the tail or found 
		// the insertion point - the first node whose priority
		// is smaller than node_priority
		if (current < 0){
			// we are at the tail, so we must replace the old tail with node
			linked_list[node_index].left = tail_node_index;
			linked_list[node_index].right = -1;
			linked_list[tail_node_index].right = node_index;
			tail_node_index = node_index;
		} else if (current == head_node_index){
			// we are at the head, and have to replace the head
			linked_list[node_index].left = -1;
			linked_list[node_index].right = head_node_index;
			linked_list[head_node_index].left = node_index;
			head_node_index = node_index;
		} else {
			// we are not at the tail, so we must stitch both sides
			int right_neighbor = current;
			int left_neighbor = linked_list[current].left;
			linked_list[node_index].left = left_neighbor;
			linked_list[node_index].right = right_neighbor;
			linked_list[right_neighbor].left = node_index;
			linked_list[left_neighbor].right = node_index;
		}
		N = N + 1;
	}

	void init_list(){
		head_node_index = 0;
		tail_node_index = linked_list.size()-1;
		for (int i = 0; i < linked_list.size(); i++){
			linked_list[i].left = i-1;
			if (i < (linked_list.size()-1)){
				linked_list[i].right = i+1;
			} else {
				linked_list[i].right = -1;
			}
		}
	}

	public:
	WeightedPriorityQueue(const std::vector<node_id_t>& nodes){
		for (int i = 0; i < nodes.size(); i++){
			linked_list.push_back({nodes[i], 0, -1, -1});
			index_table[nodes[i]] = i;
		}
		init_list();
		N = linked_list.size();
	}

	WeightedPriorityQueue(size_t _N){
		for (int i = 0; i < _N; i++){
			linked_list.push_back({i, 0, -1, -1});
			index_table[i] = i;
		}
		init_list();
		N = linked_list.size();
	}

	void print(){
		int current = head_node_index;
		while(current >= 0){
			std::cout<<"("<<linked_list[current].key<<":"<<linked_list[current].priority<<")"<<" ";
			// " ["<<linked_list[current].left<<","<<linked_list[current].right<<"] "<<
			current = linked_list[current].right;
		}
		std::cout<<std::endl;
	}


	void increment(node_id_t key, float inc){
		// 0. Find internal index of node referred to as "key"
		typename map_t::const_iterator it = index_table.find(key);
		if (it == index_table.end()){
			return;
		}

		int index = it->second;
		// 1. Unstitch this node from the linked list
		unstitch_node(index);

		// 2. Update the priority
		linked_list[index].priority = linked_list[index].priority + inc;

		// 3. Insert node (but with new priority) back into list
		insert_node(index);
	}

	void decrement(node_id_t key, float inc){
		// 0. Find internal index of node referred to as "key"
		typename map_t::const_iterator it = index_table.find(key);
		if (it == index_table.end()){
			return;
		}

		int index = it->second;
		// 1. Unstitch this node from the linked list
		unstitch_node(index);

		// 2. Update the priority
		linked_list[index].priority = linked_list[index].priority - inc;

		// 3. Insert node (but with new priority) back into list
		insert_node(index);
	}
	
	node_id_t pop(){
		int index = head_node_index;
		node_id_t key = linked_list[head_node_index].key;
		unstitch_node(index);
		index_table.erase(key);
		return key;
	}

	size_t size(){
		return N;
	}

};
