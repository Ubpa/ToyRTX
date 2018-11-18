#ifndef _SCENE_H_
#define _SCENE_H_

#include <RayTracing/Hitable.h>
#include <RayTracing/RayCamera.h>

namespace RayTracing {
	class Scene : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Scene)
	public:
		const Scene(const Hitable::CPtr & obj, const RayCamera::CPtr & camera);

		Hitable::CPtr obj;
		const RayCamera::CPtr camera;
	};
}

#endif // !_SCENE_H_
