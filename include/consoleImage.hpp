#pragma once

#include "common.hpp"

namespace cp
{
	class CP_EXPORT ConsoleImage
	{
	private:
		int count;
		std::string windowName;
		std::vector<std::string> strings;
		bool isLineNumber;
		int fontSize;
		int lineSpaceSize;
	public:
		void setFontSize(int size);
		void setLineSpaceSize(int size);

		void setIsLineNumber(bool isLine = true);
		bool getIsLineNumber();
		cv::Mat image;

		void init(cv::Size size, std::string wname);
		ConsoleImage();
		ConsoleImage(cv::Size size, std::string wname = "console");
		~ConsoleImage();

		void printData();
		void clear();

		void operator()(std::string str);
		void operator()(const char *format, ...);
		void operator()(cv::Scalar color, std::string str);
		void operator()(cv::Scalar color, const char *format, ...);
		
		void show(bool isClear = true);
	};
}