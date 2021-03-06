/**
 *
 *  |_  _ _
 *  |_)| (/_VV
 *
 *  Copyright 2015-2018 Marcus v. Keil
 *
 *  Created on: Dec 9, 2015
 *
 */

#include <brew/core/Log.h>

#include <iomanip>

namespace brew {

/*************************************************************************************************
 * LogTag class
 *************************************************************************************************/

LogTag::LogTag(const String& tag)
        : tag(tag) {

}

/*************************************************************************************************
 * LogStream class
 *************************************************************************************************/

LogStream::LogStream(AbstractLog& log)
        : log(log) {
}

LogStream::~LogStream() {
    flush();
}

LogStream::LogStream(const LogStream& other)
        : log(other.log) {
    // That sucks.
    // Is there *any* better way than having to make a const cast here?
    // The copy constructor is private, though there may not be any abuse.

    static NullLog nullLog;
    auto& ls = const_cast<LogStream&>(other);
    ls.log = nullLog;
    ls.buffer.str("");
}

void LogStream::flush() {
    String msg = buffer.str();
    log.get().record(currentTag, msg);
    buffer.str("");
}

LogStream& LogStream::operator<<(const LogTag& tag) {
    currentTag = tag.tag;
    return *this;
}

LogStream& LogStream::operator<<(const LogLevel& level) {
    // Todo: Implement log level handling.
    return *this;
}

/*************************************************************************************************
 * AbstractLog class
 *************************************************************************************************/

LogStream AbstractLog::stream(const String& tag) {
    LogStream ls(*this);
    ls << LogTag(tag);
    return ls;
}

void AbstractLog::record(const String& tag, const String& message) {
    mutex.lock();
    _record(tag, message);
    mutex.unlock();
}

/*************************************************************************************************
 * StreamLog class
 *************************************************************************************************/

StreamLog::StreamLog(std::ostream& os)
        : ostream(os) {

}

void StreamLog::_record(const String& tag, const String& message) {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    StringStream timeStr;

#if BREW_COMPILER == BREW_COMPILER_GCC && __GNUC__ < 5
    // g++ 4.x doesn't know std::put_time.
    char time[64];
    strftime(time, sizeof(time), "%H:%M:%S", std::localtime(&now_c));
    timeStr << time;
#else
    timeStr << std::put_time(std::localtime(&now_c), "%H:%M:%S");
#endif

    ostream << timeStr.str() << " [" << tag << "]: " << message << std::endl;
    ostream.flush();
}

/*************************************************************************************************
 * NullLog
 *************************************************************************************************/

void NullLog::_record(const String& tag, const String& message) {
    // Doh.
}

} /* namespace brew */
