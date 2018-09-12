#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ERROR_NOT_VALID_KEYS_NUMBER 2
#define ERROR_NOT_VALID_SCENE_LIGHTS_NUMBER 3

#define PIANO_KEYS_WHITE_NUMBER 36
#define PIANO_KEYS_BLACK_NUMBER 25
#define SCENE_LIGHTS_NUMBER 3

enum Action_Keys {
	PIANO_OPEN_FLOP,
	PIANO_CLOSE_FLOP,
	PIANO_PUSH_KEY,
	PIANO_RELEASE_KEY
};


const float PIANO_FLOP_OPEN_ANGLE_MAX = 31.0f;
const float PIANO_STICK_OPEN_ANGLE_MAX = 42.0f;
const float PIANO_KEY_PUSH_ANGLE_MAX = 5.5f;
const float FLOP_SPEED = 0.6f;
const float KEY_SPEED = 16.0f;
const float LAMP_SPEED = 42.0f;
const float SCENE_LAMP_MAX_MOVE[3] = { 7.0f, 0.0f, 3.0f };

class Actions
{
private:
	float piano_flop_angle_percentage;
	float key_white_push_percentage[PIANO_KEYS_WHITE_NUMBER] = {};
	float key_black_push_percentage[PIANO_KEYS_BLACK_NUMBER] = {};
	glm::vec3 light_direction_move[SCENE_LIGHTS_NUMBER];
	float light_direction_rotate[SCENE_LIGHTS_NUMBER];
	float delta_time_sum = 0.0f;
	float add_to_sin = 180 / (SCENE_LIGHTS_NUMBER - 1);

public:

	Actions(float flop_open_percentage)
	{
		piano_flop_angle_percentage = flop_open_percentage;
		for (int i = 0; i < SCENE_LIGHTS_NUMBER; i++) {
			light_direction_move[i] = glm::vec3(0.0f);
		}
		check_angles();
	}

	void ProcessKeyboard(Action_Keys KEY, float deltaTime)
	{
		float velocity = FLOP_SPEED * deltaTime;
		if (KEY == PIANO_OPEN_FLOP) {
			piano_flop_angle_percentage += velocity;
		}
		if (KEY == PIANO_CLOSE_FLOP) {
			piano_flop_angle_percentage -= velocity;
		}
		
		check_piano_flop_angle();
	}

	void ProcessKeyboard(Action_Keys KEY, float deltaTime, int key, bool isWhite)
	{
		if (!is_valid_keys_number(key, isWhite)) throw ERROR_NOT_VALID_KEYS_NUMBER;
		float velocity = KEY_SPEED * deltaTime;
		if (KEY == PIANO_PUSH_KEY) {
			(isWhite)? key_white_push_percentage[key] += velocity : key_black_push_percentage[key] += velocity;
		}
		if (KEY == PIANO_RELEASE_KEY) {
			(isWhite) ? key_white_push_percentage[key] -= velocity : key_black_push_percentage[key] -= velocity;
		}

		check_piano_key_push_angles();
	}

	void move_the_lamps(float deltaTime) {
		delta_time_sum = delta_time_sum + deltaTime * LAMP_SPEED;
		if (delta_time_sum > 360.0f) delta_time_sum -= 360.0f;
		for (int i = 0; i < SCENE_LIGHTS_NUMBER; i++) {
			light_direction_rotate[i] = delta_time_sum + add_to_sin*i; 
			light_direction_move[i].x = SCENE_LAMP_MAX_MOVE[0] * glm::sin(glm::radians(light_direction_rotate[i]));
		}
	}

	glm::vec3 get_light_direction_move(int lamp) {
		if (lamp < 0 || lamp >= SCENE_LIGHTS_NUMBER) throw ERROR_NOT_VALID_SCENE_LIGHTS_NUMBER;
		return light_direction_move[lamp];
	}

	float get_light_direction_angle(int lamp) {
		if (lamp < 0 || lamp >= SCENE_LIGHTS_NUMBER) throw ERROR_NOT_VALID_SCENE_LIGHTS_NUMBER;
		return light_direction_rotate[lamp];
	}

	float get_flop_angle() {
		return piano_flop_angle_percentage*PIANO_FLOP_OPEN_ANGLE_MAX;
	}

	float get_stick_angle() {
		return (piano_flop_angle_percentage*piano_flop_angle_percentage)*PIANO_STICK_OPEN_ANGLE_MAX;
	}

	float get_piano_key_angle(int key, bool isWhite) {
		if (!is_valid_keys_number(key, isWhite)) throw ERROR_NOT_VALID_KEYS_NUMBER;
		if(isWhite) return key_white_push_percentage[key] * PIANO_KEY_PUSH_ANGLE_MAX;
		else return key_black_push_percentage[key] * PIANO_KEY_PUSH_ANGLE_MAX;
	}

private:
	bool is_valid_keys_number(int number, bool isWhite) {
		if ((isWhite && (number >= PIANO_KEYS_WHITE_NUMBER)) || (!isWhite && number >= PIANO_KEYS_BLACK_NUMBER) || number < 0) return false;
		else return true;
	}

	void check_angles() {
		check_piano_flop_angle();
		check_piano_key_push_angles();
	}

	void check_piano_flop_angle() {
		if (piano_flop_angle_percentage < 0.0f) {
			piano_flop_angle_percentage = 0.0f;
		}
		if (piano_flop_angle_percentage > 1.0) {
			piano_flop_angle_percentage = 1.0;
		}
	}

	void check_piano_key_push_angles() {
		for (int i = 0; i < PIANO_KEYS_WHITE_NUMBER; i++) {
			if (key_white_push_percentage[i] < 0.0f) {
				key_white_push_percentage[i] = 0.0f;
			}
			if (key_white_push_percentage[i] > 1.0) {
				key_white_push_percentage[i] = 1.0;
			}
		}

		for (int i = 0; i < PIANO_KEYS_BLACK_NUMBER; i++) {
			if (key_black_push_percentage[i] < 0.0f) {
				key_black_push_percentage[i] = 0.0f;
			}
			if (key_black_push_percentage[i] > 1.0) {
				key_black_push_percentage[i] = 1.0;
			}
		}
	}
};