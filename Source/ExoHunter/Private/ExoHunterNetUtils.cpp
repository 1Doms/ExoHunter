#include "ExoHunterNetUtils.h"
#include <enet6/enet.h>

int32 ExoHunterNetUtils::GetENetAddressType(EExoIPVersion IPVersion)
{
	switch (IPVersion)
	{
	case EExoIPVersion::IPv4: return ENET_ADDRESS_TYPE_IPV4;
	case EExoIPVersion::IPv6: return ENET_ADDRESS_TYPE_IPV6;
	case EExoIPVersion::Any:  return ENET_ADDRESS_TYPE_ANY;
	}
	
	// Par sécurité (fallback) si jamais la valeur est invalide
	return (int32)ENET_ADDRESS_TYPE_IPV4	;
}
