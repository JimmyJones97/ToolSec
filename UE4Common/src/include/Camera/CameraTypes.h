#ifndef CAMERA_TYPES_H
#define CAMERA_TYPES_H
#include "../Math/Vector2D.h"
#include "../Math/Rotator.h"
#include "../Math/Matrix.h"
#include "../Math/UnrealMathUtility.h"
#include "../Math/PerspectiveMatrix.h"
#include "../SceneView.h"
#include "../EngineDefines.h"
typedef uint32_t uint32;

namespace ECameraProjectionMode
{
	enum Type
	{
		Perspective,
		Orthographic
	};
}


namespace ECameraAnimPlaySpace
{
	enum Type
	{
 		/** This anim is applied in camera space. */
 		CameraLocal,
 		/** This anim is applied in world space. */
 		World,
 		/** This anim is applied in a user-specified space (defined by UserPlaySpaceMatrix). */
 		UserDefined,
 	};
}
struct FMinimalViewInfo
{
	// 经过实际验证，这里结构与引擎中不同，插入了3个成员，调试看一只是0
	float unk00;
	float unk01;
	float unk02;

	/** Location */
	FVector Location;

	// 经过实际验证，这里结构与引擎中不同，插入了3个成员，调试看一只是0
	float unk10;
	float unk11;
	float unk12;

	/** Rotation */
	FRotator Rotation;


	/** The field of view (in degrees) in perspective mode (ignored in Orthographic mode) */
	
	float FOV;

	/** The desired width (in world units) of the orthographic view (ignored in Perspective mode) */
	
	float OrthoWidth;

	/** The near plane distance of the orthographic view (in world units) */
	
	float OrthoNearClipPlane;

	/** The far plane distance of the orthographic view (in world units) */
	
	float OrthoFarClipPlane;

	// Aspect Ratio (Width/Height); ignored unless bConstrainAspectRatio is true
	
	float AspectRatio;

	// If bConstrainAspectRatio is true, black bars will be added if the destination view has a different aspect ratio than this camera requested.
	
	uint32 bConstrainAspectRatio:1; 

	// If true, account for the field of view angle when computing which level of detail to use for meshes.
	
	uint32 bUseFieldOfViewForLOD:1;

	// The type of camera
	
	// TEnumAsByte<ECameraProjectionMode::Type> ProjectionMode;
    uint8_t ProjectionMode;

	/** Indicates if PostProcessSettings should be applied. */
	
	float PostProcessBlendWeight;

	// /** Post-process settings to use if PostProcessBlendWeight is non-zero. */
	
	// struct FPostProcessSettings PostProcessSettings;

	/** Off-axis / off-center projection offset as proportion of screen dimensions */
	
	FVector2D OffCenterProjectionOffset;

	inline FMinimalViewInfo()
		: Location(ForceInit)
		, Rotation(ForceInit)
		, FOV(90.0f)
		, OrthoWidth(512.0f)
		, OrthoNearClipPlane(0.0f)
		, OrthoFarClipPlane(WORLD_MAX)
		, AspectRatio(1.33333333f)
		, bConstrainAspectRatio(false)
		, bUseFieldOfViewForLOD(true)
		, ProjectionMode(ECameraProjectionMode::Perspective)
		, PostProcessBlendWeight(0.0f)
		, OffCenterProjectionOffset(ForceInitToZero)
	{
	}

	// Is this equivalent to the other one?
	bool Equals(const FMinimalViewInfo& OtherInfo) const;

	// Blends view information
	// Note: booleans are orred together, instead of blending
	void BlendViewInfo(FMinimalViewInfo& OtherInfo, float OtherWeight);

	/** Applies weighting to this view, in order to be blended with another one. Equals to this *= Weight. */
	void ApplyBlendWeight(const float& Weight);

	/** Combines this view with another one which will be weighted. Equals to this += OtherView * Weight. */
	void AddWeightedViewInfo(const FMinimalViewInfo& OtherView, const float& Weight);

	/** Calculates the projection matrix using this view info's aspect ratio (regardless of bConstrainAspectRatio) */
	FMatrix CalculateProjectionMatrix() const;

	// /** Calculates the projection matrix (and potentially a constrained view rectangle) given a FMinimalViewInfo and partially configured projection data (must have the view rect already set) */
	// static void CalculateProjectionMatrixGivenView(const FMinimalViewInfo& ViewInfo, TEnumAsByte<enum EAspectRatioAxisConstraint> AspectRatioAxisConstraint, class FViewport* Viewport, struct FSceneViewProjectionData& InOutProjectionData);
	static void CalculateProjectionMatrixGivenView(const FMinimalViewInfo& ViewInfo, struct FSceneViewProjectionData& InOutProjectionData);

};

#endif