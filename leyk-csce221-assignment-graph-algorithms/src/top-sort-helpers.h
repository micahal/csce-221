#pragma once

#include <unordered_map>

#include "weighted-graph.hpp"
#include "graph-types.h"

template <typename T>
void computeIndegrees(const WeightedGraph<T>& graph, std::unordered_map<value_type<T>, int>& indegrees) {
    // TODO store the indegree for each vertex in the graph in the indegrees map
    for (auto i = graph.begin(); i != graph.end(); i++) {
        indegrees[i->first] = 0;
    }
    for (const_iterator<T> i = graph.begin(); i != graph.end(); ++i) {
        vertex_type<T> u = i->first;
        const adjacency_list<T>& adj_list = graph.at(u);
    
        for (const_edge_iterator<T> j = adj_list.begin(); j != adj_list.end(); ++j) {
            vertex_type<T> v = j->first;
            weight_type<T> w = j->second;
    
            // Do Stuff With Edge (u, v, w)
            if (indegrees.find(v) == indegrees.end()) {
                throw std::out_of_range("malformed graph");
            }
            indegrees[v]++;
        }
    }

}
