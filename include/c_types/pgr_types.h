/*PGR-GNU*****************************************************************
File: pgr_types.h

Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: vicky_vergara@hotmail.com

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
/*! @file */

#ifndef INCLUDE_C_TYPES_PGR_TYPES_H_
#define INCLUDE_C_TYPES_PGR_TYPES_H_
#pragma once


#ifdef __cplusplus

#include <cstddef>

#else  // __cplusplus

// for bool
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-pedantic"
#endif

#include <postgres.h>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// For NULL & size_t
#include <stdlib.h>


#endif  // __cplusplus

// For int64_t etc
#include <stdint.h>


typedef struct {
    int64_t vertex_id;
    int64_t edge_id;
    double cost;
} path_element_t;



typedef struct {
    int seq;
    int64_t source;
    int64_t edge;
    double cost;
} pgr_path_t;


/**************************************************************************
 * return type for contraction
 * ***********************************************************************/
typedef struct {
    int64_t id;
    char* type;
    int64_t source;
    int64_t target;
    double cost;
    int64_t *contracted_vertices;
    int contracted_vertices_size;
} pgr_contracted_blob;

#endif  // INCLUDE_C_TYPES_PGR_TYPES_H_
