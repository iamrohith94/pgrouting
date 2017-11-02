/*PGR-GNU*****************************************************************
File: timeAnalysis_driver.cpp
Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org
Function's developer: 
Copyright (c) 2017 Rohith Reddy
Mail: rohithreddy2219@gmail.com
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


#include <sstream>
#include <deque>
#include <vector>
#include <ctime>
#include "drivers/performanceAnalysis/performanceAnalysis_driver.h"


#include "cpp_common/pgr_alloc.hpp"
#include "cpp_common/pgr_assert.h"
//#include "./../../common/src/pgr_types.h"

#include "dijkstra/pgr_dijkstra.hpp"




/************************************************************
  edges_sql TEXT,
    algorithm TEXT,
        start_vids ANYARRAY,
    end_vids ANYARRAY,
    directed BOOLEAN DEFAULT true,
    only_cost BOOLEAN DEFAULT false,
 ***********************************************************/

template < class G >
static
std::deque< Performance_analysis_t >
pgr_performanceAnalysis(
        G &graph,
        std::vector<int64_t> sources,
        std::vector<int64_t> targets,
        size_t num_iterations,
        double graph_build_time,
        std::ostringstream& log,
        bool only_cost = false) {
    Pgr_dijkstra< G > fn_dijkstra;
    clock_t start_t, end_t;
    std::deque< Performance_analysis_t > return_tuples;
    Performance_analysis_t temp;

    /*std::sort(sources.begin(), sources.end());
    sources.erase(
            std::unique(sources.begin(), sources.end()),
            sources.end());
    std::sort(targets.begin(), targets.end());
    targets.erase(
            std::unique(targets.begin(), targets.end()),
            targets.end());
    */

    temp.graph_build_time = graph_build_time;
    temp.num_vertices = graph.num_vertices();
    temp.num_edges = boost::num_edges(graph.graph);
    for (size_t i = 0; i < sources.size(); ++i) {
        if (sources[i] == targets[i])
            continue;
        Path path;
        temp.source = sources[i];
        temp.target = targets[i];

        temp.avg_computation_time = 0.0000;
        for (size_t j = 0; j < num_iterations; ++j) {
            start_t = clock();
            path = fn_dijkstra.dijkstra(graph, sources[i], targets[i], only_cost);
            end_t = clock();
            temp.avg_computation_time += (double)(1000.0 * (end_t-start_t) / CLOCKS_PER_SEC);
            temp.path_len = path.tot_cost();
            log << "tot_cost: " << temp.path_len << std::endl;
            path.clear();
        }
        temp.avg_computation_time /= num_iterations;   
        temp.path_len /= num_iterations;
        return_tuples.push_back(temp);
    }
    return return_tuples;
}


void
do_pgr_performanceAnalysis(
        pgr_edge_t  *data_edges,
        size_t total_edges,
        char* algorithm,
        int64_t* start_vidsArr,
        size_t size_start_vidsArr,
        int64_t* end_vidsArr,
        size_t size_end_vidsArr,
        size_t num_iterations,
        bool directed,
        bool only_cost,
        Performance_analysis_t **return_tuples,
        size_t *return_count,
        char ** log_msg,
        char ** notice_msg,
        char ** err_msg) {
    std::ostringstream log;
    std::ostringstream err;
    std::ostringstream notice;
    try {
        pgassert(!(*log_msg));
        pgassert(!(*notice_msg));
        pgassert(!(*err_msg));
        pgassert(!(*return_tuples));
        pgassert(*return_count == 0);
        pgassert(total_edges != 0);

        graphType gType = directed? DIRECTED: UNDIRECTED;
        clock_t start_t, end_t;
        double build_time;

        log << "Size of start_vertices: " << size_start_vidsArr << std::endl;
        log << "Start vertices" << std::endl;
        for (size_t i = 0; i < size_start_vidsArr; ++i) {
            log << start_vidsArr[i] << std::endl;
        }
        log << "Size of end_vertices: " << size_end_vidsArr << std::endl;
        log << "End vertices" << std::endl;
        for (size_t i = 0; i < size_end_vidsArr; ++i) {
            log << end_vidsArr[i] << std::endl;
        }
        log << "Inserting vertices into a c++ vector structure";
        std::vector<int64_t>
        start_vertices(start_vidsArr, start_vidsArr + size_start_vidsArr);
        std::vector<int64_t>
        end_vertices(end_vidsArr, end_vidsArr + size_end_vidsArr);

        log << "Size of start_vertices: " << start_vertices.size() << std::endl;
        log << "Size of end_vertices: " << end_vertices.size() << std::endl;

        std::deque< Performance_analysis_t > performance_details;
        std::deque< Performance_analysis_t >::iterator it;

        if (directed) {
            log << "Working with directed Graph\n";
            pgrouting::DirectedGraph digraph(gType);
            start_t = clock();
            digraph.insert_edges(data_edges, total_edges);
            end_t = clock();
            build_time = (double)(1000.0 * (end_t-start_t) / CLOCKS_PER_SEC);
            performance_details = pgr_performanceAnalysis(digraph,
                    start_vertices,
                    end_vertices,
                    num_iterations,
                    build_time,
                    log,
                    only_cost);
        } else {
            log << "Working with Undirected Graph\n";
            pgrouting::UndirectedGraph undigraph(gType);
            start_t = clock();
            undigraph.insert_edges(data_edges, total_edges);
            end_t = clock();
            build_time = (double)(1000.0 * (end_t-start_t) / CLOCKS_PER_SEC);
            performance_details = pgr_performanceAnalysis(
                    undigraph,
                    start_vertices,
                    end_vertices,
                    num_iterations,
                    build_time,
                    log,
                    only_cost);
        }

        auto count = performance_details.size();

        if (count == 0) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            notice <<
                "No paths found between start_vid and end_vid vertices";
            return;
        }
        (*return_tuples) = pgr_alloc(count, (*return_tuples));
        for (size_t i = 0; i < count; i++) {
            *((*return_tuples) + i) = performance_details[i];
        }
        (*return_count) = count;

        pgassert(*err_msg == NULL);
        *log_msg = log.str().empty()?
            *log_msg :
            pgr_msg(log.str().c_str());
        *notice_msg = notice.str().empty()?
            *notice_msg :
            pgr_msg(notice.str().c_str());
    } catch (AssertFailedException &except) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = pgr_msg(err.str().c_str());
        *log_msg = pgr_msg(log.str().c_str());
    } catch (std::exception &except) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = pgr_msg(err.str().c_str());
        *log_msg = pgr_msg(log.str().c_str());
    } catch(...) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << "Caught unknown exception!";
        *err_msg = pgr_msg(err.str().c_str());
        *log_msg = pgr_msg(log.str().c_str());
    }
}