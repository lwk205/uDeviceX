/*
 *  main.cpp
 *  Part of CTC/cell-placement/
 *
 *  Created and authored by Diego Rossinelli on 2014-12-18.
 *  Further edited by Dmitry Alexeev on 2014-03-25.
 *  Copyright 2015. All rights reserved.
 *
 *  Users are NOT authorized
 *  to employ the present software for their own publications
 *  before getting a written permission from the author of this file.
 */

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "checker.h"

using namespace std;

int main(int argc, const char ** argv)
{
    if (argc != 4)
    {
        printf("usage: ./cell-placement <xdomain-extent> <ydomain-extent> <zdomain-extent>\n");
        exit(-1);
    }

    int domainextent[3];
    for(int i = 0; i < 3; ++i)
        domainextent[i] = atoi(argv[1 + i]);

    printf("domain extent: %d %d %d\n",
            domainextent[0], domainextent[1], domainextent[2]);

    Extent extents[2] = {
            compute_extent("../cuda-rbc/rbc2.atom_parsed"),
            compute_extent("../cuda-ctc/sphere.dat")
    };

    bool failed = false;

    vector<TransformedExtent> results[2];

    const float tol = 0.1;

    Checker checker(8 + tol, domainextent, tol);
    int tot = 0;

    while(!failed)
    {
        const int maxattempts = 10000;

        int attempt = 0;
        for(; attempt < maxattempts; ++attempt)
        {
            const int type = 0;//(int)(drand48() >= 0.25);

            TransformedExtent t(extents[type], domainextent);

            bool noncolliding = true;

#if 0
            //original code
            for(int i = 0; i < 2; ++i)
                for(int j = 0; j < results[i].size() && noncolliding; ++j)
                    noncolliding &= !t.collides(results[i][j], tol);
#else
            noncolliding = checker.check(t);
#endif

            if (noncolliding)
            {
                checker.add(t);
                results[type].push_back(t);
                ++tot;
                break;
            }
        }

        if (tot % 1000 == 0)
            printf("Done with %d cells...\n", tot);

        failed |= attempt == maxattempts;
    }

    string output_names[2] = { "rbcs-ic.txt", "ctcs-ic.txt" };

    for(int idtype = 0; idtype < 2; ++idtype)
    {
        FILE * f = fopen(output_names[idtype].c_str(), "w");

        for(vector<TransformedExtent>::iterator it = results[idtype].begin(); it != results[idtype].end(); ++it)
        {
            for(int c = 0; c < 3; ++c)
                fprintf(f, "%f ", 0.5 * (it->xmin[c] + it->xmax[c]));

            for(int i = 0; i < 4; ++i)
                for(int j = 0; j < 4; ++j)
                    fprintf(f, "%f ", it->transform[i][j]);

            fprintf(f, "\n");
        }

        fclose(f);
    }

    printf("Generated %d RBCs, %d CTCs\n", (int)results[0].size(), (int)results[1].size());

    return 0;
}
