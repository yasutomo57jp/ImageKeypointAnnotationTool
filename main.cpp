#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

namespace fs = boost::filesystem;
const int CIRCLE_RADIUS = 3;
typedef std::vector<std::vector<cv::Point>> AnnoType;

AnnoType annotations;
int current_index;

void annotating(int event, int x, int y, int flags, void *params){
	if(event != cv::EVENT_LBUTTONUP) return;

	std::cout << "(x, y) = (" << x << ", " << y << ")" << std::endl;
	cv::Point pos(x, y);
	annotations[current_index].push_back(pos);
}

void load_annotation(const std::string& inputfile, AnnoType& annotations){
	std::ifstream ifs(inputfile);
	if(! ifs){
		std::cerr << "Input file is incorrect" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string line;
	while(getline(ifs, line)){
		std::istringstream iss(line);
		std::string token;
		std::vector<cv::Point> anno;
		while(getline(iss, token, ',')){
			cv::Point pos;
			pos.x = std::stoi(token);
			getline(iss, token, ',');
			pos.y = std::stoi(token);

			anno.push_back(pos);
		}
		annotations.push_back(anno);
	}
}

void save_annotation(const AnnoType& annotations, const std::string& outputfile){
	std::ofstream ofs(outputfile);
	for(auto &annotation: annotations){
		if(annotation.size() != 0) ofs << annotation[0].x << "," << annotation[0].y;
		for(int i=1; i<annotation.size(); i++){
			ofs << "," << annotation[i].x << "," << annotation[i].y;
		}
		ofs << std::endl;
	}
}

int main(int argc, char **argv){
	if(argc < 3){
		std::cerr << "usage: " << argv[0] << " images_dir outputfile" << std::endl;
		return EXIT_FAILURE;
	}

	std::string images_dir, outputfile, inputfile="";
	if(argc == 3){
		images_dir = argv[1];
		outputfile = argv[2];
	}else if(argc == 4){
		images_dir = argv[1];
		inputfile = argv[2];
		outputfile = argv[3];
	}

	fs::directory_iterator dit(images_dir), dend;
	std::vector<fs::path> images;
	for(; dit != dend; dit++){
		auto p = dit->path();
		if(p.extension() != ".png" && p.extension() != ".jpg" && p.extension() != ".bmp") continue;
		images.push_back(*dit);
	}
	std::sort(images.begin(), images.end());
	if(images.size() == 0){
		std::cerr << "No images are found" << std::endl;
		return EXIT_FAILURE;
	}

	if(inputfile==""){
		annotations.resize(images.size(), std::vector<cv::Point>(0));
	}else{
		load_annotation(inputfile, annotations);
	}
	cv::namedWindow("image", 0);

	int prev_index = -1;
	int key = 0;
	bool is_loop = true;
	cv::Mat img;
	cv::setMouseCallback("image", annotating);
	while(is_loop){
		if(current_index != prev_index){
			auto p = images[current_index];
			img = cv::imread(p.string());
			if(img.empty()){
				std::cerr << "loading " << p.string() << " failed" << std::endl;
				return EXIT_FAILURE;
			}
			prev_index = current_index;
		}

		auto& annotation = annotations[current_index];
		for(int i=0; i<annotation.size(); i++){
			auto color = cv::Scalar(i*255/annotation.size(), 255, 255);
			cv::circle(img, annotation[i], CIRCLE_RADIUS, color, -1);
		}

		cv::imshow("image", img);
		key = cv::waitKey(100);

		switch(key){
			case 'j':
			case 'f':
			case ' ':
			case 83:
				current_index = std::min<int>(++current_index, images.size()-1);
				break;
			case 'k':
			case 'b':
			case 81:
				current_index = std::max<int>(0, --current_index);
				break;
			case 8:
				if(annotation.size()>0){
					annotation.pop_back();
					img = cv::imread(images[current_index].string());
				}
				break;
			case 'q':
			case 27:
				is_loop = false;
				break;
		}
	}

	save_annotation(annotations, outputfile);
	return EXIT_SUCCESS;
}
