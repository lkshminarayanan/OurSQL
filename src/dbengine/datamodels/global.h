#ifndef GLOBAL_H_

#define GLOBAL_H_

#include <iostream>
#include "../Select.h"
#include "../Where.h"
#include "../Modify.h"

#define flag int

#if false//set true for debug on. false for debug false
#define lg(x) cout << endl << x;cout.flush()
#else
#define lg(x) cout << ""
#endif

#if false//set true for debug Level 2(unimportant) on. false for debug false
#define lg2(x) cout << endl << x;cout.flush()
#else
#define lg2(x) cout << ""
#endif

#if false//set true for error on. false for error false
#define error(x) cout << endl << "ERROR : " << x;cout.flush()
#else
#define error(x) cout << ""
#endif

#endif
