#ifndef ANTHOLOGY_OPENGL_H
#define ANTHOLOGY_OPENGL_H

inline void gl_draw_texture(Texture2D* texture, v2 position, Camera* camera);

inline void gl_draw_model(Model* model, v3* position, Camera* camera);

inline void gl_delete_texture(Texture2D* texture);

#endif