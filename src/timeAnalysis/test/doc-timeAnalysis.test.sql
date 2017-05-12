
\echo -- q1
SELECT * FROM pgr_timeAnalysis(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table', 'dijkstra',
    ARRAY[1], ARRAY[3]
);

\echo -- q2
SELECT * FROM pgr_timeAnalysis(
    'SELECT id, source, target, cost, reverse_cost FROM edge_table', 'dijkstra',
    ARRAY[2], ARRAY[3],
    FALSE
);
\echo -- q3

