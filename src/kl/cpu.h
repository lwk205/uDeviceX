#define KL_BEFORE(s, C) MSG("kl: %s", s); if (!kl::safe(ESC C)) continue;
#define KL_AFTER(s)
#define KL_CALL(F, C, A) F A

namespace kl { inline void msg(int, int, int,   int, int, int) { }; }
