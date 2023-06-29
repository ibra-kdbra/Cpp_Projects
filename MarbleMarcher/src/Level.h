
#pragma once
#include <Eigen/Dense>

static const int num_levels = 24;
static const int num_fractal_params = 9;
typedef Eigen::Matrix<float, num_fractal_params, 1> FractalParams;

class Level {
public:
  Level() {}
  Level(float s, float a1, float a2,
        const Eigen::Vector3f& v,
        const Eigen::Vector3f& c,
        float rad,
        float look_x,
        float orbit_d,
        const Eigen::Vector3f& start,
        const Eigen::Vector3f& end,
        float kill,
        bool planet,
        const char* desc,
        float an1=0.0f, float an2=0.0f, float an3=0.0f);

  FractalParams params;      //Fractal parameters
  float marble_rad;          //Radius of the marble
  float start_look_x;        //Camera direction on start
  float orbit_dist;          //Distance to orbit
  Eigen::Vector3f start_pos; //Starting position of the marble
  Eigen::Vector3f end_pos;   //Ending goal flag position
  float kill_y;              //Below this height player is killed
  bool planet;               //True if gravity should be like a planet
  const char* txt;           //Description displayed before level
  float anim_1;              //Animation amount for angle1 parameter
  float anim_2;              //Animation amount for angle2 parameter
  float anim_3;              //Animation amount for offset_y parameter
};

extern const Level all_levels[num_levels];
