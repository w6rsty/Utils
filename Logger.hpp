#pragma once

#include "core/core.hpp"
#include "core/base.hpp"

#include "debugbreak.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <functional>
#include <unordered_map>

namespace acid::log
{

enum class Level
{
    None = 0,
    Trace,
    Info,
    Warn,
    Error,
    Fatal,
};

static const std::string_view LevelToString(Level level)
{
    switch (level)
    {
        case Level::Trace:  return "[Trace]";
        case Level::Info:   return "[Info]";
        case Level::Warn:   return "[Warn]";
        case Level::Error:  return "[Error]";
        case Level::Fatal:  return "[Fatal]";
        case Level::None:   return "[]";
    }
}

/// Manage loggering to a certain stream.
/// Notice that stream can not change once been set.
class Logger
{
public:
    Logger(std::ostream& stream, Level level = Level::None, std::function<void(void)> createFn = nullptr)
        : stream_(stream), level_(level)
    {
        if (createFn) createFn();
    }

    ~Logger()
    {
        if (destoryFn_) destoryFn_();
    }

    void SetLevel(Level level) { level_ = level; }
    Level GetLevel() const { return level_; }

    /// Set custom destory callback.
    void SetDestoryCallback(std::function<void()> fn) { destoryFn_ = fn; }

    template <typename... Args>
    void Trace(std::string_view fileName, std::string function, uint32_t line, Args&&... args)
    {
        LogImpl(Level::Trace, fileName, function, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Info(std::string_view fileName, std::string function, uint32_t line, Args&&... args)
    {
        LogImpl(Level::Info, fileName, function, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Warn(std::string_view fileName, std::string function, uint32_t line, Args&&... args)
    {
        LogImpl(Level::Warn, fileName, function, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Error(std::string_view fileName, std::string function, uint32_t line, Args&&... args)
    {
        LogImpl(Level::Error, fileName, function, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Fatal(std::string_view fileName, std::string function, uint32_t line, Args&&... args)
    {
        LogImpl(Level::Fatal, fileName, function, line, std::forward<Args>(args)...);
    }
private:
    template <typename... Args>
    void LogImpl(Level level, std::string_view fileName, std::string funcName, uint32_t line, Args&&... args)
    {
        // filter out low level logs
        if (level < level_) return;

        stream_ << LevelToString(level) << " " << fileName << ":" << line << ":" << funcName << "\n";
        stream_ << ">> "; 
        doLogImpl(std::forward<Args>(args)...);
        stream_ << "\n";
    }

    template <typename Param, typename... Params>
    void doLogImpl(Param&& param, Params&&... params)
    {
        stream_ << param;
        doLogImpl(std::forward<Params>(params)...);
    }

    template <typename Param>
    void doLogImpl(Param&& param)
    {
        stream_ << param;
    }
private:
    std::ostream& stream_;
    Level level_;

    std::function<void()> destoryFn_; 
};

class LoggerManager
{
public:
    static LoggerManager& Instance() {
        static Scope<LoggerManager> instance;
        if (instance.get() == nullptr) {
            instance.reset(new LoggerManager());
        }
        return *instance;
    }

    Logger& GetDefaultLogger() { return *defaultLogger_; }
    const Logger& GetDefault() const { return *defaultLogger_; }

    Logger& AddLogger(const std::string& name, std::ostream& stream)
    {
        if (loggers_.find(name) != loggers_.end()) {
            // this should not happen, but I keep it here for debug purpose
            debug_break();
        }
        Logger* logger = new Logger(stream);
        loggers_[name].reset(logger);
        return *logger;
    }

    Logger& GetLogger(const std::string& name) { return *loggers_[name]; }
    const Logger& GetLogger(const std::string& name) const { return *loggers_.at(name); }
private:
    LoggerManager() {
        defaultLogger_.reset(new Logger(std::cout));
    }
private:
    Scope<Logger> defaultLogger_;
    std::unordered_map<std::string, Scope<Logger>> loggers_;
};

#ifdef ACID_ENABLE_LOGGING
    #define AC_LOG_TRACE(...) ::acid::log::LoggerManager::Instance().GetDefaultLogger().Trace(__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
    #define AC_LOG_INFO(...)  ::acid::log::LoggerManager::Instance().GetDefaultLogger().Info(__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
    #define AC_LOG_WARN(...)  ::acid::log::LoggerManager::Instance().GetDefaultLogger().Warn(__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
    #define AC_LOG_ERROR(...) ::acid::log::LoggerManager::Instance().GetDefaultLogger().Error(__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
    #define AC_LOG_FATAL(...) ::acid::log::LoggerManager::Instance().GetDefaultLogger().Fatal(__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)

    #define AC_TAG_TRACE(tag, ...) ::acid::log::LoggerManager::Instance().GetDefaultLogger().Trace(__FILE__, __PRETTY_FUNCTION__, __LINE__, "\x1b[31m[", tag "]\x1b[0m ", __VA_ARGS__)
    #define AC_TAG_INFO(tag, ...)  ::acid::log::LoggerManager::Instance().GetDefaultLogger().Info(__FILE__, __PRETTY_FUNCTION__, __LINE__, "\x1b[31m[", tag "]\x1b[0m ", __VA_ARGS__)
    #define AC_TAG_WARN(tag, ...)  ::acid::log::LoggerManager::Instance().GetDefaultLogger().Warn(__FILE__, __PRETTY_FUNCTION__, __LINE__, "\x1b[31m[", tag "]\x1b[0m ", __VA_ARGS__)
    #define AC_TAG_ERROR(tag, ...) ::acid::log::LoggerManager::Instance().GetDefaultLogger().Error(__FILE__, __PRETTY_FUNCTION__, __LINE__, "\x1b[31m[", tag "]\x1b[0m ", __VA_ARGS__)
    #define AC_TAG_FATAL(tag, ...) ::acid::log::LoggerManager::Instance().GetDefaultLogger().Fatal(__FILE__, __PRETTY_FUNCTION__, __LINE__, "\x1b[31m[", tag "]\x1b[0m ", __VA_ARGS__)
#else
    #define AC_LOG_TRACE(...)
    #define AC_LOG_INFO(...)
    #define AC_LOG_WARN(...)
    #define AC_LOG_ERROR(...)
    #define AC_LOG_FATAL(...)

    #define AC_TAG_TRACE(tag, ...)
    #define AC_TAG_INFO(tag, ...)
    #define AC_TAG_WARN(tag, ...)
    #define AC_TAG_ERROR(tag, ...)
    #define AC_TAG_FATAL(tag, ...)
#endif

} // namespace acid
