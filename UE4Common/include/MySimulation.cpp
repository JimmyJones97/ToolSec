#include <stdint.h>
#include <cmath>
#include "Math/IntRect.h"
#include "Camera/CameraTypes.h"
#include "MySimulation.h"

typedef int32_t int32;
typedef uint32_t uint32;

bool MySimulation::LocalPlayer__GetProjectionData(float viewport_sizeX, float viewport_sizeY, FMinimalViewInfo PlayerCameraManager__CameraCache_POV, FSceneViewProjectionData& ProjectionData){
    int32 X = 0;
    int32 Y = 0;
    uint32 SizeX = truncf(viewport_sizeX);
    uint32 SizeY = truncf(viewport_sizeY);

    FIntRect UnconstrainedRectangle = FIntRect(X, Y, X+SizeX, Y+SizeY);
    ProjectionData.SetViewRectangle(UnconstrainedRectangle);

    FMinimalViewInfo ViewInfo;
    ViewInfo = PlayerCameraManager__CameraCache_POV;
    return false;
}