#pragma once

// System Headers
#include <glad/glad.h>

// Standard headers
#include <string>

// Constants
const int         windowWidth     = 1920;
const int         windowHeight    = 1080;
const std::string windowTitle     = "Cosy";
const GLint       windowResizable = GL_TRUE;
const int         windowSamples   = 4;

struct CommandLineOptions {
    bool enableMusic;
    bool enableAutoplay;
};