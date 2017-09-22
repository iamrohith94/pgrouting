ADD_LIBRARY(components OBJECT
    connectedComponents.c
	strongComponents.c
	biconnectedComponents.c
    articulationPoints.c
    bridges.c
    makeConnected.c

    connectedComponents_driver.cpp
	strongComponents_driver.cpp
    biconnectedComponents_driver.cpp
    articulationPoints_driver.cpp
    bridges_driver.cpp
    makeConnected_driver.cpp)
