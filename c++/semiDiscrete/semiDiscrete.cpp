#include <iostream>
#include <vector>
#include <iomanip>

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
    m.edges.create_edge(2, 3);
    m.edges.create_edge(3, 0);

    GEO::vector<GEO::index_t> facets(4);
    facets[0]=0;
    facets[1]=1;
    facets[2]=2;
    facets[3]=3;
    m.facets.create_polygon(facets);

}

int main()
{

    std::cout << std::setprecision(9);
    GEO::initialize();
    GEO::CmdLine::import_arg_group("algo");
    GEO::CmdLine::set_arg("algo:predicates", "exact");

    //Init points and their weights
    std::vector<double> points(8);
    std::vector<double> target_weights(4);
    points[0] = 0.25;
    points[1] = 0.25;
    points[2] = 0.75;
    points[3] = 0.75;
    points[4] = 0.25;
    points[5] = 0.75;
    points[6] = 0.75;
    points[7] = 0.25;

    target_weights[0] = 1.;
    target_weights[1] = 1.;
    target_weights[2] = 1.;
    target_weights[3] = 2.;
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
    std::vector<double> pdWeights(4);
    std::vector<double> area(4);
    for (double &v : pdWeights){
        v = 0.;
    }

    //Init Power diagram class
    GEO::OptimalTransportMap2d OTM(&m);
    OTM.set_points(points.size()/2, points.data(), 2);

    double norm1 = 1.;
    double epsilon = std::pow(10, -9);
    int nbIter = 0;

    while (norm1 > epsilon){
        nbIter += 1;

        //Compute power diagram cells area
        OTM.compute_P1_Laplacian(pdWeights.data(), nullptr, area.data());

        norm1 = 0.;
        for(int i = 0; i < target_weights.size(); ++i){
            double diff = target_weights[i] - area[i];
            pdWeights[i] += diff;
            norm1 += std::abs(diff * diff);
            std::cout << area[i] << " " << diff << std::endl;
        }
        norm1 = std::sqrt(norm1);

        std::cout << std::endl;

    }
    std::cout << nbIter << std::endl;


}
