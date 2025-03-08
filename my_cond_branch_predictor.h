#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <stdlib.h>

/* Size of Predictor: 0 KB */
class AlwaysBranchPredictor
{
    public:
        bool predict() {
            return true;
        }
};
// =================
// Predictor End
// =================

#endif
static AlwaysBranchPredictor predictor;
