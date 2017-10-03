inline  __device__ float3 operator+(float3 a, float3 b)
{
    return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline  __device__ void operator+=(float3 &a, float3 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
}

inline  __device__ float3 operator-(float3 a, float3 b)
{
    return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline  __device__ float dot(float3 a, float3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/** Copyright 1993-2013 NVIDIA Corporation.  All rights reserved. **/
