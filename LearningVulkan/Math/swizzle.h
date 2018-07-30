#pragma once

#ifndef _MM_SHUFFLE
#define _MM_SHUFFLE(fp3,fp2,fp1,fp0) (((fp3) << 6) | ((fp2) << 4) | ((fp1) << 2) | ((fp0)))
#endif

#define XYZW(X, Y, Z, W) \
X ## Y ## Z ## W = _MM_SHUFFLE(_ ## W, _ ## Z, _ ## Y, _ ## X),

#define level3(_1, _2, _3)\
    XYZW(_1, _2, _3, X)\
    XYZW(_1, _2, _3, Y)\
    XYZW(_1, _2, _3, Z)\
    XYZW(_1, _2, _3, W)

#define level2(_1, _2)\
    level3(_1, _2, X)\
    level3(_1, _2, Y)\
    level3(_1, _2, Z)\
    level3(_1, _2, W)

#define level1(_)\
    level2(_, X)\
    level2(_, Y)\
    level2(_, Z)\
    level2(_, W)

#define _X 0
#define _Y 1
#define _Z 2
#define _W 3

enum swiz {
	level1(X)
	level1(Y)
	level1(Z)
	level1(W)
};

#undef XYZW
#undef level1
#undef level2
#undef level3
#undef _X
#undef _Y
#undef _Z
#undef _W