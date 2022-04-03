
// Author: Pierce Brooks

#ifndef LOG_HPP
#define LOG_HPP

#include <sstream>
#include <android/log.h>

#define TAG "CV_NATIVE"
#define LOGI(tag, message) __android_log_print(ANDROID_LOG_INFO, tag, "%s", message)
#define LOGD(tag, message) __android_log_print(ANDROID_LOG_DEBUG, tag, "%s", message)
#define LOGE(tag, message) __android_log_print(ANDROID_LOG_ERROR, tag, "%s", message)
#define LOGW(tag, message) __android_log_print(ANDROID_LOG_WARN, tag, "%s", message)
#define LOGV(tag, message) __android_log_print(ANDROID_LOG_VERBOSE, tag, "%s", message)
#define LOGGER *sfml::Log::getSelf()
#define LOG(message) sfml::Log::logThere(message)

namespace sfml
{
    class Log
    {
        public:
            Log();
            virtual ~Log();
            template <typename T>
            Log& operator <<(const T& message)
            {
                logHere(message);
                return *this;
            }
            template <typename T>
            void logHere(const T& message)
            {
                *ss << message;
                LOGD(TAG, ss->str().c_str());
                ss->str("");
            }
            template <typename T>
            static void logThere(const T& message)
            {
                getSelf()->logHere(message);
            }
            static Log* getSelf();
        protected:
            std::stringstream* ss;
            static Log* self;
    };
}

#endif
