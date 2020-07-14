#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openframeworks");

	ofBackground(0);
	ofSetColor(255);
	ofEnableDepthTest();

	this->base_radius = 120;
	this->number_of_satellite = 2;
	for (int i = 0; i < 2; i++) {

		this->base_noise_seed_list.push_back(glm::vec3(ofRandom(1000), ofRandom(1000), ofRandom(1000)));

		auto s_location_list = vector<glm::vec3>();
		auto s_noise_seed_list = vector<glm::vec2>();

		for (int k = 0; k < this->number_of_satellite; k++) {

			s_location_list.push_back(glm::normalize(glm::vec3(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1))) * this->base_radius * 1.2);
			s_noise_seed_list.push_back(glm::vec2(ofRandom(1000), ofRandom(1000)));

		}

		this->satellite_location_list.push_back(s_location_list);
		this->satellite_noise_seed_list.push_back(s_noise_seed_list);
	}

	this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
}

//--------------------------------------------------------------
void ofApp::update() {

	ofColor color;
	for (int i = 0; i < this->base_noise_seed_list.size(); i++) {

		auto base_location = glm::vec3(
			ofMap(ofNoise(this->base_noise_seed_list[i].x, (ofGetFrameNum() + 0) * 0.005), 0, 1, -300, 300),
			ofMap(ofNoise(this->base_noise_seed_list[i].y, (ofGetFrameNum() + 0) * 0.005), 0, 1, -300, 300),
			ofMap(ofNoise(this->base_noise_seed_list[i].z, (ofGetFrameNum() + 0) * 0.005), 0, 1, -300, 300)
		);

		for (int k = 0; k < this->number_of_satellite; k++) {

			auto rotation_y = glm::rotate(glm::mat4(), ofMap(ofNoise(this->satellite_noise_seed_list[i][k].x, (ofGetFrameNum() + 0) * 0.005), 0, 1, -PI * 2, PI * 2), glm::vec3(0, 1, 0));
			auto rotation_x = glm::rotate(glm::mat4(), ofMap(ofNoise(this->satellite_noise_seed_list[i][k].y, (ofGetFrameNum() + 0) * 0.005), 0, 1, -PI * 2, PI * 2), glm::vec3(1, 0, 0));
			auto satellite_location = glm::vec4(this->satellite_location_list[i][k], 0) * rotation_y * rotation_x;

			auto next_base_location = glm::vec3(
				ofMap(ofNoise(this->base_noise_seed_list[i].x, (ofGetFrameNum() + 1) * 0.005), 0, 1, -300, 300),
				ofMap(ofNoise(this->base_noise_seed_list[i].y, (ofGetFrameNum() + 1) * 0.005), 0, 1, -300, 300),
				ofMap(ofNoise(this->base_noise_seed_list[i].z, (ofGetFrameNum() + 1) * 0.005), 0, 1, -300, 300)
			);

			auto next_rotation_y = glm::rotate(glm::mat4(), ofMap(ofNoise(this->satellite_noise_seed_list[i][k].x, (ofGetFrameNum() + 1) * 0.005), 0, 1, -PI * 2, PI * 2), glm::vec3(0, 1, 0));
			auto next_rotation_x = glm::rotate(glm::mat4(), ofMap(ofNoise(this->satellite_noise_seed_list[i][k].y, (ofGetFrameNum() + 1) * 0.005), 0, 1, -PI * 2, PI * 2), glm::vec3(1, 0, 0));
			auto next_satellite_location = glm::vec4(this->satellite_location_list[i][k], 0) * rotation_y * rotation_x;

			auto location = base_location + satellite_location;
			auto next_location = next_base_location + next_satellite_location;
			auto distance = location - next_location;

			auto future = glm::vec3(satellite_location) + distance * 80;
			auto random_deg_1 = ofRandom(360);
			auto random_deg_2 = ofRandom(360);
			future += glm::vec3(
				30 * cos(random_deg_1 * DEG_TO_RAD) * sin(random_deg_2 * DEG_TO_RAD),
				30 * sin(random_deg_1 * DEG_TO_RAD) * sin(random_deg_2 * DEG_TO_RAD),
				30 * cos(random_deg_2 * DEG_TO_RAD)
			);
			auto future_distance = future - satellite_location;

			this->frame.addVertex(location);
			this->face.addVertex(location);

			color.setHsb((ofGetFrameNum() * 5) % 255, 255, 255);

			this->frame.addColor(color);
			this->face.addColor(ofColor(color, 64));

			this->velocity_list.push_back(glm::normalize(future_distance) * glm::length(distance));
		}
	}

	for (int i = 0; i < this->frame.getNumVertices(); i++) {

		this->frame.setVertex(i, this->frame.getVertex(i) + this->velocity_list[i]);
		this->face.setVertex(i, this->face.getVertex(i) + this->velocity_list[i]);
	}

	for (int i = this->velocity_list.size() - 1; i >= 0; i--) {

		if (glm::length(this->frame.getVertex(i)) > 720) {

			this->velocity_list.erase(this->velocity_list.begin() + i);
			this->frame.removeVertex(i);
			this->face.removeVertex(i);
			this->frame.removeColor(i);
			this->face.removeColor(i);
		}
	}

	this->frame.clearIndices();
	this->face.clearIndices();
	for (int i = 0; i < this->frame.getNumVertices(); i++) {

		vector<int> near_index_list;
		for (int k = i + 1; k < this->frame.getNumVertices(); k++) {

			auto distance = glm::distance(this->frame.getVertex(i), this->frame.getVertex(k));
			if (distance < 50) {

				this->frame.addIndex(i); this->frame.addIndex(k);
			}

			if (distance < 80) {

				near_index_list.push_back(k);
			}
		}

		if (near_index_list.size() > 3) {

			for (int k = 0; k < near_index_list.size() - 2; k++) {

				this->face.addIndex(i);
				this->face.addIndex(near_index_list[k]);
				this->face.addIndex(near_index_list[k + 1]);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateX(ofGetFrameNum() * 0.5);
	ofRotateY(ofGetFrameNum() * 0.75);

	for (int i = 0; i < this->frame.getNumVertices(); i++) {

		ofSetColor(this->frame.getColor(i));
		ofDrawSphere(this->frame.getVertex(i), 2);
	}

	this->frame.drawWireframe();
	this->face.draw();

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}