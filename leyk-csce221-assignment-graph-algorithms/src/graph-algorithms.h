#pragma once

#include <istream>
#include <ostream>
#include <sstream>
#include <limits>
#include <list>
#include <queue>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "weighted-graph.hpp"

#include "graph-types.h"

// if the arrow is a box, change to the other line
#define ARROW_SEPARATOR " \u2192 "
// #define ARROW_SEPARATOR " -> "

#include "top-sort-helpers.h"

/**
 * @brief Returns a Topological Ordering of the Graph - https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search
 *
 * @tparam T type of data stored by a vertex
 * @param graph graph upon which to perform a topological ordering
 * @return std::list<value_type<T>> list of nodes in a topological order, or an empty list if no such ordering exists
 */
template <typename T>
std::list<value_type<T>> topologicalSort(const WeightedGraph<T>& graph)
{
    std::unordered_map<value_type<T>, int> indegrees;
    std::unordered_map<value_type<T>, int> topological_numbers;

    // TODO implement Topological Sort

    std::list<value_type<T>> sort;
    computeIndegrees(graph, indegrees);
    std::queue<value_type<T>> q;
    for (auto temp : indegrees) {
        if (temp.second == 0) {
            q.push(temp.first);
        }
    }
    while (!q.empty()) {
        auto x = q.front();
        q.pop();
        sort.push_back(x);
        for (const_edge_iterator<T> j = graph.begin(x); j != graph.end(x); ++j) {
            vertex_type<T> v = j->first;
            weight_type<T> w = j->second;
    
            // Do Stuff With Edge (u, v, w)
            indegrees[v]--;
            if (indegrees[v] == 0) {
                q.push(v);
            }
        }
        
    }
    if (sort.size() != graph.size()) {
        return {};
    }

    return sort;
}

// contains initializeSingleSource, relax, & updateHeap as well as the DijkstraComaparator
#include "dijkstras-helpers.h"

/**
 * @brief Dijkstra's Algorithm - https://canvas.tamu.edu/courses/136654/files/35930572/preview Slide 16
 *
 * @tparam T type of data stored by a vertex
 * @param graph weighted, directed graph to find single-source shortest-path
 * @param initial_node source node in graph for shortest path
 * @param destination_node destination node in graph for shortest path
 * @return std::list<value_type<T>> list of nodes along shortest path including initial_node and destination_node, empty if no path exists
 */
template <typename T>
std::list<value_type<T>> dijkstrasAlgorithm(const WeightedGraph<T>& graph, vertex_type<T> initial_node, vertex_type<T> destination_node)
{
    std::unordered_map<value_type<T>, weight_type<T>> distances;
    std::unordered_map<value_type<T>, std::optional<value_type<T>>> predecessors;
    std::unordered_set<value_type<T>> s;
    std::priority_queue<value_type<T>, std::vector<value_type<T>>, DijkstraComparator<T>> q(DijkstraComparator<T>{distances});

    // TODO implement Dijkstra's Algorithm
    graph.at(initial_node); 
    graph.at(destination_node); 
    initializeSingleSource(graph, initial_node, distances, predecessors);
    for (const_iterator<T> i = graph.begin(); i != graph.end(); ++i) {
        vertex_type<T> u = i->first;
        q.push(u);
    }
    while (!q.empty()) {
        vertex_type<T> u = q.top();
        q.pop();
        if (distances[u] == std::numeric_limits<weight_type<T>>::max()) {
            break;
        }
        s.insert(u);
        const adjacency_list<T>& adj_list = graph.at(u);
        for (const_edge_iterator<T> j = adj_list.begin(); j != adj_list.end(); ++j) {
            vertex_type<T> v = j->first;
            weight_type<T> w = j->second;
            if (s.find(v) != s.end()) {
                continue;
            }
            if (relax<T>(u, v, w, distances, predecessors)) {
                updateHeap(q, distances);
            }
        }
    }


    // TODO create list by walking backwards through predecessors from the end
    std::list<value_type<T>> path;
    value_type<T> curr = destination_node;
    while (predecessors[curr] != std::nullopt) {
        path.push_front(curr);
        curr = predecessors[curr].value();
    }
    if (path.empty() && initial_node != destination_node) {
        return {};
    }
    else {
        path.push_front(initial_node);
    }

    return path;
}

template <typename T>
std::ostream &operator<<(std::ostream &o, const WeightedGraph<T> &graph)
{
    for (auto it = graph.begin(); it != graph.end(); ++it)
    {
        const vertex_type<T> &vertex = it->first;
        const adjacency_list<T> &list = it->second;
        o << vertex << ": ";
        for (auto iit = list.begin(); iit != list.end(); ++iit)
        {
            const vertex_type<T> &destination = iit->first;
            const weight_type<T> &weight = iit->second;
            o << destination << "(" << weight << ')';
            auto next_iter = iit;
            if (++next_iter != list.end())
            {
                o << ARROW_SEPARATOR;
            }
        }
        auto next_iter = it;
        if (++next_iter != graph.end())
        {
            o << "\n";
        }
    }
    return o;
}

template <typename T>
std::istream &readEdge(std::istream &i, value_type<T> &vertex, weight_type<T> &weight)
{
    std::string s_vertex, s_weight;
    std::getline(i, s_vertex, '(');
    std::getline(i, s_weight, ')');
    std::istringstream stream(s_vertex + " " + s_weight);
    stream >> vertex >> weight;
    if (stream.fail())
    {
        i.setf(stream.flags());
    }
    return i;
}

template <typename T>
std::istream &operator>>(std::istream &i, WeightedGraph<T> &graph)
{
    std::string line;
    while (std::getline(i, line))
    {
        if (line.empty())
            break;
        std::istringstream line_stream(line);
        value_type<T> vertex;
        std::string s_vertex;
        std::getline(line_stream, s_vertex, ':');
        std::istringstream stream_vertex(s_vertex);
        stream_vertex >> vertex;
        if (stream_vertex.fail())
        {
            i.setf(stream_vertex.flags());
            break;
        }
        graph.push_vertex(vertex);
        std::string separator;
        value_type<T> edge_end;
        weight_type<T> edge_weight;
        while (readEdge<T>(line_stream, edge_end, edge_weight))
        {
            line_stream >> separator;
            graph.push_edge(vertex, edge_end, edge_weight);
        }
    }

    if(i.eof() and i.fail())
        i.clear(std::ios::eofbit);
    
    return i;
}
