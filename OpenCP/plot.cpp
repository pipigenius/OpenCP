#include "plot.hpp"
#include "draw.hpp"
#include "stereo_core.hpp"//for RGB histogram
#include "pointcloud.hpp"//for RGB histogram
#include <iostream>
using namespace std;
using namespace cv;

namespace cp
{
	gnuplot::gnuplot(string gnuplotpath)
	{

		if ((fp = _popen(gnuplotpath.c_str(), "w")) == NULL)
		{
			fprintf(stderr, "Cannot open gnuplot @ %s\n", gnuplotpath.c_str());
			exit(1);
		}
	}

	void gnuplot::cmd(string name)
	{
		fprintf(fp, name.c_str());
		fprintf(fp, "\n");
		fflush(fp);
	}

	gnuplot::~gnuplot()
	{
		//fclose(fp);
		cmd("exit");
		_pclose(fp);
	}


	void plotGraph(OutputArray graph_, vector<Point2d>& data, double xmin, double xmax, double ymin, double ymax,
		Scalar color, int lt, int isLine, int thickness, int pointSize)

	{
		CV_Assert(!graph_.empty());
		const int ps = pointSize;

		Mat graph = graph_.getMat();
		double x = (double)graph.cols / (xmax - xmin);
		double y = (double)graph.rows / (ymax - ymin);

		int H = graph.rows - 1;
		const int size = (int)data.size();

		for (int i = 0; i < size; i++)
		{
			double src = data[i].x;
			double dest = data[i].y;

			cv::Point p = Point(cvRound(x*(src - xmin)), H - cvRound(y*(dest - ymin)));

			if (isLine == Plot::LINE_NONE)
			{
				;
			}
			else if (isLine == Plot::LINE_LINEAR)
			{
				if (i != size - 1)
				{
					double nsrc = data[i + 1].x;
					double ndest = data[i + 1].y;
					line(graph, p, Point(cvRound(x*(nsrc - xmin)), H - cvRound(y*(ndest - ymin))),
						color, thickness);
				}
			}
			else if (isLine == Plot::LINE_H2V)
			{
				if (i != size - 1)
				{
					double nsrc = data[i + 1].x;
					double ndest = data[i + 1].x;
					line(graph, p, Point(cvRound(x*(nsrc - xmin)), p.y), color, thickness);
					line(graph, Point(cvRound(x*(nsrc - xmin)), p.y), Point(cvRound(x*(nsrc - xmin)), H - cvRound(y*(ndest - ymin))), color, thickness);
				}
			}
			else if (isLine == Plot::LINE_V2H)
			{
				if (i != size - 1)
				{
					double nsrc = data[i + 1].x;
					double ndest = data[i + 1].x;
					line(graph, p, Point(p.x, H - cvRound(y*(ndest - ymin))), color, thickness);
					line(graph, Point(p.x, H - cvRound(y*(ndest - ymin))), Point(cvRound(x*(nsrc - xmin)), H - cvRound(y*(ndest - ymin))), color, thickness);
				}
			}

			if (lt == Plot::SYMBOL_NOPOINT)
			{
				;
			}
			else if (lt == Plot::SYMBOL_PLUS)
			{
				drawPlus(graph, p, 2 * ps + 1, color, thickness);
			}
			else if (lt == Plot::SYMBOL_TIMES)
			{
				drawTimes(graph, p, 2 * ps + 1, color, thickness);
			}
			else if (lt == Plot::SYMBOL_ASTERRISK)
			{
				drawAsterisk(graph, p, 2 * ps + 1, color, thickness);
			}
			else if (lt == Plot::SYMBOL_CIRCLE)
			{
				circle(graph, p, ps, color, thickness);
			}
			else if (lt == Plot::SYMBOL_RECTANGLE)
			{
				rectangle(graph, Point(p.x - ps, p.y - ps), Point(p.x + ps, p.y + ps),
					color, thickness);
			}
			else if (lt == Plot::SYMBOL_CIRCLE_FILL)
			{
				circle(graph, p, ps, color, FILLED);
			}
			else if (lt == Plot::SYMBOL_RECTANGLE_FILL)
			{
				rectangle(graph, Point(p.x - ps, p.y - ps), Point(p.x + ps, p.y + ps),
					color, FILLED);
			}
			else if (lt == Plot::SYMBOL_TRIANGLE)
			{
				triangle(graph, p, 2 * ps, color, thickness);
			}
			else if (lt == Plot::SYMBOL_TRIANGLE_FILL)
			{
				triangle(graph, p, 2 * ps, color, FILLED);
			}
			else if (lt == Plot::SYMBOL_TRIANGLE_INV)
			{
				triangleinv(graph, p, 2 * ps, color, thickness);
			}
			else if (lt == Plot::SYMBOL_TRIANGLE_INV_FILL)
			{
				triangleinv(graph, p, 2 * ps, color, FILLED);
			}
			else if (lt == Plot::SYMBOL_DIAMOND)
			{
				diamond(graph, p, 2 * ps, color, thickness);
			}
			else if (lt == Plot::SYMBOL_DIAMOND_FILL)
			{
				diamond(graph, p, 2 * ps, color, FILLED);
			}
			else if (lt == Plot::SYMBOL_PENTAGON)
			{
				pentagon(graph, p, 2 * ps, color, thickness);
			}
			else if (lt == Plot::SYMBOL_PENTAGON_FILL)
			{
				pentagon(graph, p, 2 * ps, color, FILLED);
			}
		}
	}


	Plot::Plot()
	{
		init(cv::Size(1024, 768));
	}

	Plot::Plot(Size plotsize_)
	{
		init(plotsize_);
	}

	Plot::~Plot()
	{
		;
	}

	void Plot::point2val(cv::Point pt, double* valx, double* valy)
	{
		double x = (double)plotImage.cols / (xmax - xmin);
		double y = (double)plotImage.rows / (ymax - ymin);
		int H = plotImage.rows - 1;

		*valx = (pt.x - (origin.x) * 2) / x + xmin;
		*valy = (H - (pt.y - origin.y)) / y + ymin;
	}

	void Plot::init(cv::Size imsize)
	{
		font = "Consolas";
		fontSize = 20;
		fontSize2 = 18;

		data_max = 1;
		xlabel = "x";
		ylabel = "y";
		setBackGoundColor(COLOR_WHITE);

		origin = Point(64, 64);//default
		setPlotImageSize(imsize);

		keyImage.create(Size(256, 256), CV_8UC3);
		keyImage.setTo(background_color);

		//setXYMinMax(0, plotsize.width, 0, plotsize.height);
		isPosition = true;

		const int DefaultPlotInfoSize = 64;
		pinfo.resize(DefaultPlotInfoSize);

		vector<Scalar> c;
		//default gnuplot5.0
		c.push_back(CV_RGB(148, 0, 211));
		c.push_back(CV_RGB(0, 158, 115));
		c.push_back(CV_RGB(86, 180, 233));
		c.push_back(CV_RGB(230, 159, 0));
		c.push_back(CV_RGB(240, 228, 66));
		c.push_back(CV_RGB(0, 114, 178));
		c.push_back(CV_RGB(229, 30, 16));
		c.push_back(COLOR_BLACK);
		/*
		//classic
		c.push_back(COLOR_RED);
		c.push_back(COLOR_GREEN);
		c.push_back(COLOR_BLUE);
		c.push_back(COLOR_MAGENDA);
		c.push_back(COLOR_CYAN);
		c.push_back(COLOR_YELLOW);
		c.push_back(COLOR_BLACK);
		c.push_back(CV_RGB(0, 76, 255));
		c.push_back(COLOR_GRAY128);
		*/
		vector<int> s;
		s.push_back(SYMBOL_PLUS);
		s.push_back(SYMBOL_TIMES);
		s.push_back(SYMBOL_ASTERRISK);
		s.push_back(SYMBOL_RECTANGLE);
		s.push_back(SYMBOL_RECTANGLE_FILL);
		s.push_back(SYMBOL_CIRCLE);
		s.push_back(SYMBOL_CIRCLE_FILL);
		s.push_back(SYMBOL_TRIANGLE);
		s.push_back(SYMBOL_TRIANGLE_FILL);
		s.push_back(SYMBOL_TRIANGLE_INV);
		s.push_back(SYMBOL_TRIANGLE_INV_FILL);
		s.push_back(SYMBOL_DIAMOND);
		s.push_back(SYMBOL_DIAMOND_FILL);
		s.push_back(SYMBOL_PENTAGON);
		s.push_back(SYMBOL_PENTAGON_FILL);

		for (int i = 0; i < pinfo.size(); i++)
		{
			pinfo[i].symbolType = s[i%s.size()];
			pinfo[i].lineType = Plot::LINE_LINEAR;
			pinfo[i].thickness = 1;

			double v = (double)i / DefaultPlotInfoSize * 255.0;
			pinfo[i].color = c[i%c.size()];

			pinfo[i].keyname = format("data %02d", i);
		}

		setPlotProfile(false, true, false);
		graphImage = render;
	}

	void Plot::setPlotProfile(bool isXYCenter_, bool isXYMAXMIN_, bool isZeroCross_)
	{
		isZeroCross = isZeroCross_;
		isXYMAXMIN = isXYMAXMIN_;
		isXYCenter = isXYCenter_;
	}

	void Plot::setPlotImageSize(Size s)
	{
		plotsize = s;
		plotImage.create(s, CV_8UC3);
		render.create(Size(plotsize.width + 4 * origin.x, plotsize.height + 2 * origin.y), CV_8UC3);
	}

	void Plot::setXYOriginZERO()
	{
		recomputeXYRangeMAXMIN(false);
		xmin = 0;
		ymin = 0;
	}

	void Plot::setYOriginZERO()
	{
		recomputeXYRangeMAXMIN(false);
		ymin = 0;
	}

	void Plot::setXOriginZERO()
	{
		recomputeXYRangeMAXMIN(false);
		xmin = 0;
	}

	void Plot::recomputeXRangeMAXMIN(bool isCenter, double marginrate)
	{
		if (marginrate<0.0 || marginrate>1.0)marginrate = 1.0;
		xmax = -DBL_MAX;
		xmin = DBL_MAX;
		
		for (int i = 0; i < data_max; i++)
		{
			for (int j = 0; j < pinfo[i].data.size(); j++)
			{
				double x = pinfo[i].data[j].x;
				xmax = (xmax < x) ? x : xmax;
				xmin = (xmin > x) ? x : xmin;
			}
		}
		xmax_no_margin = xmax;
		xmin_no_margin = xmin;

		double xmargin = (xmax - xmin)*(1.0 - marginrate)*0.5;
		xmax += xmargin;
		xmin -= xmargin;

		if (isCenter)
		{
			double xxx = abs(xmax);
			xxx = (xxx < abs(xmin)) ? abs(xmin) : xxx;

			xmax = xxx;
			xmin = -xxx;
			
			xxx = abs(xmax_no_margin);
			xxx = (xxx < abs(xmin_no_margin)) ? abs(xmin_no_margin) : xxx;

			xmax_no_margin = xxx;
			xmin_no_margin = -xxx;
		}
	}

	void Plot::recomputeYRangeMAXMIN(bool isCenter, double marginrate)
	{
		if (marginrate<0.0 || marginrate>1.0)marginrate = 1.0;
		
		ymax = -DBL_MAX;
		ymin = DBL_MAX;

		for (int i = 0; i < data_max; i++)
		{
			for (int j = 0; j < pinfo[i].data.size(); j++)
			{
				double y = pinfo[i].data[j].y;
				ymax = (ymax < y) ? y : ymax;
				ymin = (ymin > y) ? y : ymin;
			}
		}
		ymax_no_margin = ymax;
		ymin_no_margin = ymin;

		double ymargin = (ymax - ymin)*(1.0 - marginrate)*0.5;
		ymax += ymargin;
		ymin -= ymargin;

		if (isCenter)
		{
			double yyy = abs(ymax);
			yyy = (yyy < abs(ymin)) ? abs(ymin) : yyy;

			ymax = yyy;
			ymin = -yyy;

			yyy = abs(ymax_no_margin);
			yyy = (yyy < abs(ymin_no_margin)) ? abs(ymin_no_margin) : yyy;

			ymax_no_margin = yyy;
			ymin_no_margin = -yyy;
		}
	}

	void Plot::recomputeXYRangeMAXMIN(bool isCenter, double marginrate)
	{
		if (marginrate<0.0 || marginrate>1.0)marginrate = 1.0;
		xmax = -DBL_MAX;
		xmin = DBL_MAX;
		ymax = -DBL_MAX;
		ymin = DBL_MAX;
		for (int i = 0; i < data_max; i++)
		{
			for (int j = 0; j < pinfo[i].data.size(); j++)
			{
				double x = pinfo[i].data[j].x;
				double y = pinfo[i].data[j].y;
				xmax = (xmax < x) ? x : xmax;
				xmin = (xmin > x) ? x : xmin;

				ymax = (ymax < y) ? y : ymax;
				ymin = (ymin > y) ? y : ymin;
			}
		}
		xmax_no_margin = xmax;
		xmin_no_margin = xmin;
		ymax_no_margin = ymax;
		ymin_no_margin = ymin;

		double xmargin = (xmax - xmin)*(1.0 - marginrate)*0.5;
		xmax += xmargin;
		xmin -= xmargin;

		double ymargin = (ymax - ymin)*(1.0 - marginrate)*0.5;
		ymax += ymargin;
		ymin -= ymargin;

		if (isCenter)
		{
			double xxx = abs(xmax);
			double yyy = abs(ymax);
			xxx = (xxx < abs(xmin)) ? abs(xmin) : xxx;
			yyy = (yyy < abs(ymin)) ? abs(ymin) : yyy;

			xmax = xxx;
			xmin = -xxx;
			ymax = yyy;
			ymin = -yyy;

			xxx = abs(xmax_no_margin);
			yyy = abs(ymax_no_margin);
			xxx = (xxx < abs(xmin_no_margin)) ? abs(xmin_no_margin) : xxx;
			yyy = (yyy < abs(ymin_no_margin)) ? abs(ymin_no_margin) : yyy;

			xmax_no_margin = xxx;
			xmin_no_margin = -xxx;
			ymax_no_margin = yyy;
			ymin_no_margin = -yyy;
		}
	}

	void Plot::setXYMinMax(double xmin_, double xmax_, double ymin_, double ymax_)
	{
		isSetXRange = true;
		isSetYRange = true;
		xmin = xmin_;
		xmax = xmax_;
		ymin = ymin_;
		ymax = ymax_;

		xmax_no_margin = xmax;
		xmin_no_margin = xmin;
		ymax_no_margin = ymax;
		ymin_no_margin = ymin;
	}

	void Plot::setXMinMax(double xmin_, double xmax_)
	{
		isSetXRange = true;
		recomputeXYRangeMAXMIN(isXYCenter);
		xmin = xmin_;
		xmax = xmax_;
	}

	void Plot::setYMinMax(double ymin_, double ymax_)
	{
		isSetYRange = true;
		recomputeXYRangeMAXMIN(isXYCenter);
		ymin = ymin_;
		ymax = ymax_;
	}

	void Plot::setBackGoundColor(Scalar cl)
	{
		background_color = cl;
	}

	void Plot::setPlotThickness(int plotnum, int thickness_)
	{
		pinfo[plotnum].thickness = thickness_;
	}

	void Plot::setPlotColor(int plotnum, Scalar color_)
	{
		pinfo[plotnum].color = color_;
	}

	void Plot::setPlotLineType(int plotnum, int lineType)
	{
		pinfo[plotnum].lineType = lineType;
	}

	void Plot::setPlotSymbol(int plotnum, int symboltype)
	{
		pinfo[plotnum].symbolType = symboltype;
	}

	void Plot::setPlotKeyName(int plotnum, string name)
	{
		pinfo[plotnum].keyname = name;
	}

	void Plot::setPlot(int plotnum, Scalar color, int symboltype, int linetype, int thickness)
	{
		setPlotColor(plotnum, color);
		setPlotSymbol(plotnum, symboltype);
		setPlotLineType(plotnum, linetype);
		setPlotThickness(plotnum, thickness);
	}

	void Plot::setPlotSymbolALL(int symboltype)
	{
		for (int i = 0; i < pinfo.size(); i++)
		{
			pinfo[i].symbolType = symboltype;
		}
	}

	void Plot::setPlotLineTypeALL(int linetype)
	{
		for (int i = 0; i < pinfo.size(); i++)
		{
			pinfo[i].lineType = linetype;
		}
	}

	void Plot::setXLabel(string xlabel)
	{
		this->xlabel = xlabel;
	}

	void Plot::setYLabel(string ylabel)
	{
		this->ylabel = ylabel;
	}

	void Plot::push_back(double x, double y, int plotIndex)
	{
		data_max = max(data_max, plotIndex + 1);
		pinfo[plotIndex].data.push_back(Point2d(x, y));
	}

	void Plot::push_back(vector<cv::Point> point, int plotIndex)
	{
		data_max = max(data_max, plotIndex + 1);
		for (int i = 0; i < (int)point.size(); i++)
		{
			push_back(point[i].x, point[i].y, plotIndex);
		}
	}

	void Plot::push_back(vector<cv::Point2d> point, int plotIndex)
	{
		data_max = max(data_max, plotIndex + 1);
		for (int i = 0; i < (int)point.size() - 1; i++)
		{
			push_back(point[i].x, point[i].y, plotIndex);
		}
	}

	void Plot::erase(int sampleIndex, int plotIndex)
	{
		pinfo[plotIndex].data.erase(pinfo[plotIndex].data.begin() + sampleIndex);
	}

	void Plot::insert(Point2d v, int sampleIndex, int plotIndex)
	{
		pinfo[plotIndex].data.insert(pinfo[plotIndex].data.begin() + sampleIndex, v);
	}

	void Plot::insert(Point v, int sampleIndex, int plotIndex)
	{
		insert(Point2d((double)v.x, (double)v.y), sampleIndex, plotIndex);
	}

	void Plot::insert(double x, double y, int sampleIndex, int plotIndex)
	{
		insert(Point2d(x, y), sampleIndex, plotIndex);
	}

	void Plot::clear(int datanum)
	{
		if (datanum < 0)
		{
			for (int i = 0; i < data_max; i++)
				pinfo[i].data.clear();

		}
		else
			pinfo[datanum].data.clear();
	}

	void Plot::swapPlot(int plotIndex1, int plotIndex2)
	{
		swap(pinfo[plotIndex1].data, pinfo[plotIndex2].data);
	}

	void boundingRectImage(InputArray src_, OutputArray dest)
	{
		Mat src = src_.getMat();
		CV_Assert(src.depth() == CV_8U);
		

		vector<Point> v;
		const int ch = src.channels();
		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				bool flag = false;
				for (int c = 0; c < ch; c++)
				{
					if (src.at<uchar>(j, ch*i + c) != 0)
					{
						flag = true;
					}
				}
				if (flag) v.push_back(Point(i, j));
			}
		}
		
		Rect r = boundingRect(v);
		src(r).copyTo(dest);
	}

	void Plot::makeBB(bool isFont)
	{
		render.setTo(background_color);
		Mat roi = render(Rect(origin.x * 2, origin.y, plotsize.width, plotsize.height));
		rectangle(plotImage, Point(0, 0), Point(plotImage.cols - 1, plotImage.rows - 1), COLOR_BLACK, 1);
		plotImage.copyTo(roi);

		Mat label = Mat::zeros(plotImage.size(), CV_8UC3);
		Mat labelximage;
		Mat labelyimage;
		if (isFont)
		{
			cv::addText(label, xlabel, Point(0, 100), font, fontSize, COLOR_WHITE);
			boundingRectImage(label, labelximage);
			label.setTo(0);
			cv::addText(label, ylabel, Point(0, 100), font, fontSize, COLOR_WHITE);
			boundingRectImage(label, labelyimage);
			
			//putText(render, xlabel, Point(render.cols / 2, (int)(origin.y*1.85 + plotImage.rows)), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, COLOR_BLACK);
			//cv::addText(render, xlabel, Point(render.cols / 2, (int)(origin.y*1.85 + plotImage.rows)), font, fontSize, COLOR_BLACK);
		
			Mat(~labelximage).copyTo(render(Rect(render.cols / 2 - labelximage.cols / 2, (int)(origin.y*1.85 + plotImage.rows - 20), labelximage.cols, labelximage.rows)));

			labelyimage = labelyimage.t();
			flip(labelyimage, labelyimage, 0);
			//putText(render, ylabel, Point(20, (int)(origin.y*0.25 + plotImage.rows / 2)), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, COLOR_BLACK);
			//cv::addText(render, ylabel, Point(20, (int)(origin.y*0.25 + plotImage.rows / 2)), font, fontSize, COLOR_BLACK);
			Mat(~labelyimage).copyTo(render(Rect(20, (int)(origin.y*0.25 + plotImage.rows / 2 - labelyimage.rows/2), labelyimage.cols, labelyimage.rows)));

			string buff;
			//x coordinate
			buff = format("%.2f", xmin);
			cv::addText(render, buff, Point(origin.x+ 30, (int)(origin.y*1.35 + plotImage.rows)), font, fontSize2, COLOR_BLACK);

			buff = format("%.2f", (xmax - xmin)*0.25 + xmin);
			cv::addText(render, buff, Point((int)(origin.x + plotImage.cols*0.25 + 40), (int)(origin.y*1.35 + plotImage.rows)), font, fontSize2, COLOR_BLACK);

			buff = format("%.2f", (xmax - xmin)*0.5 + xmin);
			cv::addText(render, buff, Point((int)(origin.x + plotImage.cols*0.5 + 45), (int)(origin.y*1.35 + plotImage.rows)), font, fontSize2, COLOR_BLACK);

			buff = format("%.2f", (xmax - xmin)*0.75 + xmin);
			cv::addText(render, buff, Point((int)(origin.x + plotImage.cols*0.75 + 45), (int)(origin.y*1.35 + plotImage.rows)), font, fontSize2, COLOR_BLACK);

			buff = format("%.2f", xmax);
			cv::addText(render, buff, Point(origin.x + plotImage.cols + 30, (int)(origin.y*1.35 + plotImage.rows)), font, fontSize2, COLOR_BLACK);

			//y coordinate
			buff = format("%.2f", ymin);
			cv::addText(render, buff, Point(origin.x, (int)(origin.y*1.0 + plotImage.rows)), font, fontSize2, COLOR_BLACK);

			buff = format("%.2f", (ymax - ymin)*0.5 + ymin);
			cv::addText(render, buff, Point(origin.x, (int)(origin.y*1.0 + plotImage.rows*0.5)), font, fontSize2, COLOR_BLACK);

			buff = format("%.2f", (ymax - ymin)*0.25 + ymin);
			cv::addText(render, buff, Point(origin.x, (int)(origin.y*1.0 + plotImage.rows*0.75)), font, fontSize2, COLOR_BLACK);

			buff = format("%.2f", (ymax - ymin)*0.75 + ymin);
			cv::addText(render, buff, Point(origin.x, (int)(origin.y*1.0 + plotImage.rows*0.25)), font, fontSize2, COLOR_BLACK);

			buff = format("%.2f", ymax);
			cv::addText(render, buff, Point(origin.x, origin.y), font, fontSize2, COLOR_BLACK);
		}
	}

	void Plot::plotPoint(Point2d point, Scalar color_, int thickness_, int linetype)
	{
		vector<Point2d> data;

		data.push_back(Point2d(point.x, ymin));
		data.push_back(Point2d(point.x, ymax));
		data.push_back(Point2d(point.x, point.y));
		data.push_back(Point2d(xmax, point.y));
		data.push_back(Point2d(xmin, point.y));

		plotGraph(plotImage, data, xmin, xmax, ymin, ymax, color_, SYMBOL_NOPOINT, linetype, thickness_);
	}

	void Plot::plotGrid(int level)
	{
		if (level > 0)
		{
			plotPoint(Point2d((xmax - xmin) / 2.0 + xmin, (ymax - ymin) / 2.0 + ymin), COLOR_GRAY150, 1);
		}
		if (level > 1)
		{
			plotPoint(Point2d((xmax - xmin)*1.0 / 4.0 + xmin, (ymax - ymin)*1.0 / 4.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*3.0 / 4.0 + xmin, (ymax - ymin)*1.0 / 4.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*1.0 / 4.0 + xmin, (ymax - ymin)*3.0 / 4.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*3.0 / 4.0 + xmin, (ymax - ymin)*3.0 / 4.0 + ymin), COLOR_GRAY200, 1);
		}
		if (level > 2)
		{
			plotPoint(Point2d((xmax - xmin)*1.0 / 8.0 + xmin, (ymax - ymin)*1.0 / 8.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*3.0 / 8.0 + xmin, (ymax - ymin)*1.0 / 8.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*1.0 / 8.0 + xmin, (ymax - ymin)*3.0 / 8.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*3.0 / 8.0 + xmin, (ymax - ymin)*3.0 / 8.0 + ymin), COLOR_GRAY200, 1);

			plotPoint(Point2d((xmax - xmin)*(1.0 / 8.0 + 0.5) + xmin, (ymax - ymin)*1.0 / 8.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(3.0 / 8.0 + 0.5) + xmin, (ymax - ymin)*1.0 / 8.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(1.0 / 8.0 + 0.5) + xmin, (ymax - ymin)*3.0 / 8.0 + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(3.0 / 8.0 + 0.5) + xmin, (ymax - ymin)*3.0 / 8.0 + ymin), COLOR_GRAY200, 1);

			plotPoint(Point2d((xmax - xmin)*(1.0 / 8.0 + 0.5) + xmin, (ymax - ymin)*(1.0 / 8.0 + 0.5) + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(3.0 / 8.0 + 0.5) + xmin, (ymax - ymin)*(1.0 / 8.0 + 0.5) + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(1.0 / 8.0 + 0.5) + xmin, (ymax - ymin)*(3.0 / 8.0 + 0.5) + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(3.0 / 8.0 + 0.5) + xmin, (ymax - ymin)*(3.0 / 8.0 + 0.5) + ymin), COLOR_GRAY200, 1);

			plotPoint(Point2d((xmax - xmin)*(1.0 / 8.0) + xmin, (ymax - ymin)*(1.0 / 8.0 + 0.5) + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(3.0 / 8.0) + xmin, (ymax - ymin)*(1.0 / 8.0 + 0.5) + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(1.0 / 8.0) + xmin, (ymax - ymin)*(3.0 / 8.0 + 0.5) + ymin), COLOR_GRAY200, 1);
			plotPoint(Point2d((xmax - xmin)*(3.0 / 8.0) + xmin, (ymax - ymin)*(3.0 / 8.0 + 0.5) + ymin), COLOR_GRAY200, 1);
		}
	}

	void Plot::makeKey(int num)
	{
		int step = 24;
		keyImage.create(Size(256, step * (num + 1) + 3), CV_8UC3);
		keyImage.setTo(background_color);

		int height = (int)(0.8*keyImage.rows);
		for (int i = 0; i < num; i++)
		{
			vector<Point2d> data;
			data.push_back(Point2d(192.0, keyImage.rows - (i + 1) * step));
			data.push_back(Point2d(keyImage.cols - step, keyImage.rows - (i + 1) * step));

			plotGraph(keyImage, data, 0, keyImage.cols, 0, keyImage.rows, pinfo[i].color, pinfo[i].symbolType, pinfo[i].lineType, pinfo[i].thickness);
			//putText(keyImage, pinfo[i].keyname, Point(0, (i + 1) * step + 3), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, pinfo[i].color);
			cv::addText(keyImage, pinfo[i].keyname, Point(0, (i + 1) * step + 3), font, fontSize, pinfo[i].color);
		}
	}

	void Plot::plotData(int gridlevel, int isKey)
	{
		plotImage.setTo(background_color);
		plotGrid(gridlevel);

		if (isZeroCross)	plotPoint(Point2d(0.0, 0.0), COLOR_ORANGE, 1);

		for (int i = 0; i < data_max; i++)
		{
			plotGraph(plotImage, pinfo[i].data, xmin, xmax, ymin, ymax, pinfo[i].color, pinfo[i].symbolType, pinfo[i].lineType, pinfo[i].thickness);
		}
		makeBB(true);

		Mat temp = render.clone();
		if (isKey != 0)
		{
			Mat roi;
			if (isKey == 1)
			{
				roi = render(Rect(render.cols - keyImage.cols - 150, 80, keyImage.cols, keyImage.rows));
			}
			else if (isKey == 4)
			{
				roi = render(Rect(render.cols - keyImage.cols - 150, render.rows - keyImage.rows - 150, keyImage.cols, keyImage.rows));
			}
			else if (isKey == 2)
			{
				roi = render(Rect(160, 80, keyImage.cols, keyImage.rows));
			}
			else if (isKey == 3)
			{
				roi = render(Rect(160, render.rows - keyImage.rows - 150, keyImage.cols, keyImage.rows));
			}

			if (isKey == 5)
			{
				imshow("key", keyImage);
			}
			else
			{
				keyImage.copyTo(roi);
				destroyWindow("key");
			}
		}
		addWeighted(render, 0.8, temp, 0.2, 0.0, render);
	}

	void Plot::save(string name)
	{
		FILE* fp = fopen(name.c_str(), "w");

		int dmax = (int)pinfo[0].data.size();
		for (int i = 1; i < data_max; i++)
		{
			dmax = max((int)pinfo[i].data.size(), dmax);
		}

		for (int n = 0; n < dmax; n++)
		{
			for (int i = 0; i < data_max; i++)
			{
				if (n < pinfo[i].data.size())
				{
					double x = pinfo[i].data[n].x;
					double y = pinfo[i].data[n].y;
					fprintf(fp, "%f %f ", x, y);
				}
				else
				{
					double x = pinfo[i].data[pinfo[i].data.size() - 1].x;
					double y = pinfo[i].data[pinfo[i].data.size() - 1].y;
					fprintf(fp, "%f %f ", x, y);
				}
			}
			fprintf(fp, "\n");
		}
		cout << "p ";
		for (int i = 0; i < data_max; i++)
		{
			cout << "'" << name << "'" << " u " << 2 * i + 1 << ":" << 2 * i + 2 << " w lp" << ",";
		}
		cout << endl;
		fclose(fp);
	}

	Scalar Plot::getPseudoColor(uchar val)
	{
		int i = val;
		double d = 255.0 / 63.0;
		Scalar ret;

		{//g
			uchar lr[256];
			for (int i = 0; i < 64; i++)
				lr[i] = cvRound(d*i);
			for (int i = 64; i < 192; i++)
				lr[i] = 255;
			for (int i = 192; i < 256; i++)
				lr[i] = cvRound(255 - d * (i - 192));

			ret.val[1] = lr[val];
		}
		{//r
			uchar lr[256];
			for (int i = 0; i < 128; i++)
				lr[i] = 0;
			for (int i = 128; i < 192; i++)
				lr[i] = cvRound(d*(i - 128));
			for (int i = 192; i < 256; i++)
				lr[i] = 255;

			ret.val[0] = lr[val];
		}
		{//b
			uchar lr[256];
			for (int i = 0; i < 64; i++)
				lr[i] = 255;
			for (int i = 64; i < 128; i++)
				lr[i] = cvRound(255 - d * (i - 64));
			for (int i = 128; i < 256; i++)
				lr[i] = 0;
			ret.val[2] = lr[val];
		}
		return ret;
	}

	static void guiPreviewMouse(int event, int x, int y, int flags, void* param)
	{
		Point* ret = (Point*)param;

		if (flags == EVENT_FLAG_LBUTTON)
		{
			ret->x = x;
			ret->y = y;
		}
	}

	void Plot::plotMat(InputArray src_, string wname, bool isWait, string gnuplotpath)
	{
		Mat src = src_.getMat();
		clear();

		if (src.depth() == CV_32F)
			for (int i = 0; i < src.size().area(); i++) push_back(i, src.at<float>(i));
		else if (src.depth() == CV_64F)
			for (int i = 0; i < src.size().area(); i++)push_back(i, src.at<double>(i));
		else if (src.depth() == CV_8U)
			for (int i = 0; i < src.size().area(); i++)push_back(i, src.at<uchar>(i));
		else if (src.depth() == CV_16U)
			for (int i = 0; i < src.size().area(); i++)push_back(i, src.at<ushort>(i));
		else if (src.depth() == CV_16S)
			for (int i = 0; i < src.size().area(); i++)push_back(i, src.at<short>(i));
		else if (src.depth() == CV_32S)
			for (int i = 0; i < src.size().area(); i++)push_back(i, src.at<int>(i));

		plot(wname, isWait, gnuplotpath);
	}

	void Plot::plot(string wname, bool isWait, string gnuplotpath, string message)
	{
		Point pt = Point(0, 0);
		namedWindow(wname);

		plotData(0, false);

		setMouseCallback(wname, (MouseCallback)guiPreviewMouse, (void*)&pt);
		int key = 0;
		static int isKey = 1;
		int gridlevel = 0;
		makeKey(data_max);

		if (!isSetXRange && !isSetYRange)
		{
			recomputeXYRangeMAXMIN(false, 0.9);
		}
		else if (!isSetXRange)
		{
			recomputeXRangeMAXMIN(false, 0.9);
		}
		else if (!isSetYRange)
		{
			recomputeYRangeMAXMIN(false, 0.9);
		}

		while (key != 'q')
		{
			plotData(gridlevel, isKey);
			if (isPosition)
			{
				double xx = 0.0;
				double yy = 0.0;
				point2val(pt, &xx, &yy);
				if (pt.x < 0 || pt.y < 0 || pt.x >= render.cols || pt.y >= render.rows)
				{
					pt = Point(0, 0);
					xx = 0.0;
					yy = 0.0;
				}
				string text = format("(%.02f,%.02f) ", xx, yy) + message;
				//putText(render, text, Point(100, 30), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, COLOR_BLACK);
				cv::addText(render, text, Point(100, 30), font, fontSize, COLOR_BLACK);
			}

			if (isPosition)drawGrid(render, pt, Scalar(180, 180, 255), 1, 4, 0);

			imshow(wname, render);
			key = waitKey(1);

			if (key == '?')
			{
				cout << "*** Help message ***" << endl;
				cout << "m: " << "show mouseover position and grid" << endl;
				cout << "c: " << "(0,0)point must posit center" << endl;
				cout << "g: " << "Show grid" << endl;

				cout << "k: " << "Show key" << endl;

				cout << "x: " << "Set X origin zero " << endl;
				cout << "y: " << "Set Y origin zero " << endl;
				cout << "z: " << "Set XY origin zero " << endl;
				cout << "r: " << "Reset XY max min" << endl;

				cout << "s: " << "Save image (plot.png)" << endl;
				cout << "q: " << "Quit" << endl;

				cout << "********************" << endl;
				cout << endl;
			}
			if (key == 'm')
			{
				isPosition = (isPosition) ? false : true;
			}

			if (key == 'r')
			{
				recomputeXYRangeMAXMIN(false);
			}
			if (key == 'c')
			{
				recomputeXYRangeMAXMIN(true);
			}
			if (key == 'x')
			{
				setXOriginZERO();
			}
			if (key == 'y')
			{
				setYOriginZERO();
			}
			if (key == 'z')
			{
				setXYOriginZERO();
			}
			if (key == 'k')
			{
				isKey++;
				if (isKey == 6)
					isKey = 0;
			}
			if (key == 'g')
			{
				gridlevel++;
				if (gridlevel > 3)gridlevel = 0;
			}
			if (key == 'p')
			{
				save("plot");
				std::string a("plot ");
				//gnuplot gplot(gnuplotpath);
				for (int i = 0; i < data_max; i++)
				{
					char name[64];
					if (i != data_max - 1)
						sprintf(name, "'plot' u %d:%d w lp,", 2 * i + 1, 2 * i + 2);
					else
						sprintf(name, "'plot' u %d:%d w lp\n", 2 * i + 1, 2 * i + 2);
					a += name;
				}
				//gplot.cmd(a.c_str());
			}
			if (key == 's')
			{
				save("plot");
				imwrite("plotim.png", render);
				//imwrite("plot.png", save);
			}

			if (key == '0')
			{
				for (int i = 0; i < pinfo[0].data.size(); i++)
				{
					cout << i << pinfo[0].data[i] << endl;
				}
			}
			if (key == 'w')
			{
				isWait = false;
			}

			if (!isWait) break;
		}

		if (isWait) destroyWindow(wname);
	}


	void Plot2D::createPlot()
	{
		graphBase = Mat::zeros(Size(countx, county), CV_64F);
	}

	void Plot2D::setMinMaxX(double minv, double maxv, int count)
	{
		minx = minv;
		maxx = maxv;
		countx = count;
	}

	void Plot2D::setMinMaxY(double minv, double maxv, int count)
	{
		miny = minv;
		maxy = maxv;
		county = count;
	}

	void Plot2D::setMinMax(double xmin, double xmax, double xstep, double ymin, double ymax, double ystep)
	{
		setMinMaxX(xmin, xmax, (int)xstep);
		setMinMaxY(ymin, ymax, (int)ystep);
		createPlot();
	}

	Plot2D::Plot2D(Size graph_size, double xmin, double xmax, double xstep, double ymin, double ymax, double ystep)
	{
		size = graph_size;
		setMinMax(xmin, xmax, xstep, ymin, ymax, ystep);
	}

	void Plot2D::add(int x, int y, double val)
	{
		//cout<<(x-minx) / stepx<< ","<<X<<","<<(y-miny) / stepy<<","<<Y<<","<<w<<","<<h<<","<<val<<endl;
		graphBase.at<double>(y, x) = val;
		//getchar();
	}

	void Plot2D::writeGraph(bool isColor, int arg_min_max, double minvalue, double maxvalue, bool isMinMaxSet)
	{
		Mat temp;
		resize(graphBase, temp, size, 0, 0, cv::INTER_LINEAR);

		double minv, maxv;
		Point minp;
		Point maxp;

		minMaxLoc(graphBase, &minv, &maxv, &minp, &maxp);
		/*cout<<minv<<","<<maxv<<endl;
		cout<<maxp<<endl;
		cout<<minp<<endl;*/

		minMaxLoc(temp, &minv, &maxv, &minp, &maxp);

		/*cout<<minv<<","<<maxv<<endl;
		cout<<maxp<<endl;
		cout<<minp<<endl;*/

		if (isMinMaxSet)
		{
			minv = minvalue;
			maxv = maxvalue;
		}

		temp -= minv;
		Mat graphG;
		temp.convertTo(graphG, CV_8U, 255.0 / (maxv - minv));

		if (isColor)
		{
			applyColorMap(graphG, graph, 2);
			if (arg_min_max > 0)
				circle(graph, maxp, 5, Scalar(255, 255, 255));
			else
				drawPlus(graph, minp, 8, Scalar(0, 0, 0));
		}
		else
		{
			cvtColor(graphG, graph, COLOR_GRAY2BGR);
			if (arg_min_max > 0)
				circle(graph, maxp, 5, Scalar(0, 0, 0));
			else
				drawPlus(graph, minp, 8, Scalar(255, 255, 255));
		}

		flip(graph, graph, 0);
	}

	void Plot2D::setLabel(string namex, string namey)
	{
		copyMakeBorder(graph, show, 0, 50, 50, 0, BORDER_CONSTANT, Scalar(255, 255, 255));

		putText(show, namex, Point(show.cols / 2 - 50, graph.rows + 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0));
		putText(show, format("%.2f", minx), Point(50, graph.rows + 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0));

		double minv, maxv;
		Point minp;
		Point maxp;
		minMaxLoc(graphBase, &minv, &maxv, &minp, &maxp);
		putText(show, format("%.2f", maxv), Point(400, graph.rows + 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0));

		Mat text = ~Mat::zeros(Size(50 + graph.cols, 50), CV_8UC3);

		putText(text, namey, Point(80, 20), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0));

		Mat a = text.t();
		flip(a, a, 0);

		a.copyTo(show(Rect(0, 0, a.cols, a.rows)));
	}

	/*
	void Plot2D::plot(CSV& result, vector<ExperimentalParameters>& parameters)
	{
	string wname = "plot2D";
	namedWindow(wname);
	for (int i = 0; i < result.data.size(); i++)
	{
	if (result.data[i][2] == 6)
	{
	int x = parameters[0].getDiscreteValueIndex(result.data[i][0]);
	int y = parameters[1].getDiscreteValueIndex(result.data[i][1]);
	this->add(x, y, result.data[i][3]);
	}
	}


	int key = 0;

	int minvalue = 30; createTrackbar("min", wname, &minvalue, 100);
	int maxvalue = 50; createTrackbar("max", wname, &maxvalue, 100);

	int xc = 0; createTrackbar("x", wname, &xc, result.width);
	int yc = 1; createTrackbar("y", wname, &yc, result.width);
	int zc = 3; createTrackbar("z", wname, &zc, result.width);

	vector<int> p(parameters.size());

	for (int i = 0; i < parameters.size(); i++)
	{
	p[i] = parameters[i].getDiscreteValue(parameters[i].value);
	createTrackbar(parameters[i].name, wname, &p[i], parameters[i].maxvalTrackbar);
	}


	bool isColor = true;
	bool isMinMaxSet = false;

	result.initFilter();
	while (key != 'q')
	{
	setMinMax(parameters[xc].discreteValue[xc], parameters[xc].discreteValue[parameters[xc].maxvalTrackbar], parameters[xc].maxvalTrackbar + 1, parameters[yc].discreteValue[yc], parameters[yc].discreteValue[parameters[yc].maxvalTrackbar], parameters[yc].maxvalTrackbar + 1);

	result.filterClear();
	for (int i = 0; i < parameters.size(); i++)
	{
	if (i != xc && i != yc)
	{
	result.makeFilter(i, parameters[i].discreteValue[p[i]]);
	//cout<<parameters[i].name<<","<<parameters[i].discreteValue[p[i]]<<endl;
	}
	}

	for (int i = 0; i < result.data.size(); i++)
	{
	if (result.filter[i])
	{
	int x = parameters[xc].getDiscreteValueIndex(result.data[i][xc]);
	int y = parameters[yc].getDiscreteValueIndex(result.data[i][yc]);
	add(x, y, result.data[i][zc]);
	}
	}

	writeGraph(isColor, PLOT_ARG_MAX, minvalue, maxvalue, isMinMaxSet);
	setLabel(parameters[xc].name, parameters[yc].name);
	imshow(wname, show);
	key = waitKey(1);
	if (key == 'c')
	{
	isColor = (isColor) ? false : true;
	}
	}
	}
	*/


	static void onMouseHistogram3D(int event, int x, int y, int flags, void* param)
	{
		Point* ret = (Point*)param;

		if (flags == EVENT_FLAG_LBUTTON)
		{
			ret->x = x;
			ret->y = y;
		}
	}

	void RGBHistogram::projectPointsParallel(const Mat& xyz, const Mat& R, const Mat& t, const Mat& K, vector<Point2f>& dest, const bool isRotationThenTranspose)
	{
		float* data = (float*)xyz.ptr<float>(0);
		Point2f* dst = &dest[0];
		int size2 = xyz.size().area();
		int i;
		float tt[3];
		tt[0] = (float)t.at<double>(0, 0);
		tt[1] = (float)t.at<double>(1, 0);
		tt[2] = (float)t.at<double>(2, 0);

		float r[3][3];
		if (isRotationThenTranspose)
		{
			const float f00 = (float)K.at<double>(0, 0);
			const float xc = (float)K.at<double>(0, 2);
			const float f11 = (float)K.at<double>(1, 1);
			const float yc = (float)K.at<double>(1, 2);

			r[0][0] = (float)R.at<double>(0, 0);
			r[0][1] = (float)R.at<double>(0, 1);
			r[0][2] = (float)R.at<double>(0, 2);

			r[1][0] = (float)R.at<double>(1, 0);
			r[1][1] = (float)R.at<double>(1, 1);
			r[1][2] = (float)R.at<double>(1, 2);

			r[2][0] = (float)R.at<double>(2, 0);
			r[2][1] = (float)R.at<double>(2, 1);
			r[2][2] = (float)R.at<double>(2, 2);

			for (i = 0; i < size2; i++)
			{
				const float x = data[0];
				const float y = data[1];
				//const float z = data[2];
				const float z = 0.f;

				const float px = r[0][0] * x + r[0][1] * y + r[0][2] * z + tt[0];
				const float py = r[1][0] * x + r[1][1] * y + r[1][2] * z + tt[1];
				const float pz = r[2][0] * x + r[2][1] * y + r[2][2] * z + tt[2];

				const float div = 1.f / pz;

				dst->x = (f00*px + xc * pz) * div;
				dst->y = (f11*py + yc * pz) * div;

				data += 3;
				dst++;
			}
		}
		else
		{
			Mat kr = K * R;

			r[0][0] = (float)kr.at<double>(0, 0);
			r[0][1] = (float)kr.at<double>(0, 1);
			r[0][2] = (float)kr.at<double>(0, 2);

			r[1][0] = (float)kr.at<double>(1, 0);
			r[1][1] = (float)kr.at<double>(1, 1);
			r[1][2] = (float)kr.at<double>(1, 2);

			r[2][0] = (float)kr.at<double>(2, 0);
			r[2][1] = (float)kr.at<double>(2, 1);
			r[2][2] = (float)kr.at<double>(2, 2);

			for (i = 0; i < size2; i++)
			{
				const float x = data[0] + tt[0];
				const float y = data[1] + tt[1];
				const float z = data[2] + tt[2];

				const float div = 1.f / (r[2][0] * x + r[2][1] * y + r[2][2] * z);

				dst->x = (r[0][0] * x + r[0][1] * y + r[0][2] * z) * div;
				dst->y = (r[1][0] * x + r[1][1] * y + r[1][2] * z) * div;

				data += 3;
				dst++;
			}
		}
	}

	void RGBHistogram::projectPoints(const Mat& xyz, const Mat& R, const Mat& t, const Mat& K, vector<Point2f>& dest, const bool isRotationThenTranspose)
	{
		float* data = (float*)xyz.ptr<float>(0);
		Point2f* dst = &dest[0];
		int size2 = xyz.size().area();
		int i;
		float tt[3];
		tt[0] = (float)t.at<double>(0, 0);
		tt[1] = (float)t.at<double>(1, 0);
		tt[2] = (float)t.at<double>(2, 0);

		float r[3][3];
		if (isRotationThenTranspose)
		{
			const float f00 = (float)K.at<double>(0, 0);
			const float xc = (float)K.at<double>(0, 2);
			const float f11 = (float)K.at<double>(1, 1);
			const float yc = (float)K.at<double>(1, 2);

			r[0][0] = (float)R.at<double>(0, 0);
			r[0][1] = (float)R.at<double>(0, 1);
			r[0][2] = (float)R.at<double>(0, 2);

			r[1][0] = (float)R.at<double>(1, 0);
			r[1][1] = (float)R.at<double>(1, 1);
			r[1][2] = (float)R.at<double>(1, 2);

			r[2][0] = (float)R.at<double>(2, 0);
			r[2][1] = (float)R.at<double>(2, 1);
			r[2][2] = (float)R.at<double>(2, 2);

			for (i = 0; i < size2; i++)
			{
				const float x = data[0];
				const float y = data[1];
				const float z = data[2];
				//const float z = 0.f;

				const float px = r[0][0] * x + r[0][1] * y + r[0][2] * z + tt[0];
				const float py = r[1][0] * x + r[1][1] * y + r[1][2] * z + tt[1];
				const float pz = r[2][0] * x + r[2][1] * y + r[2][2] * z + tt[2];

				const float div = 1.f / pz;

				dst->x = (f00*px + xc * pz) * div;
				dst->y = (f11*py + yc * pz) * div;

				data += 3;
				dst++;
			}
		}
		else
		{
			Mat kr = K * R;

			r[0][0] = (float)kr.at<double>(0, 0);
			r[0][1] = (float)kr.at<double>(0, 1);
			r[0][2] = (float)kr.at<double>(0, 2);

			r[1][0] = (float)kr.at<double>(1, 0);
			r[1][1] = (float)kr.at<double>(1, 1);
			r[1][2] = (float)kr.at<double>(1, 2);

			r[2][0] = (float)kr.at<double>(2, 0);
			r[2][1] = (float)kr.at<double>(2, 1);
			r[2][2] = (float)kr.at<double>(2, 2);

			for (i = 0; i < size2; i++)
			{
				const float x = data[0] + tt[0];
				const float y = data[1] + tt[1];
				const float z = data[2] + tt[2];

				const float div = 1.f / (r[2][0] * x + r[2][1] * y + r[2][2] * z);

				dst->x = (r[0][0] * x + r[0][1] * y + r[0][2] * z) * div;
				dst->y = (r[1][0] * x + r[1][1] * y + r[1][2] * z) * div;

				data += 3;
				dst++;
			}
		}
	}

	void RGBHistogram::projectPoint(Point3d& xyz, const Mat& R, const Mat& t, const Mat& K, Point2d& dest)
	{
		float r[3][3];
		Mat kr = K * R;
		r[0][0] = (float)kr.at<double>(0);
		r[0][1] = (float)kr.at<double>(1);
		r[0][2] = (float)kr.at<double>(2);

		r[1][0] = (float)kr.at<double>(3);
		r[1][1] = (float)kr.at<double>(4);
		r[1][2] = (float)kr.at<double>(5);

		r[2][0] = (float)kr.at<double>(6);
		r[2][1] = (float)kr.at<double>(7);
		r[2][2] = (float)kr.at<double>(8);

		float tt[3];
		tt[0] = (float)t.at<double>(0);
		tt[1] = (float)t.at<double>(1);
		tt[2] = (float)t.at<double>(2);

		const float x = (float)xyz.x + tt[0];
		const float y = (float)xyz.y + tt[1];
		const float z = (float)xyz.z + tt[2];

		const float div = 1.f / (r[2][0] * x + r[2][1] * y + r[2][2] * z);
		dest.x = (r[0][0] * x + r[0][1] * y + r[0][2] * z) * div;
		dest.y = (r[1][0] * x + r[1][1] * y + r[1][2] * z) * div;
	}

	void RGBHistogram::convertRGBto3D(Mat& src, Mat& rgb)
	{
		if (rgb.size().area() != src.size().area()) rgb.create(src.size().area(), 1, CV_32FC3);

		const Vec3f centerv = Vec3f(center.at<float>(0), center.at<float>(1), center.at<float>(2));
		if (src.depth() == CV_8U)
		{
			for (int i = 0; i < src.size().area(); i++)
			{
				rgb.at<Vec3f>(i) = Vec3f(src.at<Vec3b>(i)) - centerv;
			}
		}
		else if (src.depth() == CV_32F)
		{
			for (int i = 0; i < src.size().area(); i++)
			{
				rgb.at<Vec3f>(i) = src.at<Vec3f>(i) - centerv;
			}
		}
	}

	RGBHistogram::RGBHistogram()
	{
		center.create(1, 3, CV_32F);
		center.at<float>(0) = 127.5f;
		center.at<float>(1) = 127.5f;
		center.at<float>(2) = 127.5f;
	}

	void RGBHistogram::setCenter(Mat& src)
	{
		src.copyTo(center);
	}

	void RGBHistogram::push_back(Mat& src)
	{
		const Vec3f centerv = Vec3f(center.at<float>(0), center.at<float>(1), center.at<float>(2));
		for (int i = 0; i < src.rows; i++)
		{
			additionalPoints.push_back(Vec3f(src.at<float>(i, 0), src.at<float>(i, 1), src.at<float>(i, 2)) - centerv);
		}
	}

	void RGBHistogram::push_back(Vec3f src)
	{
		additionalPoints.push_back(src - Vec3f(127.5f, 127.5f, 127.5f));
	}

	void RGBHistogram::push_back(const float b, const float g, const float r)
	{
		push_back(Vec3f(b, g, r));
	}

	void RGBHistogram::clear()
	{
		additionalPoints.release();
	}

	void RGBHistogram::plot(Mat& src, bool isWait, string wname)
	{
		//setup gui
		namedWindow(wname);
		static int sw = 0; createTrackbar("sw", wname, &sw, 2);
		static int sw_projection = 0; createTrackbar("projection", wname, &sw_projection, 1);
		static int f = 1000;  createTrackbar("f", wname, &f, 2000);
		static int z = 1250; createTrackbar("z", wname, &z, 2000);
		static int pitch = 90; createTrackbar("pitch", wname, &pitch, 360);
		static int roll = 0; createTrackbar("roll", wname, &roll, 180);
		static int yaw = 90; createTrackbar("yaw", wname, &yaw, 360);
		static Point ptMouse = Point(cvRound((size.width - 1)*0.75), cvRound((size.height - 1)*0.25));

		cv::setMouseCallback(wname, (MouseCallback)onMouseHistogram3D, (void*)&ptMouse);

		//project RGB2XYZ
		Mat rgb;
		convertRGBto3D(src, rgb);

		Mat xyz;
		vector<Point2f> pt(rgb.size().area());

		//set up etc plots
		Mat guide, guideDest;
		guide.push_back(Point3f(-127.5f, -127.5f, -127.5f)); //rgbzerof;
		guide.push_back(Point3f(-127.5, -127.5, 127.5)); //rmax
		guide.push_back(Point3f(-127.5, 127.5, -127.5)); //gmax
		guide.push_back(Point3f(127.5, -127.5, -127.5)); //rmax
		guide.push_back(Point3f(127.5, 127.5, 127.5)); //rgbmax
		guide.push_back(Point3f(-127.5, 127.5, 127.5)); //grmax
		guide.push_back(Point3f(127.5, -127.5, 127.5)); //brmax
		guide.push_back(Point3f(127.5, 127.5, -127.5)); //bgmax

		vector<Point2f> guidept(guide.rows);
		vector<Point2f> additionalpt(additionalPoints.rows);
		int key = 0;
		Mat show = Mat::zeros(size, CV_8UC3);
		while (key != 'q')
		{
			pitch = (int)(180 * (double)ptMouse.y / (double)(size.height - 1) + 0.5);
			yaw = (int)(180 * (double)ptMouse.x / (double)(size.width - 1) + 0.5);
			setTrackbarPos("pitch", wname, pitch);
			setTrackbarPos("yaw", wname, yaw);

			//intrinsic
			k.at<double>(0, 2) = (size.width - 1)*0.5;
			k.at<double>(1, 2) = (size.height - 1)*0.5;
			k.at<double>(0, 0) = show.cols*0.001*f;
			k.at<double>(1, 1) = show.cols*0.001*f;
			t.at<double>(2) = z - 800;

			//rotate & plot RGB plots
			Mat rot;
			cp::Eular2Rotation(pitch - 90.0, roll - 90, yaw - 90, rot);
			cp::moveXYZ(rgb, xyz, rot, Mat::zeros(3, 1, CV_64F), true);
			if (sw_projection == 0) projectPointsParallel(xyz, R, t, k, pt, true);
			if (sw_projection == 1) projectPoints(xyz, R, t, k, pt, true);

			//rotate & plot guide information
			cp::moveXYZ(guide, guideDest, rot, Mat::zeros(3, 1, CV_64F), true);
			if (sw_projection == 0) projectPointsParallel(guideDest, R, t, k, guidept, true);
			if (sw_projection == 1) projectPoints(guideDest, R, t, k, guidept, true);

			//rotate & plot additionalPoint
			if (!additionalPoints.empty())
			{
				cp::moveXYZ(additionalPoints, additionalPointsDest, rot, Mat::zeros(3, 1, CV_64F), true);
				if (sw_projection == 0) projectPointsParallel(additionalPointsDest, R, t, k, additionalpt, true);
				if (sw_projection == 1) projectPoints(additionalPointsDest, R, t, k, additionalpt, true);
			}

			//draw lines for etc points
			Point rgbzero = Point(cvRound(guidept[0].x), cvRound(guidept[0].y));
			Point rmax = Point(cvRound(guidept[1].x), cvRound(guidept[1].y));
			Point gmax = Point(cvRound(guidept[2].x), cvRound(guidept[2].y));
			Point bmax = Point(cvRound(guidept[3].x), cvRound(guidept[3].y));
			Point bgrmax = Point(cvRound(guidept[4].x), cvRound(guidept[4].y));
			Point grmax = Point(cvRound(guidept[5].x), cvRound(guidept[5].y));
			Point brmax = Point(cvRound(guidept[6].x), cvRound(guidept[6].y));
			Point bgmax = Point(cvRound(guidept[7].x), cvRound(guidept[7].y));
			line(show, bgmax, bgrmax, COLOR_WHITE);
			line(show, brmax, bgrmax, COLOR_WHITE);
			line(show, grmax, bgrmax, COLOR_WHITE);
			line(show, bgmax, bmax, COLOR_WHITE);
			line(show, brmax, bmax, COLOR_WHITE);
			line(show, brmax, rmax, COLOR_WHITE);
			line(show, grmax, rmax, COLOR_WHITE);
			line(show, grmax, gmax, COLOR_WHITE);
			line(show, bgmax, gmax, COLOR_WHITE);
			circle(show, rgbzero, 3, COLOR_WHITE, cv::FILLED);
			arrowedLine(show, rgbzero, rmax, COLOR_RED, 2);
			arrowedLine(show, rgbzero, gmax, COLOR_GREEN, 2);
			arrowedLine(show, rgbzero, bmax, COLOR_BLUE, 2);
			//arrowedLine(show, rgbzero, bgrmax, Scalar::all(50), 1);


			//rendering RGB plots
			if (sw == 0)
			{
				for (int i = 0; i < src.size().area(); i++)
				{
					const int x = cvRound(pt[i].x);
					const int y = cvRound(pt[i].y);
					int inc = 2;
					if (x >= 0 && x < show.cols&&y >= 0 && y < show.rows)
					{
						show.at<Vec3b>(Point(x, y)) += Vec3b(inc, inc, inc);
					}
				}

			}
			else if (sw == 1)
			{
				for (int i = 0; i < src.size().area(); i++)
				{
					const int x = cvRound(pt[i].x);
					const int y = cvRound(pt[i].y);
					if (x >= 0 && x < show.cols&&y >= 0 && y < show.rows)
					{
						Vec3f v = rgb.at<Vec3f>(i);
						show.at<Vec3b>(Point(x, y)) = v;
					}
				}
			}
			else if (sw == 2)
			{
				for (int i = 0; i < src.size().area(); i++)
				{
					const int x = cvRound(pt[i].x);
					const int y = cvRound(pt[i].y);
					int inc = 2;
					if (x >= 0 && x < show.cols&&y >= 0 && y < show.rows)
					{
						show.at<Vec3b>(Point(x, y)) = Vec3b(128, 128, 128);
					}
				}
			}

			//rendering additional points
			if (!additionalPoints.empty())
			{
				for (int i = 0; i < additionalPoints.size().area(); i++)
				{
					const int x = cvRound(additionalpt[i].x);
					const int y = cvRound(additionalpt[i].y);

					if (x >= 0 && x < show.cols&&y >= 0 && y < show.rows)
					{
						circle(show, Point(x, y), 2, COLOR_WHITE);
					}
				}
			}

			imshow(wname, show);
			key = waitKeyEx(1);
			if (key == 'v')
			{
				sw++;
				if (sw > 2)sw = 0;
				setTrackbarPos("sw", wname, sw);
			}
			if (key == 's')
			{
				cout << "write rgb_histogram.png" << endl;
				imwrite("rgb_histogram.png", show);
			}
			if (key == 't')
			{
				ptMouse.x = cvRound((size.width - 1)*0.5);
				ptMouse.y = cvRound((size.height - 1)*0.5);
			}
			if (key == 'r')
			{
				ptMouse.x = cvRound((size.width - 1)*0.75);
				ptMouse.y = cvRound((size.height - 1)*0.25);
			}
			if (key == '?')
			{
				cout << "v: switching rendering method" << endl;
				cout << "r: reset viewing direction for parallel view" << endl;
				cout << "t: reset viewing direction for paspective view" << endl;
				cout << "s: save 3D RGB plot" << endl;
			}
			show.setTo(0);
			if (!isWait)break;
		}
	}
}