#include <stdint.h>
#include <cmath>
#include "include/Math/IntRect.h"
#include "include/Math/InverseRotationMatrix.h"
#include "include/Camera/CameraTypes.h"
#include "include/MySimulation.h"
#include "logging.hpp"


typedef int32_t int32;
typedef uint32_t uint32;

bool MySimulation::LocalPlayer__GetProjectionData(float viewport_sizeX, float viewport_sizeY, FMinimalViewInfo PlayerCameraManager__CameraCache_POV, FSceneViewProjectionData& ProjectionData){
    DEBUG_PRINT("viewport sizeX:%f, sizeY:%f", viewport_sizeX, viewport_sizeY);

    int32 X = 0;
    int32 Y = 0;
    uint32 SizeX = truncf(viewport_sizeX);
    uint32 SizeY = truncf(viewport_sizeY);
    DEBUG_PRINT("SizeX:%d, SizeY:%d", SizeX, SizeY);

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
    
    DEBUG_PRINT("ProjectionData.ViewRect=((%d,%d),(%d,%d))",
        ProjectionData.GetViewRect().Min.X, ProjectionData.GetViewRect().Min.Y, ProjectionData.GetViewRect().Max.X, ProjectionData.GetViewRect().Max.Y);
	 

    DEBUG_PRINT("ProjectionData.ViewRotationMatrix:");
    for(int i=0; i<4; i++){
        DEBUG_PRINT("%f %f %f %f", 
          ProjectionData.ViewRotationMatrix.M[i][0],ProjectionData.ViewRotationMatrix.M[i][1],ProjectionData.ViewRotationMatrix.M[i][2],ProjectionData.ViewRotationMatrix.M[i][3]);
    }

    DEBUG_PRINT("ProjectionData.ProjectionMatrix:");
    for(int i=0; i<4; i++){
        DEBUG_PRINT("%f %f %f %f", 
          ProjectionData.ProjectionMatrix.M[i][0],ProjectionData.ProjectionMatrix.M[i][1],ProjectionData.ProjectionMatrix.M[i][2],ProjectionData.ProjectionMatrix.M[i][3]);
    }
    
    return false;
}