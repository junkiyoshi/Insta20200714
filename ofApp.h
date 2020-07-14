#pragma once
#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key) {};
	void keyReleased(int key) {};
	void mouseMoved(int x, int y) {};
	void mouseDragged(int x, int y, int button) {};
	void mousePressed(int x, int y, int button) {};
	void mouseReleased(int x, int y, int button) {};
	void windowResized(int w, int h) {};
	void dragEvent(ofDragInfo dragInfo) {};
	void gotMessage(ofMessage msg) {};

	ofEasyCam cam;

	int base_radius;
	int number_of_satellite;

	vector<glm::vec3> base_noise_seed_list;
	vector<vector<glm::vec3>> satellite_location_list;
	vector<vector<glm::vec2>> satellite_noise_seed_list;
	vector<glm::vec3> location_list;
	vector<glm::vec3> velocity_list;

	ofMesh face, frame;
};