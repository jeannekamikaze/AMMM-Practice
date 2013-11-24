#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;
using namespace cv;

class ParseDatFile {
public:

	int size;
	string path;
	ifstream buffer;

	ParseDatFile (string _path) : path(_path){};

	void parseDandZ(Mat& delta, Mat& w) {
		vector<Mat> matrices = parseMatrices();
		if(matrices.size() != 3) throw "we should have exacly 3 matrices in the .dat file";
		delta = matrices[0];
		merge(vector<Mat>(matrices.begin()+1,matrices.end()), w);	
	}

private:

	vector<Mat> parseMatrices() {
		vector<Mat> matrices;
		string output;
		buffer.open(path.c_str(),std::ifstream::in);
		if(buffer.is_open()){
			while(!buffer.eof()) {
				Mat a = parseMatrix();
				matrices.push_back(a);
				lookaheadS();
			}
		}
		return matrices;
	}

	char lookaheadC() {
		char c = ' ';
		if(!buffer.eof()) buffer >> c;
		return c;
	}

	string lookaheadS() {
		string s = "";
		if(!buffer.eof()) buffer >> s;
		return s;
	}

	Mat parseMatrix() {

		Mat matrix;

		while(lookaheadC() != '[');

		char aux = lookaheadC();
		while(aux != ']') {
			if(aux == '[') {
				vector<float> row = parseRow();
				Mat aux(1, row.size(), CV_32FC1, &row[0]);
				matrix.push_back(aux);
			}
			aux = lookaheadC();
		}
		if(lookaheadC() != ';') throw "missing ';' at the end of a matrix";
		return matrix;
	}

	vector<float> parseRow() {
		vector<float> row;
		string atom = lookaheadS();
		while(atom.find("]") == std::string::npos) {
			float aux = atof (atom.c_str());
			row.push_back(aux);
			atom = lookaheadS();
		}
		atom = atom.substr(0, atom.size()-1);
		float aux = atof (atom.c_str());
		row.push_back(aux);
		return row;
	}
};