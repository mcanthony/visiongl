
/*********************************************************************
***                                                                 ***
***  Source code generated by glsl2cpp.pl                           ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/
#include "vglImage.h"
/** Detects foreground pixels.

  */
void vglDetectFGSimpleBGModel(VglImage*  img_in, VglImage*  average, VglImage*  variance, VglImage*  foreground, float  std_thresh);

/** Updates average and variance of background model.

  */
void vglTrainSimpleBGModel(VglImage*  img_in, VglImage*  average, VglImage*  variance, float  weight);

/** Updates average and variance of background model only in pixels that are classified as background.

  */
void vglUpdatePartialSimpleBGModel(VglImage*  img_in, VglImage*  foregorundClose, VglImage*  average, VglImage*  variance, float  weight);

