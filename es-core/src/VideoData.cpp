#include <random>
#include <boost/filesystem.hpp>
#include "VideoData.h"
#include "platform.h"
#include "Log.h"

namespace fs = boost::filesystem;

VideoData* VideoData::sInstance = nullptr;

VideoData::VideoData()
{
}

VideoData* VideoData::getInstance()
{
	if(sInstance == nullptr)
		sInstance = new VideoData();

	return sInstance;
}

std::vector<std::string> VideoData::getVideoFiles()
{
  if(!mIsCached)
  {
    std::vector<fs::path> files;

    static const size_t pathCount = 2;
    fs::path paths[pathCount] = {
      "/etc/emulationstation/videos",
      getHomePath() + "/.emulationstation/videos"
    };

    for(size_t i = 0; i < pathCount; i++)
    {
      if(!fs::is_directory(paths[i]))
        continue;

      auto begin = fs::directory_iterator(paths[i]);
      auto end = fs::directory_iterator();
      std::copy_if(begin, end, std::back_inserter(files), [](fs::path path) {
        return fs::is_regular_file(path);
      });
    }

    std::sort(files.begin(), files.end());
    std::transform(files.begin(), files.end(), std::back_inserter(mCachedResult), [](fs::path path) {
      return path.string();
    });

    mIsCached = true;
  }

  return mCachedResult;
}

std::string VideoData::getRandomVideo()
{
  auto files = getVideoFiles();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, files.size() - 1);

  std::string path = files.at(dis(gen));

  LOG(LogDebug) << "Randomly chosen video: " << path;
  return path;
}
