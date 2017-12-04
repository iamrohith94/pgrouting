
\echo -- q1
SELECT * FROM pgr_performanceAnalysis('select id, source, target, cost, reverse_cost from edge_table',
	'd' ,ARRAY[2], ARRAY[8], 3);

\echo -- q1
SELECT * FROM pgr_performanceAnalysis('select id, source, target, cost, reverse_cost from edge_table',
	'd' ,ARRAY[3, 4, 6], ARRAY[6, 3, 9], 5);

