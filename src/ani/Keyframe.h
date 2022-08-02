enum Type {
	POSITION,
	ROTATION,
	SCALE,
	MORPH
};

struct Keyframe {
	Type type; // Type of the keyframe
	int num; // Which keyframe number this is
	short ms; // Time the keyframe is positioned in milliseconds
	float w; // Rotation angle value used in a quaternion
	float x; // X value of the component
	float y; // Y value of the component
	float z; // Z value of the component
};