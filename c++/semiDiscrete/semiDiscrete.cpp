#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include <geogram/basic/common.h>
#include <geogram/basic/command_line.h>
#include <geogram/basic/command_line_args.h>
#include <geogram/mesh/mesh.h>
#include <geogram/numerics/predicates.h>
#include <exploragram/optimal_transport/optimal_transport_2d.h>

#include "iodiagram.h"

void init_zone_mesh(GEO::Mesh& m){
#ifndef PRETTY_DOMAIN
    GEO::vector<double> points(8);
    points[0]=0;
    points[1]=0;
    points[2]=1.;
    points[3]=0.;
    points[4]=1.;
    points[5]=1.;
    points[6]=0.;
    points[7]=1.;

    m.vertices.assign_points(points, 2, true);

    GEO::vector<GEO::index_t> facet1(4);
    facet1[0]=0;
    facet1[1]=1;
    facet1[2]=2;
    facet1[3]=3;
    m.facets.create_polygon(facet1);
#else
    int nbpts = 100;
    GEO::vector<double> points(2 * (2 * nbpts));
    for (int i = 0; i < nbpts; ++i){
        double angle = M_PI_2 + (5. / 180. + 2 * 350./360. * double(i) / (nbpts-1) )  * M_PI;
        double x = std::cos(angle) * 0.5 + 0.5;
        double y = std::sin(angle) * 0.5 + 0.5;

        points[2 * i] = x;
        points[2 * i + 1] = y;
    }
    for (int i = 0; i < nbpts; ++i){
        double angle = M_PI_2 + (5. / 180. + 2 * 350./360. * double(i) / (nbpts-1) )  * M_PI;
        double x = std::cos(angle) * 0.3 + 0.5;
        double y = std::sin(angle) * 0.3 + 0.7;

        points[2 * nbpts + 2 * i] = x;
        points[2 * nbpts + 2 * i + 1] = y;
    }

    m.vertices.assign_points(points, 2, true);


    for (int i = 0; i < nbpts-1; ++i){
        GEO::vector<GEO::index_t> facet(4);
        facet[0] = i;
        facet[1] = i+1;
        facet[2] = nbpts+i+1;
        facet[3] = nbpts+i;
        m.facets.create_polygon(facet);
    }
#endif

}

int main()
{

    //Init Geogram
    GEO::initialize();
    GEO::CmdLine::import_arg_group("algo");
    GEO::CmdLine::set_arg("algo:predicates", "exact");

    //Init target area
    GEO::Mesh m;
    init_zone_mesh(m);
    m.vertices.set_dimension(3);

    //Init points
    int nbPoints = 256;
    std::vector<double> points(2 * nbPoints);
    std::mt19937_64 gen(133742);
    std::uniform_real_distribution<double> unif(0,1);
    for (double &v : points){
        v = unif(gen);
    }

    //Init Power diagram class
    GEO::OptimalTransportMap2d OTM(&m);

    //Set points in power diagram
    OTM.set_points(points.size()/2, points.data(), 2);

    //Init Power diagrams weights
    std::vector<double> pdWeights(nbPoints, 0.);
    std::vector<double> garbage(nbPoints);

    //Compute power diagram cells
    OTM.compute_P1_Laplacian(pdWeights.data(), nullptr, garbage.data());

    //Get mesh describing power diagram
    GEO::Mesh pd;
    OTM.get_RVD(pd);

    //Plot mesh to svg
    outputMesh(pd, points, "output.svg");
    outputPoints(points, "poimts.svg");
}
