// #ifndef RENDER_GROUP_HH
// #define RENDER_GROUP_HH

// #include "platform.h"
// #include "camera.h"

// #include <vector>

// enum RenderType
// {
//     Texture,
//     Model,
//     Rectangle,
// };

// class RenderElement
// {
// public:
//     RenderType
//     type() { return type_; }

//     v3
//     position() { return position_; }

//     void*
//     element() { return element_; }

// private:
//     v3 position_;
//     RenderType type_;
//     void* element_;
// };

// class RenderGroup
// {
// public:
//     static RenderGroup
//     Allocate(MemoryArena arena, u64 size);

//     void
//     DrawTexture(Texture2D* texture, v3* position);

//     void
//     DrawModel(Model* model, v3* position);

//     void
//     RenderToOutput();

// private:
//     void *
//     PushRenderElement();

// private:
//     Camera pers_camera;
//     Camera ortho_camera;

//     std::vector<Texture> textures;
// };

// #endif