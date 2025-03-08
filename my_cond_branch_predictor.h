#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <stdlib.h>

/* Size of Predictor: 0 KB */
class AlwaysNotBranchPredictor
{
    public:
        bool predict() {
            return false;
        }
};
// =================
// Predictor End
// =================

#endif
static AlwaysNotBranchPredictor predictor;
