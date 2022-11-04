cmake_minimum_required (VERSION 3.8)

add_executable (ptLeftarCore "ptLeftarCore.cpp" "Math/Vector3.h" "Math/Ray.h" "Math/Vector3.cpp" "Graphics/Primitives/Primitive.h" "Graphics/Primitives/Triangle.h" "Graphics/Color.h" "Graphics/Primitives/Triangle.cpp" "Graphics/Primitives/Primitive.cpp" "Math/Camera.h" "Math/Camera.cpp" "../external/objloader.h" "Graphics/Vertex.h" "Graphics/Octree.h" "Graphics/OctreeNode.h" "Graphics/Octree.cpp" "Graphics/OctreeNode.cpp" "Graphics/BoundingBox.h" "Graphics/BoundingBox.cpp" "Graphics/Texture.h" "Graphics/Texture.cpp" "Graphics/Material.h" "Math/Base.h" "Math/Base.cpp" "Graphics/Primitives/Sphere.h" "Graphics/Primitives/Sphere.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ptLeftarCore PROPERTY CXX_STANDARD 20)
endif()
