#include <hjson.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include "hjson_test.h"


static std::string _readStream(std::ifstream *pInfile) {
  assert(pInfile->is_open());

  std::string ret;
  ret.resize(pInfile->tellg());
  pInfile->seekg(0, std::ios::beg);
  pInfile->read(&ret[0], ret.size());
  pInfile->close();

  return ret;
}


static std::string _readFile(std::string path) {
  std::ifstream infile(path, std::ifstream::ate);

  return _readStream(&infile);
}


static std::string _getTestContent(std::string name) {
  std::ifstream infile("assets/" + name + "_test.hjson",
    std::ifstream::ate | std::ifstream::binary);

  if (!infile.is_open()) {
    infile.open("assets/" + name + "_test.json",
      std::ifstream::ate | std::ifstream::binary);
  }

  return _readStream(&infile);
}


static void _examine(std::string filename) {
  size_t pos = filename.find("_test.");
  if (pos == std::string::npos) {
    return;
  }
  std::string name(filename.begin(), filename.begin() + pos);

  std::cout << "running " << name << '\n';

  bool shouldFail = !name.compare(0, 4, "fail");

  auto testContent = _getTestContent(name);
  Hjson::Value root;
  try {
    root = Hjson::Unmarshal(testContent.c_str(), testContent.size());
    if (shouldFail) {
      std::cout << "Should have failed on " << name << std::endl;
      assert(false);
    }
  } catch (Hjson::syntax_error e) {
    if (!shouldFail) {
      std::cout << "Should NOT have failed on " << name << std::endl;
      assert(false);
    } else {
      return;
    }
  }

  auto rhjson = _readFile("assets/sorted/" + name + "_result.hjson");
  auto actualHjson = Marshal(root);

  //std::ofstream outputFile("out.hjson");
  //outputFile << actualHjson;
  //outputFile.close();

  // Visual studio will have trailing null chars in rhjson if there was any
  // CRLF conversion when reading it from the file. If so, `==` would return
  // `false`, therefore we need to use `strcmp`.
  if (std::strcmp(rhjson.c_str(), actualHjson.c_str())) {
    for (int a = 0; a < rhjson.size() && a < actualHjson.size(); a++) {
      if (actualHjson[a] != rhjson[a]) {
        std::cout << std::endl << "first diff at index " << a << std::endl;
        break;
      }
    }
    std::cout << std::endl << "Expected: (size " << rhjson.size() << ")" <<
      std::endl << rhjson << std::endl << std::endl << "Got: (size " <<
      actualHjson.size() << ")" << std::endl << actualHjson << std::endl <<
      std::endl;
    assert(std::strcmp(rhjson.c_str(), actualHjson.c_str()));
  }

  auto rjson = _readFile("assets/sorted/" + name + "_result.json");
  auto actualJson = MarshalJson(root);

  //std::ofstream outputFile("out.json");
  //outputFile << actualJson;
  //outputFile.close();

  // Visual studio will have trailing null chars in rjson if there was any
  // CRLF conversion when reading it from the file. If so, `==` would return
  // `false`, therefore we need to use `strcmp`.
  if (std::strcmp(rjson.c_str(), actualJson.c_str())) {
    for (int a = 0; a < rjson.size() && a < actualJson.size(); a++) {
      if (actualJson[a] != rjson[a]) {
        std::cout << std::endl << "first diff at index " << a << std::endl;
        break;
      }
    }
    std::cout << std::endl << "Expected: (size " << rjson.size() << ")" <<
      std::endl << rjson << std::endl << std::endl << "Got: (size " <<
      actualJson.size() << ")" << std::endl << actualJson << std::endl <<
      std::endl;
    assert(std::strcmp(rjson.c_str(), actualJson.c_str()));
  }
}


void test_marshal() {
  std::ifstream infile("assets/testlist.txt");

  std::string line;
  while (std::getline(infile, line)) {
    _examine(line);
  }
}
