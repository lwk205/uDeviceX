#include "common.h"

#include "mesh.h"

namespace mesh
{
    enum {X, Y, Z};
    enum {XX, XY, XZ, YY, YZ, ZZ};
    
#define Ax A[X]
#define Ay A[Y]
#define Az A[Z]

#define Bx B[X]
#define By B[Y]
#define Bz B[Z]

#define Cx C[X]
#define Cy C[Y]
#define Cz C[Z]

    /* see http://chenlab.ece.cornell.edu/Publication/Cha/icip01_Cha.pdf */
    
    static void M_0(const float *A, const float *B, const float *C, /**/ float *res)
    {
        *res =  (+Ax * (By * Cz -Bz * Cy)
                 -Ay * (Bx * Cz -Bz * Cx)
                 +Az * (Bx * Cy -By * Cx)) / 6.f;
    }

    static void M_1(const float *A, const float *B, const float *C, /**/ float *res)
    {
        const float common = (+Ax * (By * Cz -Bz * Cy)
                              -Ay * (Bx * Cz -Bz * Cx)
                              +Az * (Bx * Cy -By * Cx)) / 24.f;

        res[0] = common * (Ax + Bx + Cx);
        res[1] = common * (Ay + By + Cy);
        res[2] = common * (Az + Bz + Cz);
    }

    static void M_2(const float *A, const float *B, const float *C, /**/ float *res)
    {
        /* see /poc/mesh/moments.mac */
        const float D1 = Ax*(By*Cz-Bz*Cy)-Ay*(Bx*Cz-Bz*Cx)+Az*(Bx*Cy-By*Cx) / 60.f;
        const float D2 = By+Ay;
        const float D3 = Bz+Az;
        const float D4 = 2*Bx+Ax;
        const float D5 = 2*Cx+Bx+Ax;
        const float D6 = 0.5f * D1;
        
        res[XX] = (Ax*Ax + Ax*Bx + Bx*Bx + (Bx+Ax)*Cx + Cx*Cx) * D1;
        res[YY] = (Ay*Ay + Ay*By + By*By +    D2 * Cy + Cy*Cy) * D1;
        res[ZZ] = (Az*Az + Az*Bz + Bz*Bz +    D3 * Cz + Cz*Cz) * D1;
 
        res[XY] = (2 * Ax*Ay + Ay*Bx +        D4*By + D2*Cx +           D5*Cy) * D6;
        res[XZ] = (2 * Ax*Az + Az*Bx +        D4*Bz + D3*Cx +           D5*Cz) * D6;
        res[YZ] = (2 * Ay*Az + Az*By + (2*By+Ay)*Bz + D3*Cy + (2*Cy+By+Ay)*Cz) * D6;
    }

#undef Ax
#undef Ay
#undef Az

#undef Bx
#undef By
#undef Bz

#undef Cx
#undef Cy
#undef Cz

#define load_t(m, tid) {m.vv[3*tid + 0], m.vv[3*tid + 1], m.vv[3*tid + 2]}

    void center_of_mass(const Mesh mesh, /**/ float *com)
    {
        float Vtot, M1tot[3] = {0};
        float V, M1[3];
        
        for (int it = 0; it < mesh.nt; ++it)
        {
            const int t1 = mesh.tt[3*it + 0];
            const int t2 = mesh.tt[3*it + 1];
            const int t3 = mesh.tt[3*it + 2];

            const float A[3] = load_t(mesh, t1);
            const float B[3] = load_t(mesh, t2);
            const float C[3] = load_t(mesh, t3);
            
            M_0(A, B, C, /**/ V);
            M_1(A, B, C, /**/ M1);

            Vtot += V;
            M1tot[X] += M1[X];
            M1tot[Y] += M1[Y];
            M1tot[Z] += M1[Z];
        }

        com[X] = m1tot[X] / V;
        com[Y] = m1tot[Y] / V;
        com[Z] = m1tot[Z] / V;
    }

#define shift(a, s) do {A[X] -= s[X]; A[Y] -= s[Y]; A[Z] -= s[Z];} while(0)
    
    void inertia_tensor(const Mesh mesh, const float *com, /**/ float *I)
    {
        memset(I, 0, 6 * sizeof(float));

        float M2[6];
        
        for (int it = 0; it < mesh.nt; ++it)
        {
            const int t1 = mesh.tt[3*it + 0];
            const int t2 = mesh.tt[3*it + 1];
            const int t3 = mesh.tt[3*it + 2];

            float A[3] = load_t(mesh, t1);
            float B[3] = load_t(mesh, t2);
            float C[3] = load_t(mesh, t3);

            shift(A, com);
            shift(B, com);
            shift(C, com);
            
            M_2(A, B, C, /**/ M2);

            I[XX] += M_2[YY] + M_2[ZZ];
            I[YY] += M_2[XX] + M_2[ZZ];
            I[ZZ] += M_2[XX] + M_2[YY];

            I[XY] -= M_2[XY];
            I[XZ] -= M_2[XZ];
            I[YZ] -= M_2[YZ];
        }
    }

#undef shift
#undef load_t
}
