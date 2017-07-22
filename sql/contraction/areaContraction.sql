DROP FUNCTION pgr_areaContraction(TEXT,ANYARRAY,BOOLEAN); 
CREATE OR REPLACE FUNCTION pgr_areaContraction(
    edges_sql TEXT,
    boundary_vids ANYARRAY,
    directed BOOLEAN DEFAULT true,

    OUT id BIGINT,
    OUT source BIGINT,
    OUT target BIGINT,
    OUT cost FLOAT)
RETURNS SETOF RECORD AS
$body$
DECLARE
  paths_query TEXT;
  groupby_query TEXT;
  join_query TEXT;
  final_query TEXT;
BEGIN
    paths_query := 'SELECT * FROM pgr_dijkstra(' || quote_literal(edges_sql) || ', ' || 
    quote_literal(boundary_vids) || '::BIGINT[]' || ', ' || quote_literal(boundary_vids) || '::BIGINT[]' || ', ' || directed || ') WHERE edge != -1';
    groupby_query := 'SELECT edge as id, node as source, cost FROM a GROUP BY id, source, cost';
    join_query := 'SELECT b.id AS id, b.source AS source,
    CASE WHEN c.source = b.source THEN c.target 
    ELSE c.source END AS target, b.cost AS cost
    FROM b JOIN c on b.id = c.id';
    final_query := 'WITH 
    a AS (' || paths_query || '), 
    b AS ('|| groupby_query || '),
    c AS (' || edges_sql || '),
    d AS ('|| join_query || ') 
    SELECT * FROM d';
    RETURN QUERY EXECUTE final_query;
    
END
$body$ language plpgsql volatile;