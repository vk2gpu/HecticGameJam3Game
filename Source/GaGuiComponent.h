/**************************************************************************
*
* File:		GaGuiComponent.h
* Author:	Daniel de Zwart
* Ver/Date:		
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __GaGuiComponent_H__
#define __GaGuiComponent_H__

#include "Psybrus.h"
#include "System/Scene/Rendering/ScnMaterial.h"
#include "System/Scene/Rendering/ScnSpriteComponent.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef< class GaGuiComponent > GaGuiComponentRef;

//////////////////////////////////////////////////////////////////////////
// ScnShaderAlphaTestUniformBlockData
struct GaGuiShaderUniformBlockData
{
	REFLECTION_DECLARE_BASIC( GaGuiShaderUniformBlockData );
	GaGuiShaderUniformBlockData(){};

	MaVec4d VariableNameHere_;
};
//////////////////////////////////////////////////////////////////////////
// GaGuiComponent
class GaGuiComponent:
	public ScnComponent
{
public:
	DECLARE_RESOURCE( GaGuiComponent, ScnComponent );

	void initialise();
	void initialise( const Json::Value& Object );

	virtual void preUpdate( BcF32 Tick );
	virtual void update( BcF32 Tick );
	virtual void postUpdate( BcF32 Tick );
	
	virtual void onAttach( ScnEntityWeakRef Parent );
	virtual void onDetach( ScnEntityWeakRef Parent );

	

private:
	ReObjectRef< class ScnCanvasComponent > Canvas_;
	ReObjectRef< class GaPlayerComponent > Player_;
	ReObjectRef< class GaSwarmElementComponent > Swarm_;
	ScnSpriteComponentRef HealthSprite_;
	RsBuffer* UniformBuffer_;
};

#endif
