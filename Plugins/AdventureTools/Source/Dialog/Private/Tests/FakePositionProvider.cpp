// (c) 2026 Storybridge Games


#include "FakePositionProvider.h"

#include "Components/SphereComponent.h"

UFakePositionProvider::UFakePositionProvider()
{
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
}

USphereComponent* UFakePositionProvider::GetPosition() const
{
	return Sphere;
}