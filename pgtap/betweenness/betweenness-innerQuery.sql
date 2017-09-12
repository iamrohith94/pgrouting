\i setup.sql

SELECT plan(137);
SET client_min_messages TO ERROR;


SELECT has_function('pgr_betweenness',
    ARRAY['text', 'bigint', 'bigint', 'boolean','boolean']);

SELECT function_returns('pgr_betweenness',
    ARRAY['text', 'bigint', 'bigint', 'boolean','boolean'],
    'setof record');

SELECT style_dijkstra('pgr_betweenness', ', 2, 3)');
SELECT style_dijkstra('pgr_betweenness', ', 2, 3, true)');
SELECT style_dijkstra('pgr_betweenness', ', 2, 3, false)');



SELECT finish();
ROLLBACK;
