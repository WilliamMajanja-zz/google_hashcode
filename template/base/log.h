#pragma once

#define LOG(message) do { if (enable_logging) { clog << "[" << class_name_ << "::" << __func__ << "]: " << message << endl; } } while (false);

#ifdef DEBUG
#define DBG(message) do { if (enable_logging) { clog << "<" << class_name_ << "::" << __func__ << ">: " << message << endl; } } while (false);
#else
#define DBG(message) do {} while (false);
#endif

#define ASSERT(condition, message) do { if (!(condition)) { LOG(message); exit(0); } } while (false);
