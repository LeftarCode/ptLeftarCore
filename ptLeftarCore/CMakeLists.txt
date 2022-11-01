# CMakeList.txt: projekt CMake dla elementu ptLeftarCore, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.8)

# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (ptLeftarCore "ptLeftarCore.cpp"  "Math/Vector3.h" "Math/Ray.h" "Math/Vector3.cpp" "Graphics/Primitive.h" "Graphics/Triangle.h" "Graphics/Color.h" "Graphics/Triangle.cpp" "Graphics/Primitive.cpp" "Math/Camera.h" "Math/Camera.cpp" "../external/objloader.h" "Graphics/Vertex.h" "Graphics/Octree.h" "Graphics/OctreeNode.h" "Graphics/Octree.cpp" "Graphics/OctreeNode.cpp" "Graphics/AABB.h" "Graphics/AABB.cpp"   "Graphics/Texture.h" "Graphics/Texture.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ptLeftarCore PROPERTY CXX_STANDARD 20)
endif()

# TODO: Dodaj testy i zainstaluj elementy docelowe w razie potrzeby.
