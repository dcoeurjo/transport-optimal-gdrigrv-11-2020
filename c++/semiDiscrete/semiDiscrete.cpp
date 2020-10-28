#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <random>

#include <geogram/voronoi/RVD.h>
#include <geogram/delaunay/delaunay_2d.h>
#include <geogram/basic/common.h>
#include <geogram/basic/command_line.h>
#include <geogram/basic/command_line_args.h>
#include <geogram/mesh/mesh.h>
#include <geogram/mesh/mesh_io.h>
#include <geogram/numerics/predicates.h>
#include <exploragram/optimal_transport/optimal_transport_2d.h>

#include "simple_svg_1.0.0.hpp"

void init_zone_mesh(GEO::Mesh& m){
    GEO::vector<double> points(8);
    points[0]=0;
    points[1]=0;
    points[2]=0;
    points[3]=1;
    points[4]=1;
    points[5]=1;
    points[6]=1;
    points[7]=0;

    m.vertices.assign_points(points, 2, true);
    m.edges.create_edge(0, 1);
    m.edges.create_edge(1, 2);
    m.edges.create_edge(2, 0);
    m.edges.create_edge(3, 0);

    GEO::vector<GEO::index_t> facets(4);
    facets[0]=0;
    facets[1]=1;
    facets[2]=2;
    facets[3]=3;
    m.facets.create_polygon(facets);

}

svg::Color getColor(int i){
    int r = 56 + (1354941 * i + 865641) % 200;
    int g = 56 + (3728175 * i + 542681) % 200;
    int b = 56 + (4529786 * i + 785467)% 200;

    return svg::Color(r,g,b);
}

void outputMesh(const GEO::Mesh& m,
                const std::vector<double> &points,
                const std::string& fname
                ) {
    using namespace svg;

    double size = 1024;
    Dimensions dim(size,size);
    Document doc(fname, Layout(dim, Layout::BottomLeft));

    int* id = (int*)m.facets.attributes().find_attribute_store("chart")->data();

    for (int indFace = 0; indFace < m.facets.nb(); ++indFace){
        Polygon shape = svg::Polygon(Fill(getColor(id[indFace])), Stroke(2, Color::Black));

        for(int indVertex=0; indVertex < m.facets.nb_vertices(indFace); ++indVertex){
            int numVertex = m.facets.vertex(indFace, indVertex);
            double x = m.vertices.point(numVertex).x;
            double y = m.vertices.point(numVertex).y;
            shape << Point(size * x, size * y);
        }

        doc << shape;
    }

    for (int indPoint = 0; indPoint < points.size() / 2; indPoint += 1){
        doc << Circle(Point(size * points[2 * indPoint], size * points[2 * indPoint + 1]),
                      15,
                      Fill(getColor(indPoint)),
                      Stroke(1.5, Color::Black)
                      );
    }

    doc.save();
}

int main()
{

    std::cout << std::setprecision(9);
    GEO::initialize();
    GEO::CmdLine::import_arg_group("algo");
    GEO::CmdLine::set_arg("algo:predicates", "exact");

    //Init points and their weights
    int nbPoints = 64;
    std::vector<double> points(2 * nbPoints);
    std::vector<double> target_weights(nbPoints, 1.);
    std::mt19937_64 gen(42);
    std::uniform_real_distribution<double> unif(0,1);
    for (double &v : points){
        v = unif(gen);
    }

    double sum = 0.;
    for (double v : target_weights){
        sum += v;
    }
    for (double &v : target_weights){
        v /= sum;
    }

    //Init target area
    GEO::Mesh m;
    init_zone_mesh(m);
    m.vertices.set_dimension(3);

    //Init Power diagrams weights
    std::vector<double> pdWeights(nbPoints);
    std::vector<double> area(nbPoints);
    for (double &v : pdWeights){
        v = 0.;
    }

    //Init Power diagram class
    GEO::OptimalTransportMap2d OTM(&m);
    OTM.set_points(points.size()/2, points.data(), 2);

    double norm1 = 1.;
    double epsilon = std::pow(10, -3);
    int nbIter = 0;
    double rate = 1.;
    double rateStep = 0.99;
    while (norm1 > epsilon && nbIter < 1000){
        std::cout << nbIter << " " << norm1 << std::endl;
        nbIter += 1;

        //Compute power diagram cells area
        OTM.compute_P1_Laplacian(pdWeights.data(), nullptr, area.data());

        //Update Weights according to difference between cell size and point weight
        norm1 = 0.;
        for(int i = 0; i < target_weights.size(); ++i){
            double diff = (target_weights[i] - area[i]);
            pdWeights[i] += rate * diff;
            norm1 += std::abs(diff);
            //std::cout << area[i] << " " << diff << std::endl;
        };

        //std::cout << std::endl;
        rate *= rateStep;
    }
    std::cout << nbIter << std::endl;

    GEO::Mesh pd;
    OTM.get_RVD(pd);

    outputMesh(pd, points, "output.svg");

}
