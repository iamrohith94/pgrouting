--q1 Checking for contraction with no dead end and only linear vertices
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = ANY(ARRAY[2, 4, 5, 8])',
    ARRAY[1,2]::integer[], 1, ARRAY[]::BIGINT[], true);
--q1 -------------------------------------------

--q2 Checking for contraction with dead end and no linear vertices
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = ANY(ARRAY[11, 12, 13])',
    ARRAY[1,2]::integer[], 1, ARRAY[]::BIGINT[], true);
--q2 -------------------------------------------

--q3 Checking for contraction with dead end and linear vertices
SELECT * FROM pgr_contractGraph(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table 
    WHERE id = ANY(ARRAY[1, 2, 4, 5, 8])',
    ARRAY[1,2]::integer[], 1, ARRAY[]::BIGINT[], true);
--q3 -------------------------------------------