#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

typedef unsigned char u8;

string exec(string command) {
	char buffer[128];
	string result = "";
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) throw runtime_error("popen() failed!");
	try {
		while (fgets(buffer, sizeof buffer, pipe) != NULL) {
			result += buffer;
		}
	}
	catch (...) {
		pclose(pipe);
		throw;
	}
	pclose(pipe);
	return result;
}

struct Bitmap {
private:
	string remove_extension(string filename) {
		size_t lastindex = filename.find_last_of(".");
		return filename.substr(0, lastindex);
	}
public:
	string filename;
	int width, height;
	vector<u8> pixels;
	string header;

	Bitmap() {}
	Bitmap(string filename) {
		this->filename = filename;
		ifstream file(this->filename, ios::binary);
		string line;
		getline(file, this->header);
		file >> this->width >> this->height;
		file.ignore(256, '\n');
		this->pixels.resize(this->width * this->height * 3);
		file.read(reinterpret_cast<char*>(this->pixels.data()), this->pixels.size());
	}
	u8& operator()(int x, int y, int c) {
		return this->pixels[(y * this->width + x) * 3 + c];
	}
	string get_extension() {
		size_t lastindex = this->filename.find_last_of(".");
		return this->filename.substr(lastindex + 1);
	}
	void write() {
		ofstream file(this->filename, ios::binary);
		file << "P6\n" << this->width << " " << this->height << "\n255\n";
		file.write(reinterpret_cast<char*>(this->pixels.data()), this->pixels.size());
	}
	operator string() {
		int new_width = stoi(exec("tput cols"));
		int new_height = stoi(exec("tput lines")) - 1;
		if (new_width != this->width || new_height != this->height) {
			resize(new_width, new_height);
		}
		string out = "";
		for (int i = 0; i < this->height; i++) {
			for (int j = 0; j < this->width; j++) {
				u8 rgb[3] = { operator()(j, i, 0), operator()(j, i, 1), operator()(j, i, 2) };
				out += "\033[48;2;" + to_string(rgb[0]) +
					";" + to_string(rgb[1]) +
					";" + to_string(rgb[2]) +
					"m \033[0m";
			}
			out += "\n";
		}
		out += "(" + this->filename + ":" + this->header + ":" + to_string(this->width) + "x" + to_string(this->height) + ")\n";
		return out;
	}
	void to_ppm() {
		string ppm_filename = remove_extension(this->filename) + ".ppm";
		string command = "convert " + this->filename + " " + ppm_filename;
		system(command.c_str());
		command = "rm " + this->filename;
		system(command.c_str());
		(*this) = Bitmap(ppm_filename);
	}
	void resize(int new_width, int new_height) {
		vector<u8> new_pixels(new_width * new_height * 3);
		for (int i = 0; i < new_height; i++) {
			for (int j = 0; j < new_width; j++) {
				int x = j * this->width / new_width;
				int y = i * this->height / new_height;
				for (int c = 0; c < 3; c++) {
					new_pixels[(i * new_width + j) * 3 + c] = operator()(x, y, c);
				}
			}
		}
		this->width = new_width;
		this->height = new_height;
		this->pixels = new_pixels;
	}
};

int main(int argc, char** argv) {
	string filename = "";
	if (argc > 1) {
		filename = argv[1];
	}
	else {
		cout << "Enter filename: ";
		cin >> filename;
	}
	ifstream file(filename);
	if (!file.good()) {
		cout << "File not found.\n";
		return 1;
	}

	Bitmap image = Bitmap(filename);
	if (image.get_extension() != "ppm") {
		image.to_ppm();
	}
	cout << (string)image;
	string modify_cmd = "stat -c %Y " + filename;
	int timestamp = stoi(exec(modify_cmd));
	vector<int> size = { stoi(exec("tput cols")), stoi(exec("tput lines")) - 1 };
	while (true) {
		int new_timestamp = stoi(exec(modify_cmd));
		if (new_timestamp != timestamp || size != vector<int>{stoi(exec("tput cols")), stoi(exec("tput lines")) - 1}){
			size = { stoi(exec("tput cols")), stoi(exec("tput lines")) - 1 };
			timestamp = new_timestamp;
			image = Bitmap(filename);
			system("clear");
			cout << (string)image;
		}
	}
	return 0;
}