\echo -- q1
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 1 or id = 3', false
);