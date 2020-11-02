//
// Created by lois on 31/10/2020.
//

#ifndef SEMIDISCRETE_MESHMANIP_H
#define SEMIDISCRETE_MESHMANIP_H

#include <geogram/mesh/mesh.h>

inline double getFacetArea(const GEO::Mesh &m, int face){
    double area = 0;
    int nbVertices = m.facets.nb_vertices(face);
    for (int v = 0; v < nbVertices; ++v){
        int pta = m.facets.vertex(face, v);
        int ptb = m.facets.vertex(face, (v+1) % nbVertices);
        area += m.vertices.point(pta).x * m.vertices.point(ptb).y
                -  m.vertices.point(pta).y * m.vertices.point(ptb).x;
    }

    return 0.5 * area;

}

inline void getFacetCentroid(const GEO::Mesh &m, int face, double *p){

    p[0] = 0.;
    p[1] = 0.;
    int nbVertices = m.facets.nb_vertices(face);
    for (int v = 0; v < nbVertices; ++v){
        int pta = m.facets.vertex(face, v);
        int ptb = m.facets.vertex(face, (v+1) % nbVertices);
        double ptax = m.vertices.point(pta).x;
        double ptay = m.vertices.point(pta).y;
        double ptbx = m.vertices.point(ptb).x;
        double ptby = m.vertices.point(ptb).y;
        p[0] += (ptax + ptbx) * (ptax * ptby - ptay * ptbx);
        p[1] += (ptay + ptby) * (ptax * ptby - ptay * ptbx);
    }

    double area = getFacetArea(m, face);
    p[0] /= 6. * area;
    p[1] /= 6. * area;

}

inline int getCorrespondingPoint(const GEO::Mesh &m, int face){
    int* id = (int*)m.facets.attributes().find_attribute_store("chart")->data();
    //return 0;
    return id[face];
}

#endif //SEMIDISCRETE_MESHMANIP_H
