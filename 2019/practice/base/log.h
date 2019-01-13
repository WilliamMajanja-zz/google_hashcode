#pragma once

#define LOG(message) if (enable_logging) { std::clog << "[" << class_name_ << "::" << __func__ << "]: " << message << std::endl; }

