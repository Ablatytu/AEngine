#pragma once

#include "AComponent.h"
#include "Vector3.h"

class Transform final : public AComponent
{
public:
	Transform() = default;

	Vector3 GetPosition() const;
	Vector3 GetRotation() const;
	Vector3 GetScale() const;

	void SetPosition(const Vector3& position) const;
	void SetRotation(const Vector3& rotation) const;
	void SetScale(const Vector3& scale) const;

	void Rotate(const Vector3& rotate) const;
};