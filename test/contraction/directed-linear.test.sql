--q1 Checking for linear contraction with no linear vertices
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 11 OR id = 12',
    ARRAY[2]::integer[], 1, ARRAY[]::BIGINT[], true);
--q1 -------------------------------------------

-- q2 Checking linear contraction for the base case(one incoming and one outgoing and one linear node)
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 2 OR id = 3',
    ARRAY[2]::integer[], 1, ARRAY[]::BIGINT[], true);
-- q2 -------------------------------------------

-- q3 Checking linear contraction for two incoming and two outgoing and one linear node
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 8 OR id = 9',
    ARRAY[2]::integer[], 1, ARRAY[]::BIGINT[], true);
-- q3 -------------------------------------------

-- q4 Checking linear contraction for one incoming and one outgoing and two linear nodes
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 3 OR id = 5 OR id = 11',
    ARRAY[2]::integer[], 1, ARRAY[]::BIGINT[], true);
-- q4 -------------------------------------------

-- q5 Checking linear contraction for two incoming and two outgoing and two linear nodes
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 4 OR id = 8 OR id = 9',
    ARRAY[2]::integer[], 1, ARRAY[]::BIGINT[], true);
-- q5 -------------------------------------------

-- q6 Checking linear contraction for multiple linear nodes
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 4 OR id = 8 OR id = 9 OR id = 11 OR id = 13 OR id = 15 OR id = 16',
    ARRAY[2]::integer[], 1, ARRAY[]::BIGINT[], true);
-- q6 -------------------------------------------

-- q7 Checking linear contraction for multiple linear nodes with forbidden vertices
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 4 OR id = 8 OR id = 9 OR id = 11 OR id = 13 OR id = 15 OR id = 16',
    ARRAY[2]::integer[], 1, ARRAY[5,11]::BIGINT[], true);
-- q7 -------------------------------------------