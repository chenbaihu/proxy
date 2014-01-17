#include <rapidjson/document.h>

#include "commontypes.h"
#include "Config.h"


Config::Config():enableEventDebug(true){

}

int Config::parseFromFile(const std::string& filename){
  rapidjson::Document doc;
  std::string data = slib::readFileAsString(filename.c_str());
  doc.Parse<0>(data.c_str());
  return 0;
}


Config::~Config()
{
}
