#ifndef _SCENE_H_
#define _SCENE_H_

#include <RayTracing/Hitable.h>
#include <RayTracing/RayCamera.h>

namespace RayTracing {
	class Scene : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Scene)
	public:
const 		Scene(const Hitable::Ptr & obj, const RayCamera::Ptr & camera);

		Hitable::Ptr obj;
		const RayCamera::Ptr camera;
	};
}

#endif // !_SCENE_H_
