#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include "global_variables.h"

#ifdef USE_SINGLE_PRECISION
typedef float REAL;
#else
typedef double REAL;
#endif

//This file describe one simulation timestep
//We use KDK integrator for the N-body simulation 

void forces_old(REAL** x, REAL** F);
void forces_old_periodic(REAL**x, REAL**F);
void forces_EWALD(REAL** x, REAL** F);

void step(REAL** x, REAL** F)
{
	//Timing
	REAL step_start_time = (REAL) clock () / (REAL) CLOCKS_PER_SEC;
	REAL step_omp_start_time = omp_get_wtime();
	//Timing
	int i, k;
	REAL ACCELERATION[3];
		errmax = 0;
		printf("KDK Leapfrog integration...\n");
		for(i=0; i<N; i++)
		{
			for(k=0; k<3; k++)
			{
				ACCELERATION[k] = (F[i][k]*(REAL)(pow(a_max/a, 3.0)) - 2.0*(REAL)(Hubble_param)*x[i][k+3]);
				x[i][k+3] = x[i][k+3] + ACCELERATION[k]*(REAL)(h/2.0);
				x[i][k] = x[i][k] + x[i][k+3]*(REAL)(h);
			}
		}
		//If we are using periodic boundary conditions, the code move every "out-of-box" particle inside the box
		if(IS_PERIODIC != 0)
		{
			for(i=0; i<N; i++)
			{
			for(k=0;k<3;k++)
			{
			if(x[i][k]<0)
			{
			x[i][k] = x[i][k] + L;
			}
			if(x[i][k]>=L)
			{
			x[i][k] = x[i][k] - L;
			}
			}
			}
		}
		//Force calculation
		printf("Calculating Forces...\n");
		if(IS_PERIODIC < 2)
		{
			forces_old(x, F);
		}
		if(IS_PERIODIC == 2)
		{
			forces_old_periodic(x, F);
		}
		//Stepping in scale factor and Hubble-parameter
		//if COSMOLOGY == 1, than we step in scalefactor, using the specified cosmological model
		if(COSMOLOGY == 1)
		{
			if(NONISOTROPIC_EXPANSION == 0)
			{
				a_prev2 = a_prev1;
				a_prev1 = a;
				a = friedman_solver_step(a, h, Omega_lambda, Omega_r, Omega_m, Omega_k, H0);
				recalculate_softening();
				a_tmp = a;
				Hubble_param = H0*sqrt(Omega_m*pow(a, -3)+Omega_r*pow(a, -4)+Omega_lambda+Omega_k*pow(a, -2));
				Decel_param = CALCULATE_decel_param(a, a_prev1, a_prev2, h, h_prev); //Deceleration parameter
				Omega_m_eff = Omega_m*pow(a, -3)*pow(H0/Hubble_param, 2);
			}
			if(NONISOTROPIC_EXPANSION == 1)
			{
				density_field(x, RHO, DENSITY_CELLS);
				a_prev2 = a_prev1;
				a_prev1 = a;
				delta_a = nonis_friedmann(RHO, DENSITY_CELLS);
				a += delta_a;
				recalculate_softening();
				a_tmp = a;
				Hubble_tmp = Hubble_param;
				//Calculating Hubble parameter, using second order method:
				Hubble_param = -1*(-1*h*h*a_prev2+pow((h+h_prev),2)*a_prev1 - (2*h*h_prev+pow(h_prev,2))*a)/(a*(h*h*h_prev+h*h_prev*h_prev));
				delta_Hubble_param = Hubble_param-Hubble_tmp;
				Decel_param = CALCULATE_decel_param(a, a_prev1, a_prev2, h, h_prev); //Deceleration parameter
				Omega_m_eff = Omega_m*pow(a, -3)*pow(H0/Hubble_param, 2);
				printf("Omega_m_eff = %lf\n",Omega_m_eff);
			}
			if(NONISOTROPIC_EXPANSION == 2)
			{
				get_voronoi();
				a_tmp = a;
				a_prev2 = a_prev1;
				a_prev1 = a;
				a = nonis_friedmann_voronoi(RHO, a);
				recalculate_softening();
				delta_a = a - a_tmp;
				a_tmp = a;
				Hubble_tmp = Hubble_param;
				//Calculating Hubble parameter, using second order method:
				Hubble_param = -1*(-1*h*h*a_prev2+pow((h+h_prev),2)*a_prev1 - (2*h*h_prev+pow(h_prev,2))*a)/(a*(h*h*h_prev+h*h_prev*h_prev));
				delta_Hubble_param = Hubble_param-Hubble_tmp;
				Decel_param = CALCULATE_decel_param(a, a_prev1, a_prev2, h, h_prev); //Deceleration parameter
				Omega_m_eff = Omega_m*pow(a, -3)*pow(H0/Hubble_param, 2);
				printf("Omega_m_eff = %lf\n",Omega_m_eff);
			}
			//DTFE and SPH density estimation is performed by the DTFE library
			if(NONISOTROPIC_EXPANSION == 3 || NONISOTROPIC_EXPANSION == 4) 
			{
				DTFE_density(x);
				a_prev2 = a_prev1;
				a_prev1 = a;
				delta_a = nonis_friedmann(RHO, DENSITY_CELLS);
				a += delta_a;
				recalculate_softening();
				a_tmp = a;
				Hubble_tmp = Hubble_param;
				//Calculating Hubble parameter, using second order method:
				Hubble_param = -1*(-1*h*h*a_prev2+pow((h+h_prev),2)*a_prev1 - (2*h*h_prev+pow(h_prev,2))*a)/(a*(h*h*h_prev+h*h_prev*h_prev));
				delta_Hubble_param = Hubble_param-Hubble_tmp;
				Decel_param = CALCULATE_decel_param(a, a_prev1, a_prev2, h, h_prev); //Deceleration parameter
				Omega_m_eff = Omega_m*pow(a, -3)*pow(H0/Hubble_param, 2);
				printf("Omega_m_eff = %lf\n",Omega_m_eff);
				
			}
		}
		else
		{
			//For non-cosmological simulation, taking into account the T_max
			a_tmp = T;
		}
		for(i=0; i<N; i++)
		{
			for(k=0; k<3; k++)
			{
				ACCELERATION[k] = (F[i][k]*(REAL)(pow(a_max/a, 3.0)) - 2.0*(REAL)(Hubble_param)*x[i][k+3]);
				x[i][k+3] = x[i][k+3] + ACCELERATION[k]*(REAL)(h/2.0);
			}
				err = sqrt(ACCELERATION[0]*ACCELERATION[0] + ACCELERATION[1]*ACCELERATION[1] + ACCELERATION[2]*ACCELERATION[2]);
				if(err>errmax)
				{
					errmax = err;
				}
		}
		printf("KDK Leapfrog integration...done.\n");
	//Timing
	REAL step_end_time = (REAL) clock () / (REAL) CLOCKS_PER_SEC;
	REAL step_omp_end_time = omp_get_wtime();
	//Timing
	printf("Timestep CPU time = %lfs\n", step_end_time-step_start_time);
	printf("Timestep RUN time = %lfs\n", step_omp_end_time-step_omp_start_time);

return;
}
