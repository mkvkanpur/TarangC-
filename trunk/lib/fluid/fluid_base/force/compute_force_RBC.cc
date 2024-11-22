/* Tarang-2
 *
 * Copyright (C) 2008, 2009  Mahendra K. Verma
 *
 * Mahendra K. Verma
 * Indian Institute of Technology, Kanpur-208016
 * UP, India
 *
 * mkv@iitk.ac.in
 *
 * This file is part of Tarang-2 .
 *
 * Tarang-2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * Tarang-2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Tarang-2; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, U
 */
/*! \file  compute_force_RB.cc
 * 
 * @brief Force for RB convection
 *
 *		For Pr_large, u_small:		 F1 = Ra*Pr*T(k);  
 *		For Pr_large, u_large:		 F1 = Ra*T(k);    	
 *		For Pr_small or 0, u_small:  F1 = Ra*T(k);	   
 *		For Pr_small or 0, u_large:  F1 = Pr*T(k)
 *
 *		For Pr_large				Ftheta = V1;    	
 *		For Pr_small				Ftheta = V1/Pr;	   
 *		For Pr = 0					Ftheta = 0.
 *
 * @author  M. K. Verma
 * @version 4.0 MPI
 * @date Dec. 2008
 *
 * @bug  No known bugs
 */
 
#include "FORCE.h"


//*********************************************************************************************


void FORCE::Compute_force_RBC_basic(FluidVF& U, FluidSF& T)
{
	
	if (global.program.basis_type == "ChFF") { // box size (2,Ly,Lz)
		// For the velocity field
		if (global.PHYSICS.Pr_option == "PRLARGE") {
			
			if (global.PHYSICS.Uscaling == "USMALL")
				U.Force1 += (Real (0.125*global.PHYSICS.Rayleigh*global.PHYSICS.Prandtl))*(T.csf.F);	// (u.grad)u-Ra*Pr*theta
			
			else if (global.PHYSICS.Uscaling  == "ULARGE")
				U.Force1 +=  T.csf.F;					// (u.grad)u-theta
		}
		
		else if ((global.PHYSICS.Pr_option == "PRSMALL") || (global.PHYSICS.Pr_option == "PRZERO")) {
			
			if (global.PHYSICS.Uscaling == "USMALL")
				U.Force1 += (global.PHYSICS.Rayleigh)* (T.csf.F);				// (u.grad)u-theta
			
			else if (global.PHYSICS.Uscaling == "ULARGE")
				U.Force1 += (global.PHYSICS.Prandtl)*(T.csf.F);				// (u.grad)u-theta
		}
		
		else if (global.PHYSICS.Pr_option == "PRINFTY") {
			;  // Force only Temperature; do nothing here
		}
        
		
		// For the temperature field
		
		if (global.PHYSICS.Pr_option != "PRZERO")  {
			if (global.PHYSICS.Pr_option == "PRLARGE")
                T.Force += (U.cvf.V1)/TWO;
			
			// F(T) = globalvar_temperature_grad * ux(k)
			// globalvar_temperature_grad = +1 for RB, and -1 for stratified flows
			
			else if (global.PHYSICS.Pr_option == "PRSMALL") {
				T.Force +=  (Real (1/(2*global.PHYSICS.Prandtl)))*(U.cvf.V1);
			}
			//F(T) =  globalvar_temperature_grad * ux(k)/Pr
			
			else if (global.PHYSICS.Pr_option == "PRINFTY")
                T.Force +=   (U.cvf.V1)/TWO;
			
			if (global.PHYSICS.temperature_grad != 1)
				T.Force += (Real (global.PHYSICS.temperature_grad))*T.Force;
		}
    }
    
    else {
		// For the velocity field
		if (global.PHYSICS.Pr_option == "PRLARGE") {
			
			if (global.PHYSICS.Uscaling == "USMALL") 		
				U.Force1 +=  (global.PHYSICS.Rayleigh*global.PHYSICS.Prandtl)*(T.csf.F);	// (u.grad)u-Ra*Pr*theta
			
			else if (global.PHYSICS.Uscaling  == "ULARGE") 		
				U.Force1 +=  T.csf.F;					// (u.grad)u-theta			
		}
		
		else if ((global.PHYSICS.Pr_option == "PRSMALL") || (global.PHYSICS.Pr_option == "PRZERO")) {
			
			if (global.PHYSICS.Uscaling == "USMALL") 
				U.Force1 += (global.PHYSICS.Rayleigh)* (T.csf.F);				// (u.grad)u-theta
			
			else if (global.PHYSICS.Uscaling == "ULARGE") 		
				U.Force1 += (global.PHYSICS.Prandtl)*(T.csf.F);				// (u.grad)u-theta
		}
		
		else if (global.PHYSICS.Pr_option == "PRINFTY") {
			;  // Force only Temperature; do nothing here
		}

		
		// For the temperature field

		//if (global.PHYSICS.Pr_option == "PRZERO") 
		//	T.Force = 0.0;  // No point in adding Zero
		// Do nothing; Nonlinear term (u.grad)T does not exist- see single-time step
		
		if (global.PHYSICS.Pr_option != "PRZERO")   {
			if (global.PHYSICS.Pr_option == "PRLARGE")
			T.Force += (U.cvf.V1);
			
			// F(T) = globalvar_temperature_grad * ux(k)	
			// globalvar_temperature_grad = +1 for RB, and -1 for stratified flows
			
			else if (global.PHYSICS.Pr_option == "PRSMALL") {
				T.Force += (1/global.PHYSICS.Prandtl)*(U.cvf.V1);
			}
			//F(T) =  globalvar_temperature_grad * ux(k)/Pr
			
			else if (global.PHYSICS.Pr_option == "PRINFTY")
					T.Force +=  (U.cvf.V1);
			
			if (global.PHYSICS.temperature_grad != 1)  // for stratififed flows
				T.Force += (Real (global.PHYSICS.temperature_grad))*T.Force;
		}
	}
			
}

void FORCE::Compute_force_RBC(FluidVF& U, FluidSF& T)
{
	Compute_force_RBC_basic(U, T);
}


//*********************************************************************************************


void FORCE::Compute_force_RBC(FluidVF& U, FluidVF& W, FluidSF& T)
{
	Compute_force_RBC_basic(U, T);
	
	if (W.force_switch) {
		W.Force1 = 0.0; 
		W.Force2 = 0.0; 
		W.Force3 = 0.0;	
	}
}


//*********************************************************************************************
//*********************************************************************************************


void FORCE::Compute_force_RBC_rotation(FluidVF& U, FluidSF& T)
{
	
	Compute_force_RBC_basic(U, T);
	Compute_force_Coriolis(U);
	
}

//*********************************************************************************************
void FORCE::Compute_force_RBC_rotation(FluidVF& U, FluidVF& W, FluidSF& T)
{
	Compute_force_RBC_rotation(U, T);
}


//*********************************************************************************************
void FORCE::Compute_force_stratified_random(FluidVF& U, FluidSF& T)
{	
	Real inner_radius =  global.force.double_para(0);
	Real outer_radius = global.force.double_para(1);
 
    //Compute_force_RBC(U, T); //For startified


	if (global.PHYSICS.Uscaling == "USMALL"){
		U.Force1 = (global.PHYSICS.Prandtl*global.PHYSICS.Rayleigh)*(T.csf.F);
		T.Force = -U.cvf.V1;
	}
	else if (global.PHYSICS.Uscaling  == "ULARGE"){
		U.Force1 = T.csf.F;
		T.Force = -U.cvf.V1;
	}
    
//    Compute_force_using_random_noise_basic_add(U, inner_radius, outer_radius, force_spectrum_amplitude, force_spectrum_exponent, hk_by_kek);
    
}

//************************ End of compute_force_RB.cc *****************************************

