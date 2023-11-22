#include "Sun.hpp"

Sun::Sun() {
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    shader = new Shader();
    shader->AttachShader("shaders/Sun/sun.vert", VERT)
        ->AttachShader("shaders/Sun/sun.frag", FRAG)
        ->AttachShader("shaders/Sun/sun.geom", GEOM)
        ->LinkProgram();

}

Shader* Sun::getShad() {
    return shader;
}

void Sun::Draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, 1);
}

