/*PGR-GNU*****************************************************************

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Copyright (c) 2017 Maoguang Wang 
Mail: xjtumg1007@gmail.com

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

#ifndef INCLUDE_COMPONENTS_PGR_MAKE_CONNECTED_HPP_
#define INCLUDE_COMPONENTS_PGR_MAKE_CONNECTED_HPP_
#pragma once

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>
#include "cpp_common/pgr_base_graph.hpp"
#include "cpp_common/identifiers.hpp"
#include "cpp_common/basic_edge.h"

#include <vector>
#include <map>
#include <utility>
#include <algorithm>

template < class G > class Pgr_connect;
// user's functions
// for development

//******************************************

template < class G >
class Pgr_connect {
 public:
    typedef typename G::V V;
    typedef typename G::E E;
    typedef typename G::EO_i EO_i;
    typedef typename G::EI_i EI_i;
    typedef typename std::map<int64_t, Identifiers<int64_t> > ComponentVertices;
    
    void getConnections(G &DAG, 
        std::vector<pgr_connections_rt>& connections);

    void makeConnected(G &DAG, 
        std::vector<pgr_connections_rt>& connections);

    void makeStronglyConnected(G &DAG,
        std::vector<pgr_connections_rt>& connections);

    void _makeConnected(G &DAG, 
        ComponentVertices component_vertices,
        std::vector<pgr_connections_rt>& connections);

    void _makeStronglyConnected(G &DAG, 
        ComponentVertices component_vertices,
        std::vector<pgr_connections_rt>& connections);
};

template < class G >
void Pgr_connect< G >::getConnections(G &graph, 
        std::vector<pgr_connections_rt>& connections) {

    if (graph.m_gType == UNDIRECTED) {
        makeConnected(graph, connections);
    }
    else {
        makeStronglyConnected(graph, connections);   
    }


}

template < class G >
void Pgr_connect< G >::makeConnected(G &graph, 
        std::vector<pgr_connections_rt>& connections) {
    size_t totalNodes = graph.num_vertices();
    std::vector< int64_t > components(totalNodes);
    
    //Finding connected components
    int num_comps;
    num_comps = boost::connected_components(graph.graph, &components[0]);
    
    if (num_comps == 1) {
        return ;
    }
    ComponentVertices component_vertices;
    
    for (size_t i = 0; i < totalNodes; i++) {
        component_vertices[components[i]] += graph[i].id;
    }

    pgr_edge_t temp;
    G DAG(graph.m_gType);
    // Constructing the DAG
        for (auto eit = boost::edges(graph.graph).first; 
                    eit != boost::edges(graph.graph).second; ++eit) {
            V s, t;
            s = graph.source(*eit);
            t = graph.target(*eit);
            if (components[s] != components[t]) {
                temp.source = components[s];
                temp.target = components[t];
                temp.id = boost::num_edges(DAG.graph);
                temp.cost = graph[*eit].cost;
                temp.reverse_cost = -1;
                DAG.graph_add_edge(temp);
            }
        }
        _makeConnected(DAG, component_vertices, connections);

}


template < class G >
void Pgr_connect< G >::makeStronglyConnected(G &graph, 
        std::vector<pgr_connections_rt>& connections) {
    size_t totalNodes = graph.num_vertices();
    std::vector< int64_t > components(totalNodes);
    
    //Finding connected components
    int num_comps;
        num_comps =  boost::strong_components(graph.graph,
            boost::make_iterator_property_map(components.begin(),
                                              get(boost::vertex_index,
                                                  graph.graph)));
    
    if (num_comps == 1) {
        return ;
    }
    ComponentVertices component_vertices;
    
    for (size_t i = 0; i < totalNodes; i++) {
        component_vertices[components[i]] += graph[i].id;
    }

    pgr_edge_t temp;
    G DAG(graph.m_gType);
    // Constructing the DAG
        for (auto eit = boost::edges(graph.graph).first; 
                    eit != boost::edges(graph.graph).second; ++eit) {
            V s, t;
            s = graph.source(*eit);
            t = graph.target(*eit);
            if (components[s] != components[t]) {
                temp.source = components[s];
                temp.target = components[t];
                temp.id = boost::num_edges(DAG.graph);
                temp.cost = graph[*eit].cost;
                temp.reverse_cost = -1;
                DAG.graph_add_edge(temp);
            }
        }
        _makeStronglyConnected(DAG, component_vertices, connections);

}


template < class G >
void Pgr_connect< G >::_makeConnected(G &DAG,
        ComponentVertices component_vertices,
        std::vector<pgr_connections_rt>& connections) {
    if (component_vertices.size() <= 1) {
        return;
    }

    pgrouting::Basic_edge shortcut;
    //Adding links to the DAG
    for (auto vi = vertices(DAG.graph).first;
                    vi != vertices(DAG.graph).second; ++vi) {
        if (DAG.out_degree(*vi) == 0) {
            auto from_comp_id = DAG[*vi].id;
            auto to_comp_id = (DAG[*vi].id +1)%DAG.num_vertices(); 
            connections.push_back({component_vertices[from_comp_id].front(),
            component_vertices[to_comp_id].front()});
            
            //add edge to DAG
            shortcut.source = from_comp_id;
            shortcut.target = to_comp_id;
            shortcut.id = boost::num_edges(DAG.graph);
            shortcut.cost = 0.001;
            DAG.graph_add_edge(shortcut);

            //adjust component vertices map
            component_vertices[to_comp_id] += component_vertices[from_comp_id];
            component_vertices.erase(from_comp_id);
        }
    }

    _makeConnected(DAG, component_vertices, connections);
    
}





template < class G >
void Pgr_connect< G >::_makeStronglyConnected(G &DAG,
        ComponentVertices component_vertices,
        std::vector<pgr_connections_rt>& connections) {
    if (component_vertices.size() <= 1) {
        return;
    }

    pgr_edge_t shortcut;
    //Adding links to the DAG
    for (auto vi = vertices(DAG.graph).first;
                    vi != vertices(DAG.graph).second; ++vi) {
        int64_t from_comp_id, to_comp_id;
        if (DAG.out_degree(*vi) != 0 && DAG.in_degree(*vi) != 0) {
            continue;
        }
        if (DAG.out_degree(*vi) == 0 && DAG.in_degree(*vi) == 0) {
            from_comp_id = DAG[*vi].id;
            to_comp_id = (DAG[*vi].id +1)%DAG.num_vertices(); 
            //add edge to DAG
            shortcut.cost = 1;
            shortcut.reverse_cost = 1;
        }
        else if(DAG.out_degree(*vi) == 0) {
            from_comp_id = DAG[*vi].id;
            EI_i eit, eit_end;
            boost::tie(eit, eit_end) = in_edges(*vi, DAG.graph);
            to_comp_id = DAG[DAG.source(*eit)].id;
            //add edge to DAG
            shortcut.cost = 1;
            shortcut.reverse_cost = -1;
        }

        else {
            EO_i eit, eit_end;
            boost::tie(eit, eit_end) = out_edges(*vi, DAG.graph);
            from_comp_id = DAG[DAG.target(*eit)].id;
            to_comp_id = DAG[*vi].id;
            //add edge to DAG
            shortcut.cost = 1;
            shortcut.reverse_cost = -1;
        }

        shortcut.source = from_comp_id;
        shortcut.target = to_comp_id;
        shortcut.id = boost::num_edges(DAG.graph);

        connections.push_back({component_vertices[from_comp_id].front(),
            component_vertices[to_comp_id].front()});

        //adding the connection to the graph
        DAG.graph_add_edge(shortcut);

        //adjust component vertices map
        component_vertices[to_comp_id] += component_vertices[from_comp_id];
        component_vertices.erase(from_comp_id);
    }

    _makeStronglyConnected(DAG, component_vertices, connections);
}



#endif  // INCLUDE_COMPONENTS_PGR_MAKE_CONNECTED_HPP_
