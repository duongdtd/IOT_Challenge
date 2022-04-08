#include "test_variable.h"
#include "math.h"
///////////////////////////// QUATERNION ///////////////////////////////////////////////////////
void
setQuaternion_Init (struct Quaternion_Base *q)
{
  q->w = 1.0f;
  q->x = 0.0f;
  q->y = 0.0f;
  q->z = 0.0f;

}

/////////////////////////////// VECTORINT16 ////////////////////////////
void
setVectorInt16_Init (struct VectorInt16_Base *q)
{
  q->x = 0;
  q->y = 0;
  q->z = 0;

}

////////////////////////////// VectorFloat //////////////////////////////
void
setVectorFloat_Init (struct VectorFloat_Base *q)
{
  q->x = 0;
  q->y = 0;
  q->z = 0;

}

