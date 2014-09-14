/**************************************************************************
*
* File:		GaSwarmElementComponent.cpp
* Author:	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "GaSwarmElementComponent.h"

#include "System/Scene/Rendering/ScnShaderFileData.h"
#include "System/Scene/Rendering/ScnViewComponent.h"

#include "System/Content/CsPackage.h"
#include "System/Content/CsCore.h"

#include "Base/BcProfiler.h"
#include "Base/BcRandom.h"
#include "System/Debug/DsCore.h"
#include "GaSwarmManagerComponent.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
DEFINE_RESOURCE( GaSwarmElementComponent );

void GaSwarmElementComponent::StaticRegisterClass()
{
	ReField* Fields[] =
	{
		new ReField( "Manager_", &GaSwarmElementComponent::Manager_, bcRFF_TRANSIENT ),
		new ReField( "UnitMask_", &GaSwarmElementComponent::UnitMask_ ),
		new ReField( "Acceleration_", &GaSwarmElementComponent::Acceleration_ ),
		new ReField( "StagedAcceleration_", &GaSwarmElementComponent::StagedAcceleration_ ),
		new ReField( "Velocity_", &GaSwarmElementComponent::Velocity_ , DsCore::DsCoreSerialised),
		new ReField( "StagedVelocity_", &GaSwarmElementComponent::StagedVelocity_ ),
		new ReField( "VelocityDriven_", &GaSwarmElementComponent::VelocityDriven_ ),
		new ReField( "MaxSpeed_", &GaSwarmElementComponent::MaxSpeed_ ),
		new ReField( "AttackTarget_", &GaSwarmElementComponent::AttackTarget_ ),
		new ReField( "Health_", &GaSwarmElementComponent::Health_, DsCore::DsCoreSerialised ),
		new ReField( "MaxHealth_", &GaSwarmElementComponent::MaxHealth_, DsCore::DsCoreSerialised ),
		new ReField( "AttackSpeed_", &GaSwarmElementComponent::AttackSpeed_, DsCore::DsCoreSerialised ),
		new ReField( "AttackTimer_", &GaSwarmElementComponent::AttackTimer_, DsCore::DsCoreSerialised ),
		new ReField( "AttackTarget_", &GaSwarmElementComponent::AttackTarget_, DsCore::DsCoreSerialised ),
	};
		
	ReRegisterClass< GaSwarmElementComponent, Super >( Fields )
		.addAttribute( new ScnComponentAttribute( 1 ) );
}

//////////////////////////////////////////////////////////////////////////
// initialise
void GaSwarmElementComponent::initialise( const Json::Value& Object )
{
	initialise( );
	if (Object["unitmask"] != Json::ValueType::nullValue)
	{
		UnitMask_ = (BcU8)Object["unitmask"].asUInt();
	}
	if (Object["velocitydriven"] != Json::ValueType::nullValue)
	{
		VelocityDriven_ = Object["velocitydriven"].asBool();
	}
	if (Object["randomvelocity"] != Json::ValueType::nullValue)
	{
		Velocity_ = 
			MaVec2d( BcRandom::Global.randReal() - 0.5f, BcRandom::Global.randReal() - 0.5f ).normal() *
			BcF32( Object["randomvelocity"].asDouble() );
	}
	if( Object[ "maxspeed" ] != Json::ValueType::nullValue )
	{
		MaxSpeed_ = BcF32( Object[ "maxspeed" ].asDouble() );
	}

	if( Object[ "maxhealth" ] != Json::ValueType::nullValue )
	{
		MaxHealth_ = Object[ "maxhealth" ].asUInt();
		Health_ = MaxHealth_;
	}

	if( Object[ "attackspeed" ] != Json::ValueType::nullValue )
	{
		AttackSpeed_ = BcF32( Object[ "attackspeed" ].asDouble() );
		AttackTimer_ = 1.0f;
	}

	if( Object[ "attackdistance" ] != Json::ValueType::nullValue )
	{
		AttackDistance_ = BcF32( Object[ "attackdistance" ].asDouble() );
	}

}

//////////////////////////////////////////////////////////////////////////
// initialise
void GaSwarmElementComponent::initialise( )
{
	UnitMask_ = 0;
	Acceleration_ = MaVec2d( 0, 0 );
	Velocity_ = MaVec2d( 0, 0 );
	StagedAcceleration_ = MaVec2d( 0, 0 );
	StagedVelocity_ = MaVec2d( 0, 0 );
	VelocityDriven_ = false;
	MaxSpeed_ = 1.0f;
	AttackTarget_ = nullptr;
	MaxHealth_ = 5;
	Health_ = MaxHealth_;
	AttackSpeed_ = 1.0f;
	AttackTimer_ = 1.0f;
	AttackDistance_ = 80.0f;
}

//////////////////////////////////////////////////////////////////////////
// update
//virtual
void GaSwarmElementComponent::update( BcF32 Tick )
{
	Super::update( Tick );

	if ( VelocityDriven_ )
	{
		//Velocity_ += Acceleration_ * Tick * 20.0f;
		MaVec2d position = getPosition();
		position = position + Velocity_ * Tick * 20.0f;
	
		MaVec3d realPos(position, 0.0f);
		getParentEntity()->setWorldPosition(realPos);
	}

	// Check target for attack.
	if( getAttackTarget() != nullptr )
	{
		// Distance.
		if( AttackTimer_ < 0.0f )
		{
			auto Distance = ( getAttackTarget()->getPosition() - getPosition() ).magnitude();
			if( Distance < AttackDistance_ )
			{
				if( getAttackTarget()->Health_ > 0 ) 
				{
					// TODO: Animation thing.
					if( --getAttackTarget()->Health_ == 0 )
					{
						// Kill it.
						ScnCore::pImpl()->removeEntity( getAttackTarget()->getParentEntity() );

						// No more targetting.
						setAttackTarget( nullptr );
					}
				}
				AttackTimer_ = 1.0f;
			}
		}
	}


	// Cool down for attack.
	if( AttackTimer_ >= 0.0f )
	{
		AttackTimer_ -= Tick * AttackSpeed_;
	}
}

//////////////////////////////////////////////////////////////////////////
// onAttach
//virtual
void GaSwarmElementComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );

	Manager_ = Parent->getParentEntity()->getComponentByType< GaSwarmManagerComponent >();

	// TODO: Register with manager or something.
	//BcAssert( Manager_ != nullptr );
	// The player won't have a manager to begin with. Derpity derp that later
	if (Manager_.isValid())
		Manager_->registerElement(this);
}

//////////////////////////////////////////////////////////////////////////
// onDetach
//virtual
void GaSwarmElementComponent::onDetach(ScnEntityWeakRef Parent)
{
	Super::onDetach(Parent);

	// TODO: Unregister with manager or something.
	if (Manager_.isValid())
		Manager_->deregisterElement(this);
}


//////////////////////////////////////////////////////////////////////////
// getUnitMask
//virtual
BcU8 GaSwarmElementComponent::getUnitMask()
{
	return UnitMask_;
}

MaVec2d	GaSwarmElementComponent::getAcceleration()
{
	return Acceleration_;
}
void GaSwarmElementComponent::stageAcceleration( MaVec2d acceleration )
{
	StagedAcceleration_ = acceleration;
}
MaVec2d GaSwarmElementComponent::getVelocity()
{
	return Velocity_;
}
void GaSwarmElementComponent::stageVelocity( MaVec2d acceleration )
{
	auto mag = acceleration.magnitude();
	if( mag > MaxSpeed_ )
	{
		acceleration /= mag;
		acceleration *= MaxSpeed_;
	}
	else if( mag > ( MaxSpeed_ * 0.5f ) )
	{
		mag *= 0.9f;
	}

	StagedVelocity_ = acceleration;
}
void GaSwarmElementComponent::commitChanges()
{
	Acceleration_ = StagedAcceleration_;
	Velocity_ = StagedVelocity_;
}

MaVec2d GaSwarmElementComponent::getPosition()
{
	MaVec3d pos = getParentEntity()->getWorldPosition();
	return MaVec2d(pos.x(), pos.y());
}


void GaSwarmElementComponent::setAttackTarget( GaSwarmElementComponent* AttackTarget )
{
	AttackTarget_ = AttackTarget;
}

GaSwarmElementComponent* GaSwarmElementComponent::getAttackTarget() const
{
	return AttackTarget_;
}

//////////////////////////////////////////////////////////////////////////
// getFishSize
BcU32 GaSwarmElementComponent::getMaxHealth() const
{
	return MaxHealth_;
}

//////////////////////////////////////////////////////////////////////////
// getFishSize
BcU32 GaSwarmElementComponent::getHealth() const
{
	return Health_;
}