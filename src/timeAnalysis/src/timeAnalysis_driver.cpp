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
#include "./timeAnalysis_driver.h"


#include "../../common/src/pgr_alloc.hpp"
#include "./../../common/src/pgr_assert.h"
#include "./../../common/src/pgr_types.h"

#include "./../../dijkstra/src/pgr_dijkstra.hpp"




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
std::deque< pgr_time_analysis_t >
pgr_timeAnalysis(
        G &graph,
        std::vector<int64_t> sources,
        std::vector<int64_t> targets,
        bool only_cost = false) {
    Path path;
    Pgr_dijkstra< G > fn_dijkstra;
    clock_t start_t, end_t;
    std::deque< pgr_time_analysis_t > return_tuples;
    pgr_time_analysis_t temp;

    std::sort(sources.begin(), sources.end());
    sources.erase(
            std::unique(sources.begin(), sources.end()),
            sources.end());

    std::sort(targets.begin(), targets.end());
    targets.erase(
            std::unique(targets.begin(), targets.end()),
            targets.end());

    for (int i = 0; i < sizeof(sources); ++i) {
        temp.source = sources[i];
        temp.target = targets[i];
        start_t = clock();
        fn_dijkstra.dijkstra(graph, sources[i], targets[i], only_cost);
        end_t = clock();
        temp.time_taken = 1000.0 * (end_t-start_t) / CLOCKS_PER_SEC;
        return_tuples.push_back(temp);
    }
    return return_tuples;
}


void
do_pgr_timeAnalysis(
        pgr_edge_t  *data_edges,
        size_t total_edges,
        int64_t* start_vidsArr,
        size_t size_start_vidsArr,
        int64_t* end_vidsArr,
        size_t size_end_vidsArr,
        bool directed,
        bool only_cost,
        pgr_time_analysis_t **return_tuples,
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

        log << "Inserting vertices into a c++ vector structure";
        std::vector<int64_t>
            start_vertices(start_vidsArr, start_vidsArr + size_start_vidsArr);
        std::vector< int64_t >
            end_vertices(end_vidsArr, end_vidsArr + size_end_vidsArr);

        std::deque< pgr_time_analysis_t > time_analysis;
        std::deque< pgr_time_analysis_t >::iterator it;

        if (directed) {
            log << "Working with directed Graph\n";
            pgrouting::DirectedGraph digraph(gType);
            digraph.insert_edges(data_edges, total_edges);
            time_analysis = pgr_timeAnalysis(digraph,
                    start_vertices,
                    end_vertices,
                    only_cost);
        } else {
            log << "Working with Undirected Graph\n";
            pgrouting::UndirectedGraph undigraph(gType);
            undigraph.insert_edges(data_edges, total_edges);
            time_analysis = pgr_timeAnalysis(
                    undigraph,
                    start_vertices,
                    end_vertices,
                    only_cost);
        }

        auto count = time_analysis.size();

        if (count == 0) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            notice <<
                "No paths found between start_vid and end_vid vertices";
            return;
        }
        (*return_tuples) = pgr_alloc(count, (*return_tuples));
        it = time_analysis.begin();
        size_t sequence = 0;
        while (it != time_analysis.end()) {
            (*return_tuples)[sequence] = *it;
            it++;
            sequence++;
        }

        (*return_count) = sequence;

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
