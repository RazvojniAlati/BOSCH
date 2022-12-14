/**
 * Copyright (c) 2019, Bosch Engineering Center Cluj and BFMC organizers
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.

 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.

 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*/

#ifndef SISO_CONTROLLERS_TPP
#define SISO_CONTROLLERS_TPP


#ifndef SISO_CONTROLLERS_H
#error __FILE__ should only be included from sisocontrollers.hpp.
#endif // SISO_CONTROLLERS_H

/** @brief CPidController class constructor
  *
  * Construct a new discrete pid controller, it requires the pid parameters to calculate the discrete transferfunction. 
  *
  * @param f_kp                proportional factor
  * @param f_ki                integral factor
  * @param f_kd                derivative factor
  * @param f_tf                derivative time filter constant
  * @param f_dt                sampling time
  */
template<class T>
CPidController<T>::CPidController( T              f_kp
                                  ,T              f_ki
                                  ,T              f_kd
                                  ,T              f_tf
                                  ,T              f_dt)
    :m_pidTf()
{    
    // Calculate the coefficients for the discrete transferfunction based an Euler backward discretisation method. 
    utils::linalg::CMatrix<T,1,3> l_numPid({ (f_kd+f_tf*f_kp)/f_tf , (f_tf*f_dt*f_ki+f_dt*f_kp-2*f_tf*f_kp-2*f_kd)/f_tf , (f_kd+f_tf*f_kp+f_dt*f_dt*f_ki-f_dt*f_tf*f_ki-f_dt*f_kp)/f_tf});
    utils::linalg::CMatrix<T,1,3> l_denPid({ 1.0,-(2*f_tf-f_dt)/f_tf,(f_tf-f_dt)/f_tf });

    m_pidTf.setNum(l_numPid.transpose());
    m_pidTf.setDen(l_denPid.transpose());
}


/**
 * @brief CPidController class constructor
 * 
 * Construct a new discrete pid controller based the given discrete transferfunction. 
 *  
 * @param f_pid               discrete transferfunction
 * @param f_dt                sampling time
 */
template<class T>
CPidController<T>::CPidController(  CPidSystemmodelType             f_pid
                                    ,T                              f_dt)
                                    :m_pidTf(f_pid)
                                    ,m_dt(f_dt){
}


/** @brief  Control signal generator
  *
  *   It calculate the control signal based on the given input error. It has to be applied in each period. 
  * 
  * @param f_input             input error
  * \return                    control value
  */
template<class T>
T CPidController<T>::calculateControl(const T& f_input)
{
    return m_pidTf(f_input);
}

/** @brief  Reset to zero all memory of the controller.
  *
  */
template<class T>
void CPidController<T>::clear()
{
    m_pidTf.clearMemmory();
}

/** @brief  Set the parameter of the controller
  *
  *
  * @param f_kp                proportional factor
  * @param f_ki                integral factor
  * @param f_kd                derivative factor
  * @param f_tf                derivative time filter constant
  */
template<class T>
void CPidController<T>::setController(
    T              f_kp,
    T              f_ki,
    T              f_kd,
    T              f_tf)
{
    utils::linalg::CMatrix<T,1,3> l_numPid({ (f_kd+f_tf*f_kp)/f_tf , (f_tf*m_dt*f_ki+m_dt*f_kp-2*f_tf*f_kp-2*f_kd)/f_tf , (f_kd+f_tf*f_kp+m_dt*m_dt*f_ki-m_dt*f_tf*f_ki-m_dt*f_kp)/f_tf });
    utils::linalg::CMatrix<T,1,3> l_denPid({ 1,-(2*f_tf-m_dt)/f_tf,(f_tf-m_dt)/f_tf });
    
    m_pidTf.setNum(l_numPid.transpose());
    m_pidTf.setDen(l_denPid.transpose());
}

/** @brief  Serial callback method  for setting controller to values received. The first string has to contains the parameters 
 * (in order proportional, integral, derivative).
  *
  *
  * @param  a                   string to read data from
  * @param b                    string to write data to
  */
template<class T>
void CPidController<T>::serialCallbackTUNEPIDcommand(char const * a, char * b)
{
    float l_kp,l_ki,l_kd,l_tf;
    uint32_t l_res = sscanf(a,"%f;%f;%f;%f;",&l_kp,&l_ki,&l_kd,&l_tf);
    if (4 == l_res)
    {
        setController(l_kp,l_ki,l_kd,l_tf);
        sprintf(b,"ack;;");
    }
    else
    {
        sprintf(b,"sintax error;;");
    }
}

#endif