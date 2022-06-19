#include <iostream>
#include <string>

#include "image.hpp"
#include "render.hpp"
#include "scene_parser.hpp"

using namespace std;

int main(int argc, char *argv[]) {
  for (int argNum = 1; argNum < argc; ++argNum) {
    cout << "Argument " << argNum << " is: " << argv[argNum] << endl;
  }

  if (argc != 3) {
    cout << "Usage: ./bin/UE4 <input scene file> <output bmp file>" << endl;
    return 1;
  }

  string inputFile = argv[1];
  string outputFile = argv[2];  // only bmp is allowed.

  SceneParser sceneParser(inputFile.c_str());
  PTRenderer renderer(sceneParser);
  Image renderedImage = renderer.render();
  renderedImage.SaveImage(outputFile.c_str());

  return 0;
}
