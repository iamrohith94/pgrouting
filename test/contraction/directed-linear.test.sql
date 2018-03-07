
-- q1 Checking linear contraction for multiple linear nodes with forbidden vertices not in the graph
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 4 OR id = 8 OR id = 9 OR id = 11 OR id = 13 OR id = 15 OR id = 16',
    ARRAY[2]::integer[], 1, ARRAY[]::BIGINT[], true);
-- q1 -------------------------------------------

-- q2 Checking linear contraction for multiple linear nodes with forbidden vertices not in the graph
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = 4 OR id = 8 OR id = 9 OR id = 11 OR id = 13 OR id = 15 OR id = 16',
    ARRAY[2]::integer[], 1, ARRAY[7]::BIGINT[], true);
