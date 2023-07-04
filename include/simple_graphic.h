#pragma once

#include "check_gl.hpp"  // includes glad/glad.h

namespace project_one {
static void DrawPoint() {
  // GLEnable should be called before glBegin once.
  // GLDisable can be used to disable the function.
  // OpenGL is a state machine.

  // Q: Why do we need to enable GL_POINT_SMOOTH and GL_BLEND?
  // A: GL_POINT_SMOOTH is for anti-aliasing. GL_BLEND is for transparency.
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_BLEND);
  // Q: What is the difference between GL_SRC_ALPHA and GL_ONE_MINUS_SRC_ALPHA?
  // A: GL_SRC_ALPHA is the source alpha value. GL_ONE_MINUS_SRC_ALPHA is 1 -
  // source alpha value.
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPointSize(128.0f);
  glBegin(GL_POINTS);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glEnd();
}

static void DrawTriangle() {
  glBegin(GL_TRIANGLES);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.5f, 0.0f);
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-0.5f, -0.5f, 0.0f);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0.5f, -0.5f, 0.0f);
  CHECK_GL(glEnd());
}

static void DrawCircle() {
  glBegin(GL_TRIANGLES);
  constexpr int n = 100;
  constexpr float pi = 3.1415926535897f;
  float radius = 0.5f;
  float inner_radius = 0.25f;
  static int x = 0;
  x++;
  if (x > n) x %= n;
  for (int i = 0; i < x; ++i) {
    float angle = i / (float)n * pi * 2;
    float angle_next = (i + 1) / (float)n * pi * 2;
    // glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(radius * sinf(angle), radius * cosf(angle), 0.0f);
    glVertex3f(radius * sinf(angle_next), radius * cosf(angle_next), 0.0f);
    glVertex3f(inner_radius * sinf(angle), inner_radius * cosf(angle), 0.0f);
    glVertex3f(inner_radius * sinf(angle_next), inner_radius * cosf(angle_next),
               0.0f);
    glVertex3f(inner_radius * sinf(angle), inner_radius * cosf(angle), 0.0f);
    glVertex3f(radius * sinf(angle_next), radius * cosf(angle_next), 0.0f);
  }
  CHECK_GL(glEnd());
}

// GL_TRIANGLE_FAN
// GL_TRIANGLE_STRIP

struct PartieCircle {
  // center
  float x;
  float y;
  float z;

  float radius;
  float inner_radius;

  float start_angle;
  float end_angle;

  // color
  float r;
  float g;
  float b;
};

static void DrawCircle(const PartieCircle& params) {
  constexpr float pi = 3.1415926535897f;
  constexpr float resolution = pi / 100.0f;

  glColor3f(params.r, params.g, params.b);
  const int n = (params.end_angle - params.start_angle) / resolution;
  const float radius = params.radius;
  const float inner_radius = params.inner_radius;

  for (int i = 0; i < n; ++i) {
    float lhv = params.start_angle + i * resolution;
    float rhv = params.start_angle + (i + 1) * resolution;
    glVertex3f(params.x + radius * sinf(lhv), params.y + radius * cosf(lhv),
               0.0f);
    glVertex3f(params.x + radius * sinf(rhv), params.y + radius * cosf(rhv),
               0.0f);
    glVertex3f(params.x + inner_radius * sinf(lhv),
               params.y + inner_radius * cosf(lhv), 0.0f);

    glVertex3f(params.x + inner_radius * sinf(rhv),
               params.y + inner_radius * cosf(rhv), 0.0f);
    glVertex3f(params.x + inner_radius * sinf(lhv),
               params.y + inner_radius * cosf(lhv), 0.0f);
    glVertex3f(params.x + radius * sinf(rhv), params.y + radius * cosf(rhv),
               0.0f);
  }
}

static void DrawOpenCV() {
  glBegin(GL_TRIANGLES);
  constexpr float my_pi = 3.1415926535897f;
  constexpr float my_pi_2 = my_pi / 2.0f;
  constexpr float my_pi_3 = my_pi / 3.0f;
  constexpr float my_pi_4 = my_pi / 4.0f;
  constexpr float my_pi_6 = my_pi / 6.0f;
  constexpr float my_pi_8 = my_pi / 8.0f;

  const float total_rads = 2 * my_pi - my_pi_3;

  const float start_red = my_pi + my_pi_6;
  const float end_red = start_red + total_rads;

  const float start_green = my_pi_2;
  const float end_green = start_green + total_rads;

  const float start_blue = my_pi_6;
  const float end_blue = start_blue + total_rads;

  const float range = 0.3f;
  const float length = 2 * range * cosf(my_pi_6);
  const float height = 2 * range * sinf(my_pi_6);

  PartieCircle params{0.0f,      range,   0.0f, 0.3f, 0.1f,
                      start_red, end_red, 1.0f, 0.0f, 0.0f};

  PartieCircle params2{-length,     -height,   0.0f, 0.3f, 0.1f,
                       start_green, end_green, 0.0f, 1.0f, 0.0f};

  PartieCircle params3{length,     -height,  0.0f, 0.3f, 0.1f,
                       start_blue, end_blue, 0.0f, 0.0f, 1.0f};
  DrawCircle(params);
  DrawCircle(params2);
  DrawCircle(params3);
  CHECK_GL(glEnd());
}
}  // namespace project_one