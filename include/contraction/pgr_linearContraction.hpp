/*PGR-GNU*****************************************************************
File: pgr_linear.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer: 
Copyright (c) 2016 Rohith Reddy
Mail: 

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

#ifndef INCLUDE_CONTRACTION_PGR_LINEARCONTRACTION_HPP_
#define INCLUDE_CONTRACTION_PGR_LINEARCONTRACTION_HPP_
#pragma once


#include <queue>
#include <functional>
#include <vector>

#include "cpp_common/identifiers.hpp"


namespace pgrouting {
namespace contraction {

template < class G >
class Pgr_linear {
 private:
     typedef typename G::V V;
     typedef typename G::E E;
     typedef typename G::V_i V_i;
     typedef typename G::E_i E_i;
     typedef typename G::EO_i EO_i;
     typedef typename G::EI_i EI_i;
     typedef typename G::degree_size_type degree_size_type;


 public:
     Pgr_linear():last_edge_id(-1) {}
     void setForbiddenVertices(
             Identifiers<V> forbidden_vertices);
     void calculateVertices(G &graph, std::ostringstream& debug);
     void doContraction(G &graph, std::ostringstream& debug);

 private:
     int64_t get_next_id() {
         return --last_edge_id;
     }
     bool is_linear(G &graph, V v, std::ostringstream& debug);
     void add_if_linear(G &graph, V v);
     void add_edge_pair(V vertex, int64_t &incoming_eid,
             int64_t &outgoing_eid);

     bool is_shortcut_possible(G &graph, V v, V left, V right);

     void add_shortcut(G &graph, V vertex,
             V v1,
             V v2,
             std::ostringstream& debug);
     void add_shortcut(G &graph, V vertex,
             E incoming_edge,
             E outgoing_edge,
             std::ostringstream& debug);
     void add_shortcut(G &graph,
             pgrouting::CH_edge &shortcut);

    void add_shortcuts(G &graph,
        std::vector<CH_edge>& shortcuts);

    void append_shortcut(
            G &graph, V vertex,
            E incoming_edge,
            E outgoing_edge,
            std::vector<CH_edge>& shortcuts,
            std::ostringstream& debug);


 private:
     Identifiers<V> linearVertices;
     Identifiers<V> forbiddenVertices;

     int64_t last_edge_id;
     std::ostringstream debug;
};

/*************** IMPLEMENTTION **************/

template < class G >
void
Pgr_linear< G >::setForbiddenVertices(
        Identifiers<V> forbidden_vertices) {

    debug << "Setting forbidden vertices\n";
    forbiddenVertices = forbidden_vertices;
}

/*
Possibility of a shortcut from left vertex to right vertex
*v* should be a linear vertex
*/
template < class G >
bool
Pgr_linear< G >::is_shortcut_possible(G &graph, V v, V left_vertex, V right_vertex) {
    auto in_degree_from_left_vertex = graph.in_degree_from_vertex(v, left_vertex);
    auto out_degree_to_right_vertex = graph.out_degree_to_vertex(v, right_vertex);
    auto in_degree_from_right_vertex = graph.in_degree_from_vertex(v, right_vertex);
    auto out_degree_to_left_vertex = graph.out_degree_to_vertex(v, left_vertex);
    return (in_degree_from_left_vertex > 0 && out_degree_to_right_vertex > 0 
           && in_degree_from_right_vertex == 0 && out_degree_to_left_vertex == 0)
            || (in_degree_from_left_vertex == 0 && out_degree_to_right_vertex == 0 
           && in_degree_from_right_vertex > 0 && out_degree_to_left_vertex > 0)
            || (in_degree_from_left_vertex > 0 && out_degree_to_right_vertex > 0 
           && in_degree_from_right_vertex > 0 && out_degree_to_left_vertex > 0);
}


template < class G >
bool Pgr_linear<G>::is_linear(G &graph, V v, std::ostringstream& debug) {
#ifndef NDEBUG
    debug << "Is linear: " << graph.graph[v].id << "?\n";
#endif

    if (forbiddenVertices.has(v)) {
        /**
         * - fobbiden_vertices
         *   - Not considered as linear
         */

        return false;
    }
    /*
    m in edges
    n out edges
    m >=1 && n >=1 
    in edges should not have contracted vertices
    out edges should not have contracted vertices
    */
    bool adjacent_vertices_constraint = false;
    bool degree_constraint = false;
    bool contracted_vertices_constraint = false;
    
    // Checking adjacent vertices constraint
    auto adjacent_vertices = graph.get_adjacent_vertices(v);
    #ifndef NDEBUG
        debug << "Adjacent vertices: " << adjacent_vertices.size() << std::endl;
    #endif
    if (adjacent_vertices.size() == 2) {
        adjacent_vertices_constraint = true;

        // Checking degree constraint
        V left_vertex = adjacent_vertices.front();
        adjacent_vertices.pop_front();
        V right_vertex = adjacent_vertices.front();
        adjacent_vertices.pop_front();

        #if 0
        bool left_to_right = is_shortcut_possible(graph, v, left_vertex, right_vertex);
        #ifndef NDEBUG
        debug << "Left to right : " << left_to_right << std::endl;
        #endif

        bool right_to_left = is_shortcut_possible(graph, v, right_vertex, left_vertex);

        #ifndef NDEBUG
        debug << "Right to left : " << right_to_left << std::endl;
        #endif
        #endif
        if (is_shortcut_possible(graph, v, left_vertex, right_vertex)) {
            degree_constraint = true;
            #if 0
            // Checking contracted vertices constraint
            for(auto incident_edge : graph.get_incident_edges(v)) {
                if (graph[incident_edge].has_contracted_vertices()) {
                    return false;
                }
            }
            #ifndef NDEBUG
            debug << "Contracted vertices : " << contracted_vertices_constraint << std::endl;
            #endif
            contracted_vertices_constraint = true;
            #endif
            return true;
        }
        return false;
    }

    #ifndef NDEBUG
    debug << "Is Not Linear\n";
    #endif
    return false;
}

template < class G >
void Pgr_linear<G>::calculateVertices(G &graph, std::ostringstream& debug) {
#ifndef NDEBUG
    debug << "Calculating vertices\n";
#endif
    V_i vi;
    for (vi = vertices(graph.graph).first;
            vi != vertices(graph.graph).second;
            ++vi) {
#ifndef NDEBUG
        //debug << "Checking vertex " << graph[(*vi)].id << '\n';
#endif
        if (is_linear(graph, *vi, debug)) {

#ifndef NDEBUG
        debug << "Adding " << graph[(*vi)].id << " to linear" << '\n';
#endif
            linearVertices += (*vi);
        }
    }
    linearVertices -= forbiddenVertices;

#ifndef NDEBUG
        debug << "Number of linear vertices: " << linearVertices.size() << '\n';
#endif

}


template < class G >
void
Pgr_linear<G>::add_if_linear(G &graph, V v) {
    if (is_linear(graph, v, debug)) {
        linearVertices += v;
    }
}



template < class G >
void Pgr_linear<G>::add_shortcuts(G &graph,
        std::vector<CH_edge>& shortcuts) {
    for (auto shortcut : shortcuts) {
        graph.add_shortcut(shortcut);
    }
}



template < class G >
void Pgr_linear<G>::append_shortcut(
        G &graph, V vertex,
        E incoming_edge,
        E outgoing_edge,
        std::vector<CH_edge>& shortcuts,
        std::ostringstream& debug) {

        auto in_vertex = graph.adjacent(vertex, incoming_edge);
        auto out_vertex = graph.adjacent(vertex, outgoing_edge);
        pgassert(in_vertex != vertex);
        pgassert(in_vertex != out_vertex);
        pgassert(vertex != out_vertex);

        CH_edge shortcut(
                get_next_id(),
                graph[in_vertex].id,
                graph[out_vertex].id,
                graph[incoming_edge].cost + graph[outgoing_edge].cost);


        shortcut.add_contracted_vertex(graph[vertex]);
        shortcut.add_contracted_edge_vertices(graph[incoming_edge]);
        shortcut.add_contracted_edge_vertices(graph[outgoing_edge]);

        debug << "appending shortcut: " << shortcut << std::endl;
        shortcuts.push_back(shortcut);
}

template < class G >
void Pgr_linear<G>::doContraction(G &graph, std::ostringstream& debug) {
    EI_i in, in_end;
    EO_i out, out_end;
#ifndef NDEBUG
    debug << "Performing linear contraction\n";
#endif
    std::priority_queue<V, std::vector<V>, std::greater<V> > linearPriority;
    std::vector<CH_edge> shortcuts;

    for (V linearVertex : linearVertices) {
        linearPriority.push(linearVertex);
    }

#ifndef NDEBUG
    #if 0
    debug << "Linear vertices" << std::endl;
    for (V v : linearVertices) {
        debug << graph[v].id << ", ";
    }
    debug << std::endl;
    #endif
#endif

    shortcuts.clear();
    while (!linearPriority.empty()) {
        V current_vertex = linearPriority.top();
	debug << "Remaining linear vertex count: " << linearPriority.size() << std::endl;
	debug << "Current linear vertex: " << graph[current_vertex].id << std::endl;
        linearPriority.pop();

        if (!is_linear(graph, current_vertex, debug)) {
            continue;
        }

        Identifiers<V> adjacent_vertices =
            graph.get_adjacent_vertices(current_vertex);
        pgassert(adjacent_vertices.size() == 2);

        V v_1 = adjacent_vertices.front();
        adjacent_vertices.pop_front();
        V v_2 = adjacent_vertices.front();
        adjacent_vertices.pop_front();

#ifndef NDEBUG
	#if 0
        debug << "Adjacent vertices\n";
        debug << graph[vertex_1].id
            << ", " << graph[vertex_2].id
            << std::endl;
	#endif
#endif


        if (graph.m_gType == DIRECTED) {

            #if 1
            /*
            Adding edge for every in edge and every out edge
            */
            for (boost::tie(in, in_end) = boost::in_edges(current_vertex, graph.graph);
                    in != in_end; ++in) {
                for (boost::tie(out, out_end) = boost::out_edges(current_vertex, graph.graph);
                        out != out_end; ++out) {
                    //append_shortcut(graph, current_vertex, *in, *out, shortcuts, debug);
                    if (graph.source(*in) == graph.target(*out)) {
                        continue;
                    }
                    add_shortcut(graph, current_vertex, *in, *out, debug);
                }       
            }
            #endif
            #if 0
            add_shortcut(graph, current_vertex, v_1, v_2, debug);
            add_shortcut(graph, current_vertex, v_2, v_1, debug);
            #endif


        } else {
            /*
                write logic afterwards
            */
            #if 0
            if (graph.out_degree_to_vertex(vertex_1, current_vertex) > 0 &&
                    graph.in_degree_from_vertex(vertex_2, current_vertex) > 0) {
                //debug << "UNDIRECTED graph before contraction\n";
                graph.print_graph(contraction_debug);
                E e1 = graph.get_min_cost_edge(vertex_1,
                        current_vertex);
                E e2 = graph.get_min_cost_edge(current_vertex,
                        vertex_2);
                add_shortcut(graph, current_vertex, e1, e2);
            }
            #endif
        }

        //debug << "Disconnecting vertex "  << std::endl;

        graph.disconnect_vertex(current_vertex);
        graph[current_vertex].clear_contracted_vertices();

        //debug << "After removing vertex the graph is: " << graph << std::endl;
        //add_shortcuts(graph, shortcuts);


        linearVertices -= current_vertex;
        if (is_linear(graph, v_1, debug)
                && !forbiddenVertices.has(v_1) && !linearVertices.has(v_1)) {

            debug << "Adding linear vertex: " << graph[v_1].id << std::endl;
            linearPriority.push(v_1);
            linearVertices += v_1;
        }
        if (is_linear(graph, v_2, debug)
                && !forbiddenVertices.has(v_2) && !linearVertices.has(v_2)) {
            linearPriority.push(v_2);
            debug << "Adding linear vertex: " << graph[v_2].id << std::endl;
            linearVertices += v_2;
        }
    }
    //debug << contraction_debug.str().c_str() << "\n";
}



template < class G >
void Pgr_linear<G>::add_shortcut(G &graph, V v,
             V v_1,
             V v_2,
             std::ostringstream& debug) {
    if (is_shortcut_possible(graph, v, v_1, v_2)) {

        debug << "Adding shortcut from " << graph[v_1].id << " to " << graph[v_2].id << std::endl;
        // Get min cost incoming edge from left
        auto min_cost_incoming  = graph.get_min_cost_edge(v_1, v);
        debug << "Min cost incoming: " << graph[min_cost_incoming].id << std::endl;

        // Get min cost incoming edge from right
        auto min_cost_outgoing  = graph.get_min_cost_edge(v, v_2);
        debug << "Min cost outgoing: " << graph[min_cost_outgoing].id << std::endl;

        // Add a shortcut
        add_shortcut(graph, v, min_cost_incoming, min_cost_outgoing, debug);
    }
}

/*! \brief add edges(shortuct) to the graph during contraction

  a --incomming--> b ---outgoing--> c

  a -> c

  edge (a, c) is a new edge: @b shortcut
  e.contracted_vertices = b + b.contracted vertices
  b is "removed" disconnected from the graph
  - by removing all edges to/from b
  */
#if 1
template < class G >
void Pgr_linear<G>::add_shortcut(
        G &graph, V vertex,
        E incoming_edge,
        E outgoing_edge,
        std::ostringstream& debug) {
    /*pgassert(incoming_edge != outgoing_edge);
    auto in_vertex = graph.adjacent(vertex, incoming_edge);
    auto out_vertex = graph.adjacent(vertex, outgoing_edge);
    
    pgassert(in_vertex != vertex);
    pgassert(in_vertex != out_vertex);
    pgassert(vertex != out_vertex);
    
    if (in_vertex == vertex ||
        in_vertex == out_vertex ||
        vertex == out_vertex || 
        incoming_edge == outgoing_edge)
        return;
    */
    CH_edge shortcut(
            get_next_id(),
            graph[graph.source(incoming_edge)].id,
            graph[graph.target(outgoing_edge)].id,
            graph[incoming_edge].cost + graph[outgoing_edge].cost);


    shortcut.add_contracted_vertex(graph[vertex]);
    shortcut.add_contracted_edge_vertices(graph[incoming_edge]);
    shortcut.add_contracted_edge_vertices(graph[outgoing_edge]);

    
    debug << "Adding shortcut\n" << shortcut << std::endl;
    graph.add_shortcut(shortcut);
}


template < class G >
void Pgr_linear<G>::add_shortcut(G &graph,
        pgrouting::CH_edge &shortcut) {
    graph.add_shortcut(shortcut);
}
#endif



}  // namespace contraction
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_PGR_LINEARCONTRACTION_HPP_
