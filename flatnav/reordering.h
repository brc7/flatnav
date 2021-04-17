#pragma once

#include <vector>
#include "ExplicitSet.h"
#include <algorithm>
#include <queue>
#include <utility>


// Ben: all algorithms make the following assumptions
// input is a vector of vectors called outdegree_table, where outdegree_table[node] is a vector of the outbound edges from node
// template parameter is the type of nodes in the table. This should be an integral type. Nodes in the graph should be labeled from 
// 0 to N-1 (where N is the number of nodes in the graph) with no non-existent nodes
// 
// All functions must accept outdegree_table and return a permutation P. P is a length-N vector where
// P[i] is the new node ID of the node currently labeled "i". That is, to find the new label of node "i", we 
// look at P[i]



template <typename node_id_t>
std::vector<node_id_t> g_order(std::vector< std::vector<node_id_t> > &outdegree_table, const int w){

	int cur_num_nodes = outdegree_table.size();

	// create table of in-degrees
	std::vector< std::vector<node_id_t> > indegree_table(cur_num_nodes);
	for(node_id_t node = 0; node < cur_num_nodes; node++){
		for (node_id_t& edge : outdegree_table[node]){
			indegree_table[edge].push_back(node);
		}
	}

	GorderPriorityQueue<node_id_t> Q(cur_num_nodes);
	std::vector<node_id_t> P(cur_num_nodes, 0);
	
	node_id_t seed_node = 0;
	Q.increment(seed_node);
	P[0] = Q.pop();

	// for i = 1 to N: 
	for (int i = 1; i < cur_num_nodes; i++){
		node_id_t v_e = P[i-1];
		// ve = newest node in window
		// for each node u in out-edges of ve:
		for (node_id_t& u : outdegree_table[v_e]){
			Q.increment(u);
		}
		// for each node u in in-edges of v_e:
		for (node_id_t& u : indegree_table[v_e]){
			// if u in Q, increment priority of u
			Q.increment(u);
			// for each node v in out-edges of u:
			for (node_id_t& v : outdegree_table[u]){
				Q.increment(v);
			}
		}

		if (i > w+1){
			node_id_t v_b = P[i-w-1];
			// for each node u in out-edges of vb:
			for (node_id_t& u : outdegree_table[v_b]){
				Q.decrement(u);
			}
			
			// for each node u in in-edges of v_b
			for (node_id_t& u : indegree_table[v_b]){
				// if u in Q, increment priority of u
				// it honestly doesn't seem to matter whether this particular operation is an increment or a decrement
				// in my original code, it was "increment" (which is technically wrong) but the performance is basically the same
				Q.decrement(u); 
				// for each node v in out-edges of u: 
				for (node_id_t& v : outdegree_table[u]){
					Q.decrement(v);
				}
			}
		}
		P[i] = Q.pop();
	}

    std::vector<node_id_t> Pinv(cur_num_nodes, 0);
    for (int n = 0; n < cur_num_nodes; n++){
        Pinv[P[n]] = n;
    }
	// now we have a mapping Pinv[i] -> new label of node i
    return Pinv;
}

template <typename node_id_t>
std::vector<node_id_t> indegree_order(std::vector< std::vector<node_id_t> > &outdegree_table){

	int cur_num_nodes = outdegree_table.size();
	// create table of in-degrees
	std::vector< std::pair<node_id_t,int> > indegrees(cur_num_nodes, std::make_pair(0,0));

	for (node_id_t node = 0; node < cur_num_nodes; node++) {
		indegrees[node].first = node;
		for (node_id_t& edge : outdegree_table[node]){
			indegrees[edge].second++;
		}
	}

	std::sort(indegrees.begin(), indegrees.end(), 
		[](const std::pair<node_id_t,int> & a, const std::pair<node_id_t,int> & b){ return a.second > b.second; });
	
	std::vector<node_id_t> P(cur_num_nodes);
	for (node_id_t node = 0; node < cur_num_nodes; node++){
		P[indegrees[node].first] = node;
	}

	return P;
}

template <typename node_id_t>
std::vector<node_id_t> outdegree_order(std::vector< std::vector<node_id_t> > &outdegree_table){

	int cur_num_nodes = outdegree_table.size();
	std::vector< std::pair<node_id_t, int> > outdegrees(cur_num_nodes, std::make_pair(0,0)); 

	for (node_id_t node = 0; node < cur_num_nodes; node++){
		outdegrees[node].first = node;
		outdegrees[node].second = outdegree_table[node].size();
	}

	std::sort(outdegrees.begin(), outdegrees.end(), 
		[](const std::pair<node_id_t, int> & a, const std::pair<node_id_t, int> &b) { return a.second > b.second; } );

	std::vector<node_id_t> P(cur_num_nodes); 
	for (node_id_t node = 0; node < cur_num_nodes; node++){
		P[outdegrees[node].first] = node;
	}

	return P;
}

template <typename node_id_t>
std::vector<node_id_t> hubsort_order(std::vector< std::vector<node_id_t> > &outdegree_table){
	// sorted by in-degree, since kNN is a push implementation

	int cur_num_nodes = outdegree_table.size();
	std::vector< int > indegrees(cur_num_nodes, 0);
	double mean_deg = 0;

	for (node_id_t node = 0; node < cur_num_nodes; node++) {
		for (node_id_t& edge : outdegree_table[node]){
			indegrees[edge]++;
			mean_deg += 1;
		}
	}
	mean_deg = mean_deg / cur_num_nodes;

    std::vector< std::pair<node_id_t,int> > sorted_hubs;
    std::vector<node_id_t> non_hubs;
    std::vector<node_id_t> P;

    for (node_id_t node = 0; node < cur_num_nodes; node++){
        if (indegrees[node] >= mean_deg){
            sorted_hubs.push_back({node, indegrees[node]});
        }
        else{
            non_hubs.push_back(node);
        }
    }

    std::sort(sorted_hubs.begin(), sorted_hubs.end(), [](const std::pair<node_id_t,int> &a, const std::pair<node_id_t,int> &b)
        { return a.second > b.second; });

    for (int i = 0; i < sorted_hubs.size(); i++){
        P.push_back( sorted_hubs[i].first );
    }
    for (int i = 0; i < non_hubs.size(); i++){
        P.push_back( non_hubs[i] );
    }

    std::vector<node_id_t> Pinv(cur_num_nodes, 0);
    for (int n = 0; n < cur_num_nodes; n++){
        Pinv[P[n]] = n;
    }
    return Pinv;
}

template <typename node_id_t>
std::vector<node_id_t> dbg_order(std::vector< std::vector<node_id_t> > &outdegree_table, const int w){

	int cur_num_nodes = outdegree_table.size();
	std::vector< int > indegrees(cur_num_nodes, 0);

	for (node_id_t node = 0; node < cur_num_nodes; node++) {
		for (node_id_t& edge : outdegree_table[node]){
			indegrees[edge]++;
		}
	}
	
	std::vector<int> quantiles(indegrees);
	std::sort(quantiles.begin(), quantiles.end());

	std::vector<int> thresholds(w,0); 
	for (int i = 0; i < w; i++){
		int idx = (i*cur_num_nodes) / w;
		thresholds[i] = quantiles[idx];
	}

	std::vector< std::vector<node_id_t> > assignments(w); 
	std::vector<node_id_t> P; 

	for (node_id_t node = 0; node < cur_num_nodes; node++){
		int deg = indegrees[node];
		int group_number = 0; 

		for (int i = 0; i < w; i++){
			if (deg > thresholds[i]){
				group_number = i;
			}
		}
		assignments[group_number].push_back(node);
	}

	for (int i = 0; i < assignments.size(); i++){
		for (int j = 0; j < assignments[i].size(); j++){
			P.push_back(assignments[i][j]);
		}
	}

	std::reverse(P.begin(), P.end());
	std::vector<node_id_t> Pinv(cur_num_nodes,0);
	for (int n = 0; n < cur_num_nodes; n++){
		Pinv[P[n]] = n;
	}
	return Pinv;
}

template <typename node_id_t>
std::vector<node_id_t> rcm_order(std::vector< std::vector<node_id_t> > &outdegree_table){

	int cur_num_nodes = outdegree_table.size();
	std::vector< std::pair<node_id_t, int> > sorted_nodes; 
	std::vector<int> degrees;
	
	for (node_id_t node = 0; node < cur_num_nodes; node++){
		int deg = outdegree_table[node].size();
		sorted_nodes.push_back({node, deg});
		degrees.push_back(deg);
	}

	std::sort(sorted_nodes.begin(), sorted_nodes.end(),
		[](const std::pair<node_id_t,int> &a, const std::pair<node_id_t,int> &b){return a.second < b.second; });

	std::vector<node_id_t> P;
	ExplicitSet is_listed = ExplicitSet(cur_num_nodes);
	is_listed.clear();

	for (int i = 0; i < sorted_nodes.size(); i++){
		node_id_t node = sorted_nodes[i].first; 
		std::queue<node_id_t> Q; 

		if (!is_listed[node]){
			// add node to permutation
			P.push_back(node); 
			is_listed.insert(node);

			// get list of neighbors
			std::vector< std::pair<node_id_t, int> > neighbors; 
			for (auto& edge : outdegree_table[node]){
				neighbors.push_back( { edge, degrees[edge] } );
			}

			// sort neighbors by degree (min degree first)
			std::sort(neighbors.begin(), neighbors.end(), 
				[](const std::pair<node_id_t,int>&a, const std::pair<node_id_t,int>&b) { return a.second < b.second; });

			// add neighbors to queue
			for (int j = 0; j < neighbors.size(); j++){
				Q.push(neighbors[j].first);
			}

			while (Q.size() > 0){
				// exhause all the neighbors
				node_id_t candidate = Q.front();
				Q.pop(); 
				if (!is_listed[candidate]){
					P.push_back(candidate);
					is_listed.insert(candidate);

					// get list of neighbors of candidate
					std::vector< std::pair<node_id_t,int> > candidate_neighbors; 
					for(auto& edge: outdegree_table[candidate]){
						candidate_neighbors.push_back( {edge, degrees[edge]} );
					}
					// sort neighbors by degree (min degree first)
					std::sort(candidate_neighbors.begin(), candidate_neighbors.end(),
						[](const std::pair<node_id_t,int>&a, const std::pair<node_id_t,int>&b){ return a.second < b.second; });
					// add neighbors to queue
					for (int j = 0; j < candidate_neighbors.size(); j++){
						Q.push(candidate_neighbors[j].first);
					}
				}
			}
		}
	}
    
	std::reverse(P.begin(),P.end());
    std::vector<node_id_t> Pinv(cur_num_nodes, 0);
    for (int n = 0; n < cur_num_nodes; n++){
        Pinv[P[n]] = n;
    }
    return Pinv;
}

template <typename node_id_t> 
std::vector<node_id_t> hubcluster_order(std::vector< std::vector<node_id_t> > &outdegree_table){

	int cur_num_nodes = outdegree_table.size();
	std::vector< int > indegrees(cur_num_nodes, 0);
	double mean_deg = 0;

	for (node_id_t node = 0; node < cur_num_nodes; node++) {
		for (node_id_t& edge : outdegree_table[node]){
			indegrees[edge]++;
			mean_deg += 1;
		}
	}
	mean_deg = mean_deg / cur_num_nodes;

    std::vector<node_id_t> hubs;
    std::vector<node_id_t> non_hubs;
    std::vector<node_id_t> P;

    for (node_id_t node = 0; node < cur_num_nodes; node++){
        if (indegrees[node] >= mean_deg){
            hubs.push_back(node);
        }
        else{
            non_hubs.push_back(node);
        }
    }

    for (int i = 0; i < hubs.size(); i++){
        P.push_back( hubs[i] );
    }
    for (int i = 0; i < non_hubs.size(); i++){
        P.push_back( non_hubs[i] );
    }

    std::vector<node_id_t> Pinv(cur_num_nodes, 0);
    for (int n = 0; n < cur_num_nodes; n++){
        Pinv[P[n]] = n;
    }
    return Pinv;
}


/* Notes:

Gorder Algorithm: 

insert all v into Q each with priority 0
select a start node into P

for i = 1 to N:
    ve = P[i-1] # new node in window
    for each node u in out-edges of ve: 
        if u in Q, increment priority of u
    for each node u in in-edges of ve: 
        if u in Q, increment priority of u
        for each node v in out-edges of u:
            if v in Q, increment priority of v
    if i > w+1: 
        // remove the tailing node
        vb = P[i - w - 1]
        for each node u in out-edges of vb: 
            if u in Q, decrement priority of u
        for each node u in in-edges of fb:
            if u in Q, decrement priority of u
            for each node v in out-edges of u:
                if v in Q, decrement priority of v
    vmax = Q.pop()
    P[i] = vmax
    i++
*/
