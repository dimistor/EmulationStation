#pragma once

#include <vector>
#include <string>

class VideoData
{
public:
  static VideoData* getInstance();
  std::vector<std::string> getVideoFiles();
  std::string getRandomVideo();

private:
  static VideoData* sInstance;
  VideoData();

  bool mIsCached;
  std::vector<std::string> mCachedResult;
};
