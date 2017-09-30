// Prevent unknowning usage due to type promotion
template<typename T> __device__ class Automatic_Type_Promotion_Not_Allowed f2i( T const &t );
template<typename T> __device__ class Automatic_Type_Promotion_Not_Allowed f2u( T const &t );
template<typename T> __device__ class Automatic_Type_Promotion_Not_Allowed i2f( T const &t );
template<typename T> __device__ class Automatic_Type_Promotion_Not_Allowed u2f( T const &t );
template<typename T> __device__ class Automatic_Type_Promotion_Not_Allowed i2u( T const &t );
template<typename T> __device__ class Automatic_Type_Promotion_Not_Allowed u2i( T const &t );

__forceinline__ __device__ int f2i( float f ) {
  int i;
  asm( "mov.b32 %0, %1;" : "=r"( i ) : "f"( f ) );
  return i;
}

__forceinline__ __device__ uint f2u( float f ) {
  uint u;
  asm( "mov.b32 %0, %1;" : "=r"( u ) : "f"( f ) );
  return u;
}

__forceinline__ __device__ float i2f( int i ) {
  float f;
  asm( "mov.b32 %0, %1;" : "=f"( f ) : "r"( i ) );
  return f;
}

__forceinline__ __device__ float u2f( uint u ) {
  float f;
  asm( "mov.b32 %0, %1;" : "=f"( f ) : "r"( u ) );
  return f;
}

__forceinline__ __device__ uint i2u( int i ) {
  uint u;
  asm( "mov.b32 %0, %1;" : "=r"( u ) : "r"( i ) );
  return u;
}

__forceinline__ __device__ int u2i( uint u ) {
  int i;
  asm( "mov.b32 %0, %1;" : "=r"( i ) : "r"( u ) );
  return i;
}

// CUDA 5.5 does not define shuffle for uint
#if CUDART_VERSION <= 5500
__inline__ __device__ uint __shfl_up(uint var, unsigned int delta, int width=warpSize) {
  return i2u( __shfl_up( u2i(var), delta, width ) );
}
#endif

/******************************************************************************
  Arithmetic
 ******************************************************************************/

// Prevent unknowning usage due to type promotion
template<typename T, typename S, typename R> __device__ class Do_Not_Use_Tiny_Float_For_Normal_Operations xmad( T const &t, S const &s, R const &r );
template<typename T, typename S> __device__ class Do_Not_Use_Tiny_Float_For_Normal_Operations xadd( T const &t, S const &s );
template<typename T, typename S> __device__ class Do_Not_Use_Tiny_Float_For_Normal_Operations xsub( T const &t, S const &s );
template<typename T, typename S> __device__ class Do_Not_Use_Tiny_Float_For_Normal_Operations xscale( T const &t, S const &s );
template<typename T, typename S> __device__ class Do_Not_Use_Tiny_Float_For_Normal_Operations xmin( T const &t, S const &s );
template<typename T, typename S> __device__ class Do_Not_Use_Tiny_Float_For_Normal_Operations xmax( T const &t, S const &s );

// u * v + w
__forceinline__ __device__ uint xmad( uint u, float v, uint w ) {
  float a = u2f(u), c = u2f(w), d;
  asm( "fma.rm.f32 %0, %1, %2, %3;" : "=f"(d) : "f"(a), "f"(v), "f"(c) );
  return f2u(d);
}

__forceinline__ __device__ uint xmadrp( uint u, float v, uint w ) {
  float a = u2f(u), c = u2f(w), d;
  asm( "fma.rp.f32 %0, %1, %2, %3;" : "=f"(d) : "f"(a), "f"(v), "f"(c) );
  return f2u(d);
}

// u * v + w
__forceinline__ __device__ int xmad( int u, float v, int w ) {
  float a = i2f(u), c = i2f(w), d;
  asm( "fma.rm.f32 %0, %1, %2, %3;" : "=f"(d) : "f"(a), "f"(v), "f"(c) );
  return f2i(d);
}

// u * v - w
__forceinline__ __device__ uint xmsb( uint u, float v, uint w ) {
  float a = u2f(u), c = u2f(w), d;
  asm( "{.reg .f32 nc; neg.f32 nc, %3; fma.rm.f32 %0, %1, %2, nc;}" : "=f"(d) : "f"(a), "f"(v), "f"(c) );
  return f2u(d);
}

// u * v - w
__forceinline__ __device__ int xmsb( int u, float v, int w ) {
  float a = i2f(u), c = i2f(w), d;
  asm( "{.reg .f32 nc; neg.f32 nc, %3; fma.rm.f32 %0, %1, %2, nc;}" : "=f"(d) : "f"(a), "f"(v), "f"(c) );
  return f2i(d);
}


__forceinline__ __device__ uint xadd( uint u, uint v ) {
  float a = u2f(u), b = u2f(v), c;
  asm( "add.f32 %0, %1, %2;" : "=f"(c) : "f"(a), "f"(b) );
  return f2u(c);
}

__forceinline__ __device__ int xadd( int u, int v ) {
  float a = i2f(u), b = i2f(v), c;
  asm( "add.f32 %0, %1, %2;" : "=f"(c) : "f"(a), "f"(b) );
  return f2i(c);
}

__forceinline__ __device__ uint xadd( float u, float v ) {
  float c;
  asm( "add.f32 %0, %1, %2;" : "=f"(c) : "f"(u), "f"(v) );
  return f2i(c);
}

__forceinline__ __device__ uint xsub( uint u, uint v ) {
  float a = u2f(u), b = u2f(v), c;
  asm( "sub.f32 %0, %1, %2;" : "=f"(c) : "f"(a), "f"(b) );
  return f2u(c);
}

__forceinline__ __device__ int xsub( int u, int v ) {
  float a = i2f(u), b = i2f(v), c;
  asm( "sub.f32 %0, %1, %2;" : "=f"(c) : "f"(a), "f"(b) );
  return f2i(c);
}

__forceinline__ __device__ uint xscale( uint u, float s ) {
  float a = u2f(u), b;
  asm( "mul.f32 %0, %1, %2;" : "=f"(b) : "f"(a), "f"(s) );
  return f2u(b);
}

__forceinline__ __device__ int xscale( int u, float s ) {
  float a = i2f(u), b;
  asm( "mul.f32 %0, %1, %2;" : "=f"(b) : "f"(a), "f"(s) );
  return f2i(b);
}

// semantic: xdiv(x,0.25f) = x * 0.25f = x / 4
__forceinline__ __device__ uint xdiv( uint u, float s ) {
  float a = u2f(u), b;
  asm( "mul.f32.rm %0, %1, %2;" : "=f"(b) : "f"(a), "f"(s) );
  return f2u(b);
}

__forceinline__ __device__ int xdiv( int u, float s ) {
  float a = i2f(u), b;
  asm( "mul.f32.rm %0, %1, %2;" : "=f"(b) : "f"(a), "f"(s) );
  return f2i(b);
}

template<uint m> __forceinline__ __device__ uint xmod( uint u ) {
float a = u2f(u);
asm( "mul.f32.rm %0, %1, %2;" : "+f"(a) : "f"(a), "f"(1.f/m) );
return xsub( u, xscale( f2u(a), float(m) ) );
}

__forceinline__ __device__ int xmin( int u, int v ) {
  float a = i2f(u), b = i2f(v), c;
  asm( "min.f32 %0, %1, %2;" : "=f"(c) : "f"(a), "f"(b) );
  return f2i(c);
}

__forceinline__ __device__ uint xmin( uint u, uint v ) {
  float a = u2f(u), b = u2f(v), c;
  asm( "min.f32 %0, %1, %2;" : "=f"(c) : "f"(a), "f"(b) );
  return f2u(c);
}

__forceinline__ __device__ int xmax( int u, int v ) {
  float a = i2f(u), b = i2f(v), c;
  asm( "max.f32 %0, %1, %2;" : "=f"(c) : "f"(a), "f"(b) );
  return f2i(c);
}

__forceinline__ __device__ uint xmax( uint u, uint v ) {
  float a = u2f(u), b = u2f(v), c;
  asm( "max.f32 %0, %1, %2;" : "=f"(c) : "f"(a), "f"(b) );
  return f2u(c);
}
