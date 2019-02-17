#pragma once

#define LOG(message) if (enable_logging) { clog << "[" << class_name_ << "::" << __func__ << "]: " << message << endl; }

#ifdef DEBUG
#define DBG(message) if (enable_logging) { clog << "<" << class_name_ << "::" << __func__ << ">: " << message << endl; }
#else
#define DBG(message)
#endif

#define ASSERT(condition, message) do { if (!(condition)) { LOG(message); exit(0); } } while (false);
