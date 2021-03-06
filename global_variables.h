#define pi 3.14159265358979323846264338327950288419716939937510

#ifdef USE_SINGLE_PRECISION
typedef float REAL;
#else
typedef double REAL;
#endif

extern int IS_PERIODIC; //periodic boundary conditions, 0=none, 1=nearest images, 2=ewald forces
extern int COSMOLOGY; //Cosmological Simulation, 0=no, 1=yes
extern REAL L; //Size of the simulation box
extern char IC_FILE[1024]; //input file
extern char OUT_DIR[1024]; //output directory
extern int IC_FORMAT; // 0: ascii, 1:GADGET
extern int LOC_CURV; // Using local (=1) or global (=0) curvature in backreaction simulations



extern int e[2202][4]; //ewald space
extern int H[2202][4]; //ewald space

extern REAL x4, err, errmax, mean_err; //variables used for error calculations
extern double h, h_min, h_max; //actual stepsize, minimal and maximal stepsize
extern double a_max,t_bigbang; //maximal scalefactor; Age of Big Bang

extern double FIRST_T_OUT, H_OUT; //First output time, output frequency in Gy

extern int RESTART; //Restarted simulation(0=no, 1=yes)
extern double T_RESTART; //Time of restart
extern double A_RESTART; //Scalefactor at the time of restart
extern double H_RESTART; //Hubble-parameter at the time of restart

extern REAL M; //Particle mass
extern int N; //Number of particles
extern REAL** x; //particle coordinates and velocities
extern REAL** F; //Forces
extern REAL w[3]; //Parameters for smoothing in force calculation
extern REAL beta; //Particle radii
extern REAL ParticleRadi; //Particle radii; readed from parameter file
extern REAL SOFT_CONST[8]; //Parameters for smoothing in force calculation

extern REAL G; //Newtonian gravitational constant

//Cosmological parameters
extern double Omega_b,Omega_lambda,Omega_dm,Omega_r,Omega_k,Omega_m,H0,H0_start,Hubble_param, Decel_param, delta_Hubble_param, Hubble_tmp;
extern double rho_crit; //Critical density
extern double a, a_start, a_prev, a_prev1, a_prev2, a_tmp; //Scalefactor, scalefactor at the starting time, previous scalefactor
extern double T, h_prev, delta_a, Omega_m_eff; //Physical time, previous timestep length, change of scalefactor, effectve Omega_m
extern int NONISOTROPIC_EXPANSION; //inhomogeneous expansion (Backreaction) (0=standard cosmology, 1=naiv backreaction with boxes, 2=backreaction with voronoi-cells, 3=DTFE backreaction, 4=naive SPH backreaction)

//Variables for the inhomogeneous "Friedmann-equation" integrator
extern int DENSITY_CELLS;
extern double* RHO;
extern double* vol_cell;
extern float DTFE_MEMORY;

//Functions
//Functions used for the Friedmann-equation
double friedman_solver_step(double a0, double h, double Omega_lambda, double Omega_r, double Omega_m, double Omega_k, double H0);
void recalculate_softening();
//Functions for the  naive box backreaction
void density_field(REAL **x, double* RHO, int DENSITY_CELLS);
double nonis_friedmann(double* RHO, int DENSITY_CELLS);
//Function for the Voronoi-cell backreaction
void get_voronoi();
double nonis_friedmann_voronoi(double* RHO, double a_prev);
//Functions for the DTFE backreaction
void DTFE_density(REAL** x);
//This function calculates the deceleration parameter
double CALCULATE_decel_param(double a, double a_prev1, double a_prev2, double h, double h_prev);
