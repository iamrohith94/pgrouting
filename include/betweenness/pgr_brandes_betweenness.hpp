/*PGR-GNU*****************************************************************
File: betweenness.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2017 Rohith Reddy
Mail: rohithreddy2219.com


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
#ifndef INCLUDE_BETWEENNESS_PGR_BRANDES_BETWEENNESS_HPP_
#define INCLUDE_BETWEENNESS_PGR_BRANDES_BETWEENNESS_HPP_
#pragma once

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/betweenness_centrality.hpp>
#include <vector>
#include "cpp_common/pgr_base_graph.hpp"
#include "c_types/betweenness_rt.h"



template < class G > class Pgr_brandes_betweenness;



template < class G >
class Pgr_brandes_betweenness {
public:
	typedef typename G::V V;
    typedef typename G::E E;
    typedef typename G::B_G B_G;
    typedef std::map<E, int64_t> StdEdgeIndexMap;
    typedef boost::associative_property_map< StdEdgeIndexMap > EdgeIndexMap;
    typedef typename boost::property_map< B_G, boost::vertex_index_t>::type VertexIndexMap;

    void get_edge_betweenness(const G &graph, 
        std::vector<pgr_betweenness_rt>& edges_container, bool is_parallel) {
        if (!is_parallel)
    		this->edge_betweenness(graph, edges_container);
    	else
    		this->parallel_edge_betweenness(graph, edges_container);
    }

    void edge_betweenness(const G &graph,
    	std::vector<pgr_betweenness_rt>& edges_container) {

        StdEdgeIndexMap my_e_index;
        EdgeIndexMap e_index(my_e_index);   

        int64_t i = 0;


        for (auto eit = boost::edges(graph.graph).first; 
            eit != boost::edges(graph.graph).second; ++eit) {
            my_e_index.insert(std::pair< E, int64_t >(*eit, i));
            ++i;
        }

    	// Define EdgeCentralityMap
        std::vector< double > e_centrality_vec(boost::num_edges(graph.graph), 0.0);
        // Create the external property map
        boost::iterator_property_map< std::vector< double >::iterator, EdgeIndexMap >
            e_centrality_map(e_centrality_vec.begin(), e_index);


        // Define VertexCentralityMap
        VertexIndexMap v_index = get(boost::vertex_index, graph.graph);
        std::vector< double > v_centrality_vec(boost::num_vertices(graph.graph), 0.0);
        // Create the external property map
        boost::iterator_property_map< std::vector< double >::iterator, VertexIndexMap >
                  v_centrality_map(v_centrality_vec.begin(), v_index);
    	
        brandes_betweenness_centrality(graph.graph, v_centrality_map, e_centrality_map);

        for (auto eit = boost::edges(graph.graph).first; 
            eit != boost::edges(graph.graph).second; ++eit) {
                edges_container.push_back({graph[*eit].id, graph[graph.source(*eit)].id,
                    graph[graph.target(*eit)].id, graph[*eit].cost, e_centrality_map[*eit]});
        }
    	
    }

    void parallel_edge_betweenness(const G &graph,
    	std::vector<pgr_betweenness_rt>& edges_container) {
    	
    }
};


#endif  // INCLUDE_BETWEENNESS_PGR_BRANDES_BETWEENNESS_HPP_