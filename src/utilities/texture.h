#include <glad/glad.h>
#include <fmt/format.h>

#include "imageLoader.hpp"

unsigned int getTextureID(PNGImage *image);
unsigned int getCubeMapID(std::vector<std::string> images);