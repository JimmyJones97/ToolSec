#include <stdint.h>
#include <cmath>
#include "Math/IntRect.h"
#include "Math/InverseRotationMatrix.h"
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

	// // Create the view matrix
	// ProjectionData.ViewOrigin = StereoViewLocation;
	// ProjectionData.ViewRotationMatrix = FInverseRotationMatrix(ViewInfo.Rotation) * FMatrix(
	// 	FPlane(0,	0,	1,	0),
	// 	FPlane(1,	0,	0,	0),
	// 	FPlane(0,	1,	0,	0),
	// 	FPlane(0,	0,	0,	1));
    ProjectionData.ViewOrigin = PlayerCameraManager__CameraCache_POV.Location;
    ProjectionData.ViewRotationMatrix = FInverseRotationMatrix(ViewInfo.Rotation) * FMatrix(
		FPlane(0,	0,	1,	0),
		FPlane(1,	0,	0,	0),
		FPlane(0,	1,	0,	0),
		FPlane(0,	0,	0,	1));

    FMinimalViewInfo::CalculateProjectionMatrixGivenView(ViewInfo, /*inout*/ ProjectionData);
    
    return false;
}