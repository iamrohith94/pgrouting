--q1 Checking dead end contraction for a graph with no dead end vertex
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = ANY(ARRAY[2, 4, 5, 8])',
    ARRAY[1]::integer[], 1, ARRAY[]::BIGINT[], true);
--q1 -------------------------------------------

--q2 Checking dead end contraction for a graph with dead start
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 12',
    ARRAY[1]::integer[], 1, ARRAY[]::BIGINT[], true);
--q2 -------------------------------------------

--q3 Checking dead end contraction for a graph with base case dead end
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 2',
    ARRAY[1]::integer[], 1, ARRAY[]::BIGINT[], true);
--q3 -------------------------------------------

--q4 Checking dead end contraction for a graph with dead end(bidirectional)
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 8',
    ARRAY[1]::integer[], 1, ARRAY[]::BIGINT[], true);
--q4 -------------------------------------------

--q5 Checking dead end contraction for a graph with multiple dead starts
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = ANY(ARRAY[11, 12])',
    ARRAY[1]::integer[], 1, ARRAY[]::BIGINT[], true);
--q5 -------------------------------------------

--q6 Checking dead end contraction for a graph with multiple dead ends
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = ANY(ARRAY[2, 3])',
    ARRAY[1]::integer[], 1, ARRAY[]::BIGINT[], true);
--q6 -------------------------------------------

--q7 Checking dead end contraction for a graph with multiple dead ends(bidirectional)
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = ANY(ARRAY[6, 7, 8, 9])',
    ARRAY[1]::integer[], 1, ARRAY[]::BIGINT[], true);
--q7 -------------------------------------------

--q8 Checking dead end contraction for a graph with multiple dead ends(bidirectional) with forbidden vertices
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = ANY(ARRAY[6, 7, 8, 9])',
    ARRAY[1]::integer[], 1, ARRAY[5,6]::BIGINT[], true);
--q8 -------------------------------------------
