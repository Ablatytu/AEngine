#include "../Renderer/GameObject.h"
#include "Rei.h"

void Rei::OnInspector()
{
	RegisterField(speedRange);
	Range(speedRange, 0, 100);
	RegisterField(speedWithoutRange);
	RegisterField(flag);
	RegisterField(countRange);
	Range(countRange, 0, 5);
	RegisterField(countWithoutRange)
}

AE_REGISTER_COMPONENT(Rei);