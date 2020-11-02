//
// Created by lois on 31/10/2020.
//

#ifndef SEMIDISCRETE_IODIAGRAM_H
#define SEMIDISCRETE_IODIAGRAM_H

#include <vector>
#include <string>
#include <geogram/mesh/mesh.h>

#include "simple_svg_1.0.0.hpp"
#include "meshManip.h"

svg::Color getColor(int i){
    int r = 56 + (1354941 * i + 865641) % 200;
    int g = 56 + (3728175 * i + 542681) % 200;
    int b = 56 + (4529786 * i + 785467)% 200;

    return svg::Color(r,g,b);
}

inline void outputMesh(const GEO::Mesh& m,
                const std::vector<double> &points,
                const std::string& fname
) {
    using namespace svg;

    double size = 1024;
    Dimensions dim(size,size);
    Document doc(fname, Layout(dim, Layout::BottomLeft));

    std::vector<std::pair<int, int> > faces(m.facets.nb());
    for (int indFace = 0; indFace < m.facets.nb(); ++indFace) {
        faces[indFace].first = getCorrespondingPoint(m, indFace);
        faces[indFace].second = indFace;
    }
    std::sort(faces.begin(), faces.end());

    double centroid[2];
    centroid[0] = 0.;
    centroid[1] = 0.;
    double nbCentroids = 0;
    int predIndPoint = 0;
    for (int i = 0; i < faces.size(); ++i){
        int indFace = faces[i].second;
        int indPoint = faces[i].first;
        svg::Polygon shape = svg::Polygon(Fill(getColor(indPoint)), Stroke(1, getColor(indPoint)));

        for(int indVertex=0; indVertex < m.facets.nb_vertices(indFace); ++indVertex){
            int numVertex = m.facets.vertex(indFace, indVertex);
            double x = m.vertices.point(numVertex).x;
            double y = m.vertices.point(numVertex).y;
            shape << Point(size * x, size * y);
        }
        doc << shape;

        double tmpcentroid[2];

        getFacetCentroid(m, indFace, tmpcentroid);
        double area = getFacetArea(m, indFace);

        if(indPoint == predIndPoint){
            centroid[0] += area * tmpcentroid[0];
            centroid[1] += area * tmpcentroid[1];
            nbCentroids += area;
        } else {
            centroid[0] /= nbCentroids;
            centroid[1] /= nbCentroids;
            doc << Circle(Point(size * centroid[0], size * centroid[1]), 5, Fill(Color::Black));
            centroid[0] = area * tmpcentroid[0];
            centroid[1] = area * tmpcentroid[1];
            nbCentroids = area;
        }

        predIndPoint = indPoint;
    }
    centroid[0] /= nbCentroids;
    centroid[1] /= nbCentroids;
    doc << Circle(Point(size * centroid[0], size * centroid[1]), 5, Fill(Color::Black));

    for (int indPoint = 0; indPoint < points.size() / 2; indPoint += 1){
        doc << Circle(Point(size * points[2 * indPoint], size * points[2 * indPoint + 1]),
                      15,
                      Fill(getColor(indPoint)),
                      Stroke(1.5, Color::Black)
        );
    }

    doc.save();
}

inline void outputPoints(const std::vector<double> &points,
                  const std::string& fname){

    using namespace svg;

    double size = 1024;
    Dimensions dim(size,size);
    Document doc(fname, Layout(dim, Layout::BottomLeft));

    doc << (svg::Polygon(Fill(Color::White)) << Point(0,0)
                                        << Point(size,0)
                                        << Point(size,size)
                                        << Point(0,size) );

    for (int i = 0; i < points.size() / 2; ++i){
        doc << Circle(Point(size * points[2*i], size * points[2*i+1]), 5, Fill(Color::Black));
    }
    doc.save();
}

#endif //SEMIDISCRETE_IODIAGRAM_H
