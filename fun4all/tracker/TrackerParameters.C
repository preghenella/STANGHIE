#pragma once

/** GENERATOR **/
int GENERATOR_pid = 211;
double GENERATOR_etamin = -1.0;
double GENERATOR_etamax = 1.0;
double GENERATOR_pmin = 0.0;
double GENERATOR_pmax = 10.0;
double GENERATOR_phimin = 0.;
double GENERATOR_phimax = 2. * M_PI;

/** field MAGNET **/
double MAGNET_field = 0.2; // T

/** beam PIPE **/
double PIPE_thickness = 500.e-4;
double PIPE_radius = 2.9;//1.6;
double PIPE_length = 100;

/** INNER tracker **/
const int INNER_layers = 3;
double INNER_thickness = 50.e-4;
double INNER_radius[INNER_layers] = {1.8, 2.8, 3.8};
double INNER_length[INNER_layers] = {1.8, 2.8, 3.8};  // -1 use eta coverage to determine length
double INNER_resolution_r = 1.e-4;//INNER_thickness / sqrt(12.);
double INNER_resolution_rphi = 1.e-4;
double INNER_resolution_z = 1.e-4;

/** OUTER tracker **/
const int OUTER_layers = 6;
double OUTER_thickness = 500.e-4;
double OUTER_radius[OUTER_layers] = {8.0, 20.0, 25.0, 40.0, 55.0, 80.};
double OUTER_length[OUTER_layers] = {8.0, 20.0, 25.0, 40.0, 55.0, 80.};
double OUTER_resolution_r = 5.e-4;//OUTER_thickness / sqrt(12.);
double OUTER_resolution_rphi = 5.e-4;
double OUTER_resolution_z = 5.e-4;

/** TOF tracker **/
double TOF_thickness = 500.e-4;
double TOF_radius = 100.0;
double TOF_length = 100.0;
double TOF_resolution_r = 5.e-4;//TOF_thickness / sqrt(12.);
double TOF_resolution_rphi = 5.e-4;
double TOF_resolution_z = 5.e-4;

