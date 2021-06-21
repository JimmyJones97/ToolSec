#include "SceneView.h"
#include "../logging.hpp"

bool FSceneView::ProjectWorldToScreen(const FVector& WorldPosition, const FIntRect& ViewRect, const FMatrix& ViewProjectionMatrix, FVector2D& out_ScreenPos)
{
	DEBUG_PRINT("WorldPosition=(%f,%f,%f), ViewRect=((%d,%d),(%d,%d))",
		WorldPosition.X, WorldPosition.Y, WorldPosition.Z,
		ViewRect.Min.X, ViewRect.Min.Y, ViewRect.Max.X, ViewRect.Max.Y);
	DEBUG_PRINT("ViewProjectionMatrix:");
    for(int i=0; i<4; i++){
        DEBUG_PRINT("%f %f %f %f", 
          ViewProjectionMatrix.M[i][0],ViewProjectionMatrix.M[i][1],ViewProjectionMatrix.M[i][2],ViewProjectionMatrix.M[i][3]);
    }
	FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
	if ( Result.W > 0.0f )
	{
		// the result of this will be x and y coords in -1..1 projection space
		const float RHW = 1.0f / Result.W;
		FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

		// Move from projection space to normalized 0..1 UI space
		const float NormalizedX = ( PosInScreenSpace.X / 2.f ) + 0.5f;
		const float NormalizedY = 1.f - ( PosInScreenSpace.Y / 2.f ) - 0.5f;

		FVector2D RayStartViewRectSpace(
			( NormalizedX * (float)ViewRect.Width() ),
			( NormalizedY * (float)ViewRect.Height() )
			);

		out_ScreenPos = RayStartViewRectSpace + FVector2D(static_cast<float>(ViewRect.Min.X), static_cast<float>(ViewRect.Min.Y));

		return true;
	}
	
	return false;
}