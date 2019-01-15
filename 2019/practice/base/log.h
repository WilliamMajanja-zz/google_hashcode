#pragma once

#define LOG(message) if (enable_logging) { clog << "[" << class_name_ << "::" << __func__ << "]: " << message << endl; }

