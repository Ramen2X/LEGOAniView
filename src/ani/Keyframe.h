// Information about rotation is currently unknown

enum Type {
	POSITION,
	ROTATION,
	SCALE,
	MORPH
};

struct Keyframe {
	Type type; // Type of the keyframe
	short ms; // Time the keyframe is positioned in milliseconds
	float x_pos; // X position of the component
	float y_pos; // Y position of the component
	float z_pos; // Z position of the component
};