#include <iostream>
#include <vector>
#include <iomanip>
#include <random>
#include <algorithm>
#include <string>

#include <geogram/basic/common.h>
#include <geogram/basic/command_line.h>
#include <geogram/basic/command_line_args.h>
#include <geogram/mesh/mesh.h>
#include <geogram/mesh/mesh_io.h>
#include <geogram/numerics/predicates.h>
#include <exploragram/optimal_transport/optimal_transport_2d.h>

#include "simple_svg_1.0.0.hpp"
#include "iodiagram.h"

void init_zone_mesh(GEO::Mesh& m){
    /*
    GEO::vector<double> points(8);
    points[0]=0;
    points[1]=0.6;
    points[2]=0.5;
    points[3]=0.;
    points[4]=0.5;
    points[5]=0.4;
    points[6]=1.;
    points[7]=0.6;

    m.vertices.assign_points(points, 2, true);

    GEO::vector<GEO::index_t> facet1(3);
    facet1[0]=0;
    facet1[1]=1;
    facet1[2]=2;
    m.facets.create_polygon(facet1);

    GEO::vector<GEO::index_t> facet2(3);
    facet2[0]=2;
    facet2[1]=1;
    facet2[2]=3;
    m.facets.create_polygon(facet2);
*/
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


}

double getArea(const GEO::Mesh &m){
    double area = 0.;

    for (int f = 0; f < m.facets.nb(); ++f){
        area += std::abs(getFacetArea(m, f));
    }

    return area;

}

int main()
{

    std::cout << std::setprecision(9);
    GEO::initialize();
    GEO::CmdLine::import_arg_group("algo");
    GEO::CmdLine::set_arg("algo:predicates", "exact");

    //Init target area
    GEO::Mesh m;
    init_zone_mesh(m);
    m.vertices.set_dimension(3);


    std::cout << getArea(m) << std::endl;
    //exit(0);
    //Init points and their weights
    int nbPoints = 256;
    std::vector<double> points(2 * nbPoints);
    std::vector<double> target_weights(nbPoints, 1.);
    std::mt19937_64 gen(42);
    std::uniform_real_distribution<double> unif(0,1);
    for (double &v : points){
        v = unif(gen);
    }

    //outputMesh(m, points, "test.svg");
    //exit(0);

    double sum = 0.;
    for (double v : target_weights){
        sum += v;
    }
    sum /= getArea(m);
    for (double &v : target_weights){
        v /= sum;
    }
    //Init Power diagram class
    GEO::OptimalTransportMap2d OTM(&m);

    int nbIterLloyd = 0;
    while(nbIterLloyd < 100){
        //Set points in power diagram
        OTM.set_points(points.size()/2, points.data(), 2);

        //Init Power diagrams weights
        std::vector<double> pdWeights(nbPoints);
        std::vector<double> area(nbPoints);
        std::vector<double> garbage(nbPoints);
        for (double &v : pdWeights){
            v = 0.;
        }

        //Gradient Descent parameters
        double norm1 = 1.;
        double epsilon = std::pow(10, -3);
        int nbIter = 0;
        double learningRate = 1.;
        double rateStep = 0.995;
        while (norm1 > epsilon && nbIter < 1000){
            nbIter += 1;

            //Compute power diagram cells
            OTM.compute_P1_Laplacian(pdWeights.data(), nullptr, garbage.data());

            //Get power diagram
            GEO::Mesh pd;
            OTM.get_RVD(pd);

            //Compute each point corresponding cell area
            for (double& v : area){
                v = 0.;
            }
            for(int indFace = 0; indFace < pd.facets.nb(); ++indFace){
                area[getCorrespondingPoint(pd, indFace)] += getFacetArea(pd, indFace);
            }

            //Update Weights according to difference between cell size and point weight
            norm1 = 0.;
            for(int i = 0; i < target_weights.size(); ++i){
                double diff = (target_weights[i] - area[i]);
                pdWeights[i] += learningRate * diff;
                norm1 += std::abs(diff);
            };

            //std::cout << std::endl;
            learningRate *= rateStep;
        }
        GEO::Mesh pd;
        OTM.get_RVD(pd);
        //outputMesh(pd, points, "output" + std::to_string(nbIterLloyd) + ".svg");

        std::vector<double> centroids(nbPoints * 2);
        std::vector<double> weight(nbPoints);
        for(int indFace = 0; indFace < pd.facets.nb(); ++indFace){
            int indPoint = getCorrespondingPoint(pd, indFace);
            double w = getFacetArea(pd, indFace);
            weight[indPoint] += w;
            double c[2];
            getFacetCentroid(pd, indFace, c);
            centroids[2 * indPoint] += w * c[0];
            centroids[2 * indPoint + 1] += w * c[1];
        }
        for (int indPoint = 0; indPoint < nbPoints; ++indPoint){
            points[2 * indPoint] = centroids[2 * indPoint] / weight[indPoint];
            points[2 * indPoint + 1] = centroids[2 * indPoint + 1] / weight[indPoint];
        }

        std::cout << nbIterLloyd++ << " " << nbIter << std::endl;

    }

    GEO::Mesh pd;
    OTM.get_RVD(pd);

    outputMesh(pd, points, "output.svg");
    outputPoints(points, "poimts.svg");
}
