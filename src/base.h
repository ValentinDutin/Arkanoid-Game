#include <mathfu/rect.h>
#include <mathfu/vector.h>

using Rect = mathfu::Rect<float>;
using Vect = mathfu::Vector<float, 2>;
using Vect4 = mathfu::Vector<float, 4>;

#define IM_VEC2_CLASS_EXTRA                         \
        ImVec2(const Vect& f) { x = f.x; y = f.y; } \
        operator Vect() const { return Vect(x,y); }

#define IM_VEC4_CLASS_EXTRA                         \
        ImVec4(const Vect4& f) { x = f.x; y = f.y; z = f.z; w = f.w; } \
        operator Vect4() const { return Vect4(x,y,z,w); }
#include <imgui.h>
