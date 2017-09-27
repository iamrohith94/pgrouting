\echo -- q1
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 1', false
);

\echo -- q2
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 1');

\echo -- q3
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 1 OR id = 3', false
);

\echo -- q4
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 1 OR id = 3');

\echo -- q5
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 2 OR id = 4 OR id = 5 OR id = 8', false);

\echo -- q6
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 2 OR id = 4 OR id = 5 OR id = 8');

\echo -- q7
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 2 OR id = 4 OR id = 5 OR id = 8 OR id = 10 OR id = 11 OR id = 12', false);

\echo -- q8
SELECT * FROM pgr_makeConnected(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table WHERE id = 2 OR id = 4 OR id = 5 OR id = 8 OR id = 10 OR id = 11 OR id = 12');