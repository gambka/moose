#include "ElasticityTensorR4.h"

ElasticityTensorR4::ElasticityTensorR4() :
    RankFourTensor()
{
}

ElasticityTensorR4::ElasticityTensorR4(const ElasticityTensorR4 &a) :
    RankFourTensor(a)
{
}

ElasticityTensorR4::ElasticityTensorR4(const RankFourTensor &a) :
    RankFourTensor(a)
{
}

Real
ElasticityTensorR4::elasticJacobian(const unsigned int i, const unsigned int k, const RealGradient & grad_test, const RealGradient & grad_phi) const
{
  // d(stress_ij*d(test)/dx_j)/du_k = d(C_ijmn*du_m/dx_n dtest/dx_j)/du_k (which is nonzero for m=k)
  Real the_sum = 0.0;
  for (unsigned int j = 0; j < N; ++j)
    for (unsigned int l = 0; l < N; ++l)
      the_sum += _vals[i][j][k][l] * grad_phi(l) * grad_test(j);
  return the_sum;
}

Real
ElasticityTensorR4::elasticJacobianwc(const unsigned int i, const unsigned int k, const RealGradient & grad_test, const Real & phi) const
{
  // d(stress_ij*d(test)/dx_j)/dw_k = d(C_ijmn*eps_mnp*w_p*dtest/dx_j)/dw_k (only nonzero for p=k)
  Real the_sum = 0.0;
  for (unsigned int j = 0; j < N; ++j)
    for (unsigned int m = 0; m < N; ++m)
      for (unsigned int n = 0; n < N; ++n)
        the_sum += _vals[i][j][m][n] * PermutationTensor::eps(m,n,k) * grad_test(j);
  return the_sum * phi;
}

Real
ElasticityTensorR4::momentJacobian(const unsigned int i, const unsigned int k, const Real & test, const RealGradient & grad_phi) const
{
  // Jacobian entry: d(eps_ijm*stress_jm*test)/du_k = d(eps_ijm*C_jmln*du_l/dx_n*test)/du_k (only nonzero for l=k)
  Real the_sum = 0.0;
  for (unsigned int j = 0 ; j < N ; ++j)
    for (unsigned int m = 0 ; m < N ; ++m)
      for (unsigned int n = 0 ; n < N ; ++n)
        the_sum += PermutationTensor::eps(i,j,m) *_vals[j][m][k][n] * grad_phi(n);
  return test * the_sum;
}

Real
ElasticityTensorR4::momentJacobianwc(const unsigned int i, const unsigned int k, const Real & test, const Real & phi) const
{
  // Jacobian entry: d(eps_ijm*stress_jm*test)/dw_k = d(eps_ijm*C_jmln*eps_lnp*w_p*test)/dw_k (only nonzero for p=k)
  Real the_sum = 0.0;
  for (unsigned int j = 0 ; j < N ; ++j)
    for (unsigned int l = 0 ; l < N ; ++l)
      for (unsigned int m = 0 ; m < N ; ++m)
        for (unsigned int n = 0 ; n < N ; ++n)
          the_sum += PermutationTensor::eps(i,j,m) * _vals[j][m][l][n] * PermutationTensor::eps(l,n,k);
  return test * phi * the_sum;
}
